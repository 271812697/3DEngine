#pragma once
#include "Opengl/scene/scene.h"
namespace scene {
    class Scene05 : public Scene {

        using Scene::Scene;
        std::shared_ptr<FBO>render_Fbo;

        void Init() override;
        void OnSceneRender(float dt) override;
        void OnImGuiRender() override;

        asset_ref<Texture> irradiance_map;
        asset_ref<Texture> prefiltered_map;
        asset_ref<Texture> BRDF_LUT;
        Entity camera;
        Entity skybox;
        Entity point_light;
        Entity spotlight;
        Entity moonlight;
        Entity floor;
        Entity wall;
        Entity ball[3];
        Entity suzune;
        Entity mingyue;
        Entity korean_fire;
        void PrecomputeIBL(const std::string& hdri);
        void SetupMaterial(Material& mat, int id);
    public :
        void setRenderFBO(std::shared_ptr<FBO>fbo);

    };
}