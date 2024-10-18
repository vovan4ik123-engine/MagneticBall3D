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
        std::unique_ptr<Beryll::Texture> m_backButtonTexture;
        bool m_backButtonClicked = false;

        // Talents header.
        std::unique_ptr<Beryll::Texture> m_talentsHeaderTexture;

        // Max speed. = 0
        std::shared_ptr<Beryll::Texture> m_maxSpeedButtonTexture;
        bool m_maxSpeedButtonClicked = false;
        // Magnetic radius. = 1
        std::shared_ptr<Beryll::Texture> m_magneticRadiusButtonTexture;
        bool m_magneticRadiusButtonClicked = false;
        // Amount of magnetized garbage. = 2
        std::shared_ptr<Beryll::Texture> m_amountOfMagnetizedGarbageButtonTexture;
        bool m_amountOfMagnetizedGarbageButtonClicked = false;
        // Accelerate faster. = 3
        std::shared_ptr<Beryll::Texture> m_accelerateFasterButtonTexture;
        bool m_accelerateFasterButtonClicked = false;
        // Ball and garbage protection. = 4
        std::shared_ptr<Beryll::Texture> m_ballAndGarbageProtectionButtonTexture;
        bool m_ballAndGarbageProtectionButtonClicked = false;
        // Resurrection attempts. = 5
        std::shared_ptr<Beryll::Texture> m_resurrectionAttemptsButtonTexture;
        bool m_resurrectionAttemptsButtonClicked = false;

        // Menu selected talent.
        std::shared_ptr<Beryll::Texture> m_selectedTalentTexture;
        std::shared_ptr<Beryll::Texture> m_improveByAdTexture;
        std::shared_ptr<Beryll::Texture> m_improveByAdTimerTexture;
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
        int m_selectedCanBeImprovedByAds = 0;
        static int m_selectedIndex;
        std::string m_adTimerText;
        ImFont* m_adTimerFont;

        void selectTalent(int index);

        // Menu with buttons to improve selected talent.
        bool m_improveByAdClicked = false;
        bool m_improveByCrystalsClicked = false;

        // Not enough crystals menu.
        std::unique_ptr<Beryll::Texture> m_noCrystalsTexture;
        std::unique_ptr<Beryll::Texture> m_noCrystalsButtonOkTexture;
        bool m_noCrystalsButtonOkClicked = false;
        bool m_noCrystalsMenuShow = false;

        // Ad callbacks. Can be called from different thread.
        std::function<void()> m_adSuccessCallback;
        std::function<void()> m_adErrorCallback;
        // Can be assigned from different thread.
        static std::atomic<bool> m_adSuccess;
        static std::atomic<bool> m_adError;
        // Ad loading.
        std::unique_ptr<Beryll::Texture> m_adLoadingTexture;
        bool m_adLoadingMenuShow = false;
        // Ad error.
        std::unique_ptr<Beryll::Texture> m_adErrorTexture;
        std::unique_ptr<Beryll::Texture> m_adErrorButtonOkTexture;
        bool m_adErrorButtonOkClicked = false;
        bool m_adErrorMenuShow = false;
    };
}
