#pragma once

#include "BaseMap.h"

namespace MagneticBall3D
{
    class Map1 : public BaseMap
    {
    public:
        Map1(std::shared_ptr<PlayStateGUILayer> gui);
        ~Map1() override;

        void draw() override;

    private:
        void loadPlayer();
        void loadEnv();
        void loadGarbage();
        void loadEnemies();
        void loadBoss();

        void spawnEnemies() override;
        void startBossPhase() override;
        void handlePossPhase() override;

        // Enemies waves.
        bool m_prepareWave1 = true;
        const float m_enemiesWave1Time = 0.0f; // Sec.
        bool m_prepareWave2 = false;
        const float m_enemiesWave2Time = 30.0f;
        bool m_prepareWave3 = false;
        const float m_enemiesWave3Time = 60.0f;
        bool m_prepareWave4 = false;
        const float m_enemiesWave4Time = 120.0f;
        bool m_prepareWave5 = false;
        const float m_enemiesWave5Time = 180.0f;
        bool m_prepareWave6 = false;
        const float m_enemiesWave6Time = 240.0f;
        bool m_prepareWave7 = false;
        const float m_enemiesWave7Time = 300.0f;
        bool m_prepareWave8 = false;
        const float m_enemiesWave8Time = 360.0f;
        bool m_prepareWave9 = false;
        const float m_enemiesWave9Time = 420.0f;
        bool m_prepareWave10 = false;
        const float m_enemiesWave10Time = 480.0f;

        // Prepare to boss.
        bool m_prepareToBoss = true;
        const float m_prepareToBossTime = 15.0f;
    };
}
