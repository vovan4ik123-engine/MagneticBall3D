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
        const float m_enemiesWave2Time = 20.0f;
        bool m_prepareWave3 = true;
        const float m_enemiesWave3Time = 50.0f;
        bool m_prepareWave4 = true;
        const float m_enemiesWave4Time = 80.0f;
        bool m_prepareWave5 = true;
        const float m_enemiesWave5Time = 110.0f;
        bool m_prepareWave6 = true;
        const float m_enemiesWave6Time = 140.0f;
        bool m_prepareWave7 = true;
        const float m_enemiesWave7Time = 170.0f;
        bool m_prepareWave8 = true;
        const float m_enemiesWave8Time = 200.0f;
        bool m_prepareWave9 = true;
        const float m_enemiesWave9Time = 230.0f;
        bool m_prepareWave10 = true;
        const float m_enemiesWave10Time = 260.0f;
        bool m_prepareWave11 = true;
        const float m_enemiesWave11Time = 290.0f;
        bool m_prepareWave12 = true;
        const float m_enemiesWave12Time = 320.0f;

        // Last wave.
        bool m_prepareLastWave = true;
        const float m_prepareLastWaveTime = 360.0f;
    };
}
