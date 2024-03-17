#pragma once

#include "EngineHeaders.h"
#include "PlayStateGUILayer.h"
#include "enemies/AnimatedCollidingEnemy.h"
#include "pathfinding/AStar.h"
#include "player/Player.h"
#include "garbage/Garbage.h"

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
        void loadAnimatedModels();
        void loadShaders();
        void loadSunPosition(const glm::vec3& pos, float clipCubeWidth, float clipCubeHeight, float clipCubeDepth);

        // Before physics.
        void controlPlayer();
        void updateGarbageGravity();

        // After physics.
        void updateGravity();
        void updatePlayerSpeed();
        void updatePathfindingAndSpawnEnemies();
        void killEnemies();
        void handleCamera();

    private:
        std::shared_ptr<PlayStateGUILayer> m_gui;

        std::shared_ptr<Player> m_player;
        std::vector<Garbage> m_allGarbageWrappers; // Garbage contains Beryll::SimpleCollidingObject inside.
        std::vector<std::shared_ptr<AnimatedCollidingEnemy>> m_allAnimatedEnemies;
        std::vector<std::shared_ptr<Beryll::SceneObject>> m_allSceneObjects;
        std::vector<std::shared_ptr<Beryll::SimpleCollidingObject>> m_allStaticEnv;
        std::vector<std::shared_ptr<Beryll::BaseSimpleObject>> m_simpleObjForShadowMap;
        std::vector<std::shared_ptr<Beryll::BaseAnimatedObject>> m_animatedObjForShadowMap;

        std::function<void(std::vector<std::shared_ptr<Beryll::SceneObject>>&, int, int)> m_updateAfterPhysics;

        // Shaders.
        std::shared_ptr<Beryll::Shader> m_simpleObjSunLightShadows;
        std::shared_ptr<Beryll::Shader> m_simpleObjSunLightShadowsNormals;
        std::shared_ptr<Beryll::Shader> m_animatedObjSunLightShadows;
        std::unique_ptr<Beryll::ShadowMap> m_shadowMap;

        glm::mat4 m_sunLightVPMatrix;
        glm::vec3 m_sunLightDir;

        glm::vec3 m_cameraOffset = glm::normalize(glm::vec3(-1.0f, 0.2f, 0.0f));
        glm::vec3 m_cameraFront{0.0f};
        const float m_startCameraDistance = 220.0f; // For player without garbage.
        float m_cameraDistance = m_startCameraDistance;

        // Control player.
        int m_fingerDownID = -1;
        glm::vec2 m_fingerDownPos{0.0f};
        glm::vec2 m_fingerUpPos{0.0f};
        glm::vec3 m_playerMoveDir{0.0f};
        float m_playerMoveSpeed{0.0f};
        const glm::vec3 m_startDir{1.0f, 0.0f, 0.0f};
        glm::vec3 m_screenSwipeDir{0.0f};
        float m_lastTimeOnBuilding = 0.0f; // Sec.
        const float m_applyGravityDelay = 0.25f; // Sec. For player after he stop collide with buildings.
        int m_objectsInMagneticRadius = 0;

        // Pathfinding.
        AStar m_pathFinder{-250, 250, -250, 250, 5};
        std::vector<glm::ivec2> m_pathGridXZ; // Points for enemy movements.
        glm::ivec2 m_playerClosestAllowedPos{0}; // On m_allowedPointsToMoveXZ.
        std::vector<glm::ivec2> m_allowedPointsToSpawnEnemies; // From m_allowedPointsToMoveXZ.
        int m_pathFindingIteration = 0; // To separate complicated calculations between many frames.
    };
}
