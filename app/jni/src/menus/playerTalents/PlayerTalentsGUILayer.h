#pragma once

#include "EngineHeaders.h"

namespace MagneticBall3D
{
    class PlayerTalentsGUILayer : public Beryll::Layer
    {
    public:
        PlayerTalentsGUILayer();
        ~PlayerTalentsGUILayer() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

    private:
        // GUI based on raw ImGUI.
        // ImGUI flags.
        int m_noBackgroundNoFrameNoFocus = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                           ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                                           ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus;

        // Back.
        static const std::string m_backButtonID;
        std::unique_ptr<Beryll::Texture> m_backButtonTexture;
        bool m_backButtonClicked = false;

        // Talents header.
        static const std::string m_talentsHeaderID;
        std::unique_ptr<Beryll::Texture> m_talentsHeaderTexture;

        // All talents.
        static const std::string m_allTalentsMenuID;
        // Max speed.
        static const std::string m_maxSpeedButtonID;
        std::unique_ptr<Beryll::Texture> m_maxSpeedButtonTexture;
        bool m_maxSpeedButtonClicked = false;
        // Magnetic radius.
        static const std::string m_magneticRadiusButtonID;
        std::unique_ptr<Beryll::Texture> m_magneticRadiusButtonTexture;
        bool m_magneticRadiusButtonClicked = false;
        // Amount of magnetized garbage.
        static const std::string m_amountOfMagnetizedGarbageButtonID;
        std::unique_ptr<Beryll::Texture> m_amountOfMagnetizedGarbageButtonTexture;
        bool m_amountOfMagnetizedGarbageButtonClicked = false;
        // Accelerate faster.
        static const std::string m_accelerateFasterButtonID;
        std::unique_ptr<Beryll::Texture> m_accelerateFasterButtonTexture;
        bool m_accelerateFasterButtonClicked = false;
        // Ball and garbage protection.
        static const std::string m_ballAndGarbageProtectionButtonID;
        std::unique_ptr<Beryll::Texture> m_ballAndGarbageProtectionButtonTexture;
        bool m_ballAndGarbageProtectionButtonClicked = false;
        //Resurrection attempts.
        static const std::string m_resurrectionAttemptsButtonID;
        std::unique_ptr<Beryll::Texture> m_resurrectionAttemptsButtonTexture;
        bool m_resurrectionAttemptsButtonClicked = false;
    };
}
