#pragma once

#include "EngineHeaders.h"
#include "playState/PlayStateGUILayer.h"
#include "pathfinding/AStar.h"
#include "improvements/Improvements.h"
#include "player/Player.h"
#include "garbage/Garbage.h"
#include "enemies/BaseEnemy.h"
#include "bosses/BaseBoss.h"
#include "SendStatisticsHelper.h"

namespace MagneticBall3D
{
    class BaseMap
    {
    public:
        BaseMap(std::shared_ptr<PlayStateGUILayer> gui);
        virtual ~BaseMap();

        virtual void updateBeforePhysics();
        virtual void updateAfterPhysics();
        virtual void draw() = 0;

        // Load.
        void loadShaders();
        void loadPlayer();

        // Before physics.
        void handlePlayerDie();
        void handlePlayerWin();
        void handleScreenSwipe();
        void updatePathfindingAndSpawnEnemies();
        virtual void spawnEnemies() = 0; // In subclass.
        virtual void spawnCommonGarbage();
        void handleJumpPads();
        void checkPlayerOutOfMap();


        // After physics.
        void updateEnemiesAndTheirsAttacks();
        void updateAndMagnetizeGarbage();
        void killEnemies();
        void handleCamera();
        void updateGUI();

        void emitParticlesLine(const glm::vec3& from, const glm::vec3& to, const float sizeBegin, const float sizeEnd,
                               const glm::vec4& colorBegin, const glm::vec4& colorEnd, const float lifeTime);
        void emitParticlesExplosion(const glm::vec3& orig, const int count, const float sizeBegin, const float sizeEnd,
                                    const glm::vec4& colorBegin, const glm::vec4& colorEnd, const float lifeTime);
        void spawnGarbage(const int count, const GarbageType type, glm::vec3 spawnPoint);

        void updateSunPosition(const glm::vec3& pos, float clipCubeWidth, float clipCubeHeight, float clipCubeDepth);
        void respawnEnemiesAtNewDistance(float minDistance);

        // Boss phase.
        void lastWaveToBossPhase(); // At last enemies wave if map has boss.
        void lastWaveToWinPhase(); // At last enemies wave if map without boss.
        virtual void startBossPhase() {}; // Maps with boss must override that.
        virtual void handlePossPhase() {};

    protected:
        std::shared_ptr<PlayStateGUILayer> m_gui;

        std::shared_ptr<Player> m_player;
        std::vector<Garbage> m_allGarbage; // Common garbage spawned on map + garbage after kill enemies.
        std::vector<std::shared_ptr<BaseEnemy>> m_allAnimatedEnemies;
        std::vector<std::shared_ptr<Beryll::SceneObject>> m_animatedOrDynamicObjects;
        std::vector<std::shared_ptr<Beryll::BaseSimpleObject>> m_staticEnv;
        std::vector<std::shared_ptr<Beryll::BaseSimpleObject>> m_objWithNormalMap;
        std::vector<std::shared_ptr<Beryll::BaseSimpleObject>> m_simpleObjForShadowMap;
        std::vector<std::shared_ptr<Beryll::BaseAnimatedObject>> m_animatedObjForShadowMap;

        std::shared_ptr<BaseBoss> m_boss;
        Beryll::BannerProgressTwoColors m_bossHpBar{30.0f, 5.0f, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}};

//        std::function<void(std::vector<std::shared_ptr<Beryll::SceneObject>>&, int, int)> m_updateAfterPhysics;
//        m_updateAfterPhysics = [](std::vector<std::shared_ptr<Beryll::SceneObject>>& v, int begin, int end) -> void // -> void = return type.
//        { for(int i = begin; i < end; ++i) { if(v[i]->getIsEnabledUpdate()) { v[i]->updateAfterPhysics(); } } };
//        Beryll::AsyncRun::Run(m_allSceneObjects, m_updateAfterPhysics);

        // Map size. Should be assigned in subclasses of specific map.
        float m_minX = -999999.0f;
        float m_maxX = 999999.0f;
        float m_minZ = -999999.0f;
        float m_maxZ = 999999.0f;

        // Shaders and light.
        std::shared_ptr<Beryll::Shader> m_simpleObjSunLightShadows;
        std::shared_ptr<Beryll::Shader> m_simpleObjSunLightShadowsNormals;
        std::shared_ptr<Beryll::Shader> m_animatedObjSunLightShadows;
        std::shared_ptr<Beryll::Shader> m_animatedObjSunLight;
        std::unique_ptr<Beryll::ShadowMap> m_shadowMap;
        glm::mat4 m_sunLightVPMatrix{1.0f};
        // Assign in subclass constructor.
        glm::vec3 m_dirToSun{0.0f};
        glm::vec3 m_sunLightDir{0.0f};
        float m_ambientLight = 0.7f;
        const float m_shadowsCubeWidth = 650.0f;
        const float m_shadowsCubeHeight = 650.0f;
        const float m_shadowsCubeDepth = 1200.0f;

        std::unique_ptr<Beryll::SkyBox> m_skyBox;

        glm::vec3 m_cameraAngleOffset = glm::normalize(glm::vec3(-1.0f, 0.0f, 0.0f));
        glm::vec3 m_cameraFront{0.0f};
        const float m_startCameraDistance = 70.0f;
        float m_cameraDistance = m_startCameraDistance;
        const float m_startCameraYOffset = 10.0f;
        float m_cameraYOffset = m_startCameraYOffset;
        bool m_cameraHit = false;

        // Screen swipe.
        int m_fingerDownID = -1;
        glm::vec2 m_fingerLastPos{0.0f};
        glm::vec2 m_fingerCurrentPos{0.0f};
        const glm::vec3 m_startDir{1.0f, 0.0f, 0.0f};
        glm::vec3 m_screenSwipe3D{0.0f};

        // Pathfinding for enemies.
        AStar m_pathFinderEnemies; // Assign new object with map size in constructor of specific map.
        std::vector<glm::ivec2> m_pathAllowedPositionsXZ; // Points for enemy movements.
        glm::ivec2 m_playerClosestAllowedPos{0}; // On m_allowedPointsToMoveXZ.
        std::vector<glm::ivec2> m_pointsToSpawnEnemies; // From m_allowedPointsToMoveXZ.
        std::vector<glm::ivec2> m_pointsToSpawnCommonGarbage; // From m_allowedPointsToMoveXZ.
        int m_pathFindingIteration = 0; // To separate complicated calculations between many frames.

        // Pathfinding for boss.
        AStar m_pathFinderBoss;
        std::vector<glm::ivec2> m_pathAllowedPositionsXZBoss;

        // Improvements.
        Improvements m_improvements; // m_improvements.setPlayer(m_player); in subclass constructor after player created.
    };
}
