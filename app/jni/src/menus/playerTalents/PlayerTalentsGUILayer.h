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

        int m_noBackgroundNoFrame = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground;

        int m_noFrameNoFocus = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                               ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBringToFrontOnFocus;

        int m_noFrame = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar;

        // Common texture for all transparent elements.
        std::shared_ptr<Beryll::Texture> m_transparentTexture;

        // Back.
        static const std::string m_backButtonID;
        std::unique_ptr<Beryll::Texture> m_backButtonTexture;
        bool m_backButtonClicked = false;

        // Talents header.
        static const std::string m_talentsHeaderID;
        std::unique_ptr<Beryll::Texture> m_talentsHeaderTexture;

        // All talents.
        static const std::string m_allTalentsMenuID;
        // Max speed. = 0
        static const std::string m_maxSpeedButtonID;
        std::shared_ptr<Beryll::Texture> m_maxSpeedButtonTexture;
        bool m_maxSpeedButtonClicked = false;
        // Magnetic radius. = 1
        static const std::string m_magneticRadiusButtonID;
        std::shared_ptr<Beryll::Texture> m_magneticRadiusButtonTexture;
        bool m_magneticRadiusButtonClicked = false;
        // Amount of magnetized garbage. = 2
        static const std::string m_amountOfMagnetizedGarbageButtonID;
        std::shared_ptr<Beryll::Texture> m_amountOfMagnetizedGarbageButtonTexture;
        bool m_amountOfMagnetizedGarbageButtonClicked = false;
        // Accelerate faster. = 3
        static const std::string m_accelerateFasterButtonID;
        std::shared_ptr<Beryll::Texture> m_accelerateFasterButtonTexture;
        bool m_accelerateFasterButtonClicked = false;
        // Ball and garbage protection. = 4
        static const std::string m_ballAndGarbageProtectionButtonID;
        std::shared_ptr<Beryll::Texture> m_ballAndGarbageProtectionButtonTexture;
        bool m_ballAndGarbageProtectionButtonClicked = false;
        // Resurrection attempts. = 5
        static const std::string m_resurrectionAttemptsButtonID;
        std::shared_ptr<Beryll::Texture> m_resurrectionAttemptsButtonTexture;
        bool m_resurrectionAttemptsButtonClicked = false;

        // Menu selected talent.
        static const std::string m_selectedTalentMenuID;
        std::shared_ptr<Beryll::Texture> m_selectedTalentTexture;
        std::shared_ptr<Beryll::Texture> m_improveByAdTexture;
        std::shared_ptr<Beryll::Texture> m_improveByCrystalsTexture;
        std::shared_ptr<Beryll::Texture> m_maxLevelReachedTexture;
        ImFont* m_selectedDescriptionFont;
        ImFont* m_selectedValueLevelFont;
        ImFont* m_valueToAddFont;
        std::string m_selectedDescription;
        std::string m_selectedValue;
        std::string m_selectedValueToAdd;
        int m_selectedCurrentLevel = 0;
        int m_selectedMaxLevel = 0;
        int m_selectedPriceCrystals = 0;
        int m_selectedIndex = 0;

        void selectTalent(int index);

        // Menu with buttons to improve selected talent.
        static const std::string m_improveTalentMenuID;
        static const std::string m_improveTalentByAdButtonID;
        static const std::string m_improveTalentByCrystalsButtonID;
        bool m_improveByAdClicked = false;
        bool m_improveByCrystalsClicked = false;

        // Not enough crystals menu.
        static const std::string m_noCrystalsMenuID;
        std::unique_ptr<Beryll::Texture> m_noCrystalsTexture;
        static const std::string m_noCrystalsButtonOkID;
        std::unique_ptr<Beryll::Texture> m_noCrystalsButtonOkTexture;
        bool m_noCrystalsButtonOkClicked = false;
        bool m_showNoCrystalsMenu = false;

        // Ad loading menu.
    };
}
