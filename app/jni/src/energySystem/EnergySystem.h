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
            static EnergySystem energySystem;
            return energySystem;
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

        uint64_t m_currentSec = 0; // Real time clock in seconds.

        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonEnergy;
        std::shared_ptr<Beryll::Text> m_textAmount;
        std::shared_ptr<Beryll::Text> m_textRestoreTimer;
    };
}
