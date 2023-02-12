#pragma once
#include <vector>
#include "Quad.h"
#include "Program.h"
#include"Opengl/asset/fbo.h"
#include"Opengl/asset/shader.h"
#include "../math/Vec2.h"
#include "../math/Vec3.h"
namespace GLSLPT
{
    Program* LoadShaders(const ShaderInclude::ShaderSource& vertShaderObj, const ShaderInclude::ShaderSource& fragShaderObj);

    struct RenderOptions
    {
        RenderOptions()
        {
            renderResolution = iVec2(1280, 720);
            windowResolution = iVec2(1280, 720);
            uniformLightCol = Vec3(0.3f, 0.3f, 0.3f);
            backgroundCol = Vec3(1.0f, 1.0f, 1.0f);
            tileWidth = 100;
            tileHeight = 100;
            maxDepth = 2;
            maxSpp = -1;
            RRDepth = 2;
            texArrayWidth = 2048;
            texArrayHeight = 2048;
            denoiserFrameCnt = 20;
            enableRR = true;
            enableDenoiser = false;
            enableTonemap = true;
            enableAces = false;
            openglNormalMap = true;
            enableEnvMap = false;
            enableUniformLight = false;
            hideEmitters = false;
            enableBackground = false;
            transparentBackground = false;
            independentRenderSize = false;
            enableRoughnessMollification = false;
            enableVolumeMIS = false;
            envMapIntensity = 1.0f;
            envMapRot = 0.0f;
            roughnessMollificationAmt = 0.0f;
        }

        iVec2 renderResolution;
        iVec2 windowResolution;
        Vec3 uniformLightCol;
        Vec3 backgroundCol;
        int tileWidth;
        int tileHeight;
        int maxDepth;
        int maxSpp;
        int RRDepth;
        int texArrayWidth;
        int texArrayHeight;
        int denoiserFrameCnt;
        bool enableRR;
        bool enableDenoiser;
        bool enableTonemap;
        bool enableAces;
        bool simpleAcesFit;
        bool openglNormalMap;
        bool enableEnvMap;
        bool enableUniformLight;
        bool hideEmitters;
        bool enableBackground;
        bool transparentBackground;
        bool independentRenderSize;
        bool enableRoughnessMollification;
        bool enableVolumeMIS;
        float envMapIntensity;
        float envMapRot;
        float roughnessMollificationAmt;
    };

    class Scene;

    class Renderer
    {
    protected:
        Scene* scene;
        Quad* quad;

        // Opengl buffer objects and textures for storing scene data on the GPU
        GLuint BVHBuffer;
        GLuint BVHTex;
        GLuint vertexIndicesBuffer;
        GLuint vertexIndicesTex;
        GLuint verticesBuffer;
        GLuint verticesTex;
        GLuint normalsBuffer;
        GLuint normalsTex;
        GLuint materialsTex;
        GLuint transformsTex;
        GLuint lightsTex;
        GLuint textureMapsArrayTex;
        GLuint envMapTex;
        GLuint envMapCDFTex;

        // FBOs
        std::shared_ptr<asset::FBO> pathTracefbo;
        std::shared_ptr<asset::FBO> pathTraceFBOLowRes;
        std::shared_ptr<asset::FBO> accumFBO;
        std::shared_ptr<asset::FBO> outputFBO;



        // Shaders
        std::string shadersDirectory;

        std::shared_ptr<asset::Shader> pathTraceShader;
        //Program* pathTraceShader;
        Program* pathTraceShaderLowRes;
        Program* outputShader;
        Program* tonemapShader;

        // Render textures
        GLuint pathTraceTextureLowRes;
        GLuint pathTraceTexture;
        GLuint accumTexture;
        GLuint tileOutputTexture[2];
        GLuint denoisedTexture;

        // Render resolution and window resolution
        iVec2 renderSize;
        iVec2 windowSize;

        // Variables to track rendering status
        iVec2 tile;
        iVec2 numTiles;
        Vec2 invNumTiles;
        int tileWidth;
        int tileHeight;
        int currentBuffer;
        int frameCounter;
        int sampleCounter;
        float pixelRatio;

        // Denoiser output
        Vec3* denoiserInputFramePtr;
        Vec3* frameOutputPtr;
        bool denoised;

        bool initialized;

    public:
        Renderer(Scene* scene, const std::string& shadersDirectory);
        ~Renderer();

        void ResizeRenderer();
        void ReloadShaders();
        void Render();
        void Present();
        void Update(float secondsElapsed);
        float GetProgress();
        int GetSampleCount();
        void GetOutputBuffer(unsigned char**, int& w, int& h);

    private:
        void InitGPUDataBuffers();
        void InitFBOs();
        void InitShaders();
    };
}