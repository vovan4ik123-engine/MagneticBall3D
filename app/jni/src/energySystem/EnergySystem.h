#pragma once

#include "EngineHeaders.h"

namespace MagneticBall3D
{
    // Manage energy required to start game, refill energy, allow buy energy.
    class EnergySystem
    {
    public:
        static EnergySystem& getInstance()
        {
            static EnergySystem inst;
            return inst;
        }

        EnergySystem(const EnergySystem& es) = delete;
        EnergySystem& operator=(const EnergySystem& es) = delete;
        EnergySystem(EnergySystem&& es) = delete;
        EnergySystem& operator=(EnergySystem&& es) = delete;

        void update();
        void draw();

        bool isEnoughForPlay();
        void handlePlay();

    private:
        EnergySystem();
        ~EnergySystem();

        uint64_t m_currentSec = 0; // Real time clock in seconds. Since epoch (1.1.1970)
        std::string m_restoreTimerText;

    };
}
