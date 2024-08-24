#pragma once

#include "BaseMap.h"

namespace MagneticBall3D
{
    class Map4 : public BaseMap
    {
    public:
        Map4(std::shared_ptr<PlayStateGUILayer> gui);
        ~Map4() override;

        void draw() override;

    private:
        void loadEnv();
        void loadGarbage();
        void loadEnemies();

        void spawnEnemies() override;
        void startBossPhase() override {};
        void handlePossPhase() override {};

        // Enemies waves.
        bool m_prepareWave1 = true;
        const float m_enemiesWave1Time = 0.0f; // Sec.
        bool m_prepareWave2 = true;
        const float m_enemiesWave2Time = 20.0f;
        bool m_prepareWave3 = true;
        const float m_enemiesWave3Time = 40.0f;
        bool m_prepareWave4 = true;
        const float m_enemiesWave4Time = 60.0f;
        bool m_prepareWave5 = true;
        const float m_enemiesWave5Time = 80.0f;
        bool m_prepareWave6 = true;
        const float m_enemiesWave6Time = 100.0f;
        bool m_prepareWave7 = true;
        const float m_enemiesWave7Time = 120.0f;
        bool m_prepareWave8 = true;
        const float m_enemiesWave8Time = 150.0f;
        bool m_prepareWave9 = true;
        const float m_enemiesWave9Time = 180.0f;

        // Last wave.
        bool m_prepareLastWave = true;
        const float m_prepareLastWaveTime = 200.0f;
    };
}
