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

        uint64_t m_currentSec = 0; // Real time clock in seconds. Since epoch (1.1.1970)

        // ImGUI flags.
        int m_noBackgroundNoFrame = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground |
                                    ImGuiWindowFlags_NoScrollbar;

        // Texture energy.
        static const std::string m_energyTextureID;
        std::unique_ptr<Beryll::Texture> m_energyTexture;

        static const std::string m_textAmountID;
        ImFont* m_fontAmount;

        static const std::string m_textRestoreTimerID;
        ImFont* m_fontRestoreTimer;
        std::string m_textRestoreTimer;

        // Button energy. Transparent. On top of texture + texts.
        static const std::string m_buttonEnergyID;
        std::unique_ptr<Beryll::Texture> m_buttonEnergyTexture;
        bool m_buttonEnergyClicked = false;

        // Menu buy energy.
    };
}
