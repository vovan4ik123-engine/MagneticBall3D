#pragma once

#include "BaseMap.h"

namespace MagneticBall3D
{
    class Map1 : public BaseMap
    {
    public:
        Map1(std::shared_ptr<PlayStateGUILayer> gui);
        ~Map1() override;

    private:
        void loadPlayer();
        void loadEnv();
        void loadGarbage();
        void loadEnemies();

        void spawnEnemies() override;

        const float m_enemiesWave1Time = 0.0f; // Sec.
        bool m_prepareWave1 = true;
        const float m_enemiesWave2Time = 20.0f;
        bool m_prepareWave2 = true;
        const float m_enemiesWave3Time = 40.0f;
        bool m_prepareWave3 = true;
        const float m_enemiesWave4Time = 60.0f;
        bool m_prepareWave4 = true;
        const float m_enemiesWave5Time = 80.0f;
        bool m_prepareWave5 = true;
    };
}
