#pragma once

#include "EngineHeaders.h"
#include "PlayStateGUILayer.h"

namespace MagneticBall3D
{
    class PlayStateSceneLayer : public Beryll::Layer
    {
    public:
        PlayStateSceneLayer(std::shared_ptr<PlayStateGUILayer> gui);
        ~PlayStateSceneLayer() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

        // Load.
        void loadPlayerAndStaticEnv();
        void loadDynamicEnv();
        void loadShaders();
        void loadSunPosition(const glm::vec3& pos, float clipCubeWidth, float clipCubeHeight, float clipCubeDepth);

        // Logic.
        void controlPlayer();
        void rotateCamera();

    private:
        std::shared_ptr<PlayStateGUILayer> m_gui;

        std::shared_ptr<Beryll::SimpleCollidingObject> m_player;
        std::shared_ptr<Beryll::SimpleCollidingObject> m_ground;
        std::vector<std::shared_ptr<Beryll::SceneObject>> m_allSceneObjects;
        std::vector<std::shared_ptr<Beryll::SimpleCollidingObject>> m_allGarbage;
        std::vector<std::shared_ptr<Beryll::BaseSimpleObject>> m_simpleObjForShadowMap;

        // Shaders.
        std::shared_ptr<Beryll::Shader> m_simpleObjSunLightShadows;
        std::shared_ptr<Beryll::Shader> m_simpleObjSunLightShadowsNormals;
        std::unique_ptr<Beryll::ShadowMap> m_shadowMap;

        glm::mat4 m_sunLightVPMatrix;
        glm::vec3 m_sunLightDir;

        glm::vec3 m_cameraOffset{-1.0f, 0.3f, 0.0f};
        glm::vec3 m_cameraFront{0.0f};
        const float m_maxCameraDistance = 90.0f;
        float m_cameraDistance = m_maxCameraDistance;

        // Control player.
        int m_fingerDownID = -1;
        glm::vec2 m_fingerDownPos{0.0f};
        glm::vec2 m_fingerUpPos{0.0f};
        glm::vec3 m_playerMoveDir{0.0f};
        float m_playerMoveSpeed{0.0f};
        const glm::vec3 m_startDir{1.0f, 0.0f, 0.0f};
    };
}
