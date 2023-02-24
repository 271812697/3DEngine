#include"OvEditor/Utils/ext.h"
#include "Opengl//pch.h"
#include "Opengl//core/base.h"
#include "Opengl//core/sync.h"
#include "Opengl//component/all.h"
#include "Opengl//scene/renderer.h"
#include "Opengl//util/ext.h"
#include "Opengl//util/math.h"
#include "Opengl/./util/path.h"
namespace Ext {
    using namespace core;
    using namespace asset;
    using namespace utils;
	std::tuple< asset_ref<Texture>, asset_ref<Texture>, asset_ref<Texture>> PrecomputeIBL(const std::string& hdri) {

        auto irradiance_shader = CShader("Resource\\Shader\\irradiance_map.glsl");
        auto prefilter_shader = CShader("Resource\\Shader\\prefilter_envmap.glsl");
        auto envBRDF_shader = CShader("Resource\\Shader\\environment_BRDF.glsl");

        auto env_map = MakeAsset<Texture>(hdri, 2048, 0);
        env_map->Bind(0);

        auto irradiance_map = MakeAsset<Texture>(GL_TEXTURE_CUBE_MAP, 128, 128, 6, GL_RGBA16F, 1);
        auto prefiltered_map = MakeAsset<Texture>(GL_TEXTURE_CUBE_MAP, 2048, 2048, 6, GL_RGBA16F, 8);
        auto BRDF_LUT = MakeAsset<Texture>(GL_TEXTURE_2D, 1024, 1024, 1, GL_RGBA16F, 1);

       // CORE_INFO("Precomputing diffuse irradiance map from {0}", hdri);
        irradiance_map->BindILS(0, 0, GL_WRITE_ONLY);

        if (irradiance_shader.Bind(); true) {
            irradiance_shader.Dispatch(128 / 32, 128 / 32, 6);
            irradiance_shader.SyncWait(GL_TEXTURE_FETCH_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);

            auto irradiance_fence = Sync(0);
            irradiance_fence.ClientWaitSync();
            irradiance_map->UnbindILS(0);
        }

        //CORE_INFO("Precomputing specular prefiltered envmap from {0}", hdri);
        Texture::Copy(*env_map, 0, *prefiltered_map, 0);  // copy the base level

        const GLuint max_level = prefiltered_map->n_levels - 1;
        GLuint resolution = prefiltered_map->width / 2;
        prefilter_shader.Bind();

        for (unsigned int level = 1; level <= max_level; level++, resolution /= 2) {
            float roughness = level / static_cast<float>(max_level);
            GLuint n_groups = glm::max<GLuint>(resolution / 32, 1);

            prefiltered_map->BindILS(level, 1, GL_WRITE_ONLY);
            prefilter_shader.SetUniform(0, roughness);
            prefilter_shader.Dispatch(n_groups, n_groups, 6);
            prefilter_shader.SyncWait(GL_TEXTURE_FETCH_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);

            auto prefilter_fence = Sync(level);
            prefilter_fence.ClientWaitSync();
            prefiltered_map->UnbindILS(1);
        }

       
        BRDF_LUT->BindILS(0, 2, GL_WRITE_ONLY);

        if (envBRDF_shader.Bind(); true) {
            envBRDF_shader.Dispatch(1024 / 32, 1024 / 32, 1);
            envBRDF_shader.SyncWait(GL_ALL_BARRIER_BITS);
            Sync::WaitFinish();
            BRDF_LUT->UnbindILS(2);
        }
        //return std::tuple< asset_ref<Texture>, asset_ref<Texture>, asset_ref<Texture>>( irradiance_map ,prefiltered_map, BRDF_LUT);
        return { irradiance_map ,prefiltered_map, BRDF_LUT };
	}

}