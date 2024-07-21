#pragma once

#include "BaseMap.h"

namespace MagneticBall3D
{
    class Map2 : public BaseMap
    {
    public:
        Map2(std::shared_ptr<PlayStateGUILayer> gui);
        ~Map2() override;

        void draw() override;

        void spawnCommonGarbage() override;

    private:
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
        bool m_prepareWave2 = true;
        const float m_enemiesWave2Time = 30.0f;
        bool m_prepareWave3 = true;
        const float m_enemiesWave3Time = 60.0f;
        bool m_prepareWave4 = true;
        const float m_enemiesWave4Time = 90.0f;
        bool m_prepareWave5 = true;
        const float m_enemiesWave5Time = 120.0f;
        bool m_prepareWave6 = true;
        const float m_enemiesWave6Time = 150.0f;
        bool m_prepareWave7 = true;
        const float m_enemiesWave7Time = 180.0f;
        bool m_prepareWave8 = true;
        const float m_enemiesWave8Time = 210.0f;
        bool m_prepareWave9 = true;
        const float m_enemiesWave9Time = 240.0f;
        bool m_prepareWave10 = true;
        const float m_enemiesWave10Time = 270.0f;
        bool m_prepareWave11 = true;
        const float m_enemiesWave11Time = 300.0f;
        bool m_prepareWave12 = true;
        const float m_enemiesWave12Time = 330.0f;

        // Last wave.
        bool m_prepareLastWave = true;
        const float m_prepareLastWaveTime = 360.0f;
    };
}
