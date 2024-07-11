#pragma once

#include "EngineHeaders.h"

namespace MagneticBall3D
{
    class PlayStateGUILayer : public Beryll::Layer
    {
    public:
        PlayStateGUILayer();
        ~PlayStateGUILayer() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

        void disableMapPlayTimer() { m_showMapPlayTimer = false; }

        void showMenuResurrect();
        void showMenuResurrectNoCrystals();
        void showMenuKillAllBeforeBoss();
        void showMenuLose();
        void showMenuWin();

        // Menus before specific bosses.
        void showMenuBossTankWithCommander();

        std::shared_ptr<Beryll::SliderHorizontal> sliderAmbient;
        std::shared_ptr<Beryll::SliderHorizontal> sliderSpecularPower;
//        std::shared_ptr<Beryll::SliderHorizontal> sliderSunPower;

        float progressBarHP = 1.0f;
        float progressBarXP = 0.0f;

        bool resurrectPlayer = false;
        bool tutorialSwipeEnabled = false;
        bool tutorialHowToSwipeEnabled = false;
        bool tutorialSwipeOnWallEnabled = false;

    private:
        std::vector<std::shared_ptr<Beryll::GUIObject>> m_guiObjects;

        bool m_showStatistics = true;
        std::shared_ptr<Beryll::Text> m_statistics1;
        std::shared_ptr<Beryll::Text> m_statistics2;
        std::shared_ptr<Beryll::Text> m_swipeCount;
        uint64_t m_statisticsUpdateTime = 0;

        float m_timeAppearsOnScreen = 0.0f;
        // Because user can accidentally click when he actively swipe and buttons appears on screen.
        float m_delayBeforeCanBeClicked = 0.7f;

        // GUI based on raw ImGUI.
        // ImGUI flags.
        int m_noBackgroundNoFrame = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground |
                                    ImGuiWindowFlags_NoScrollbar;

        // HP bar.
        static const std::string m_progressBarHPID;
        // XP bar.
        static const std::string m_progressBarXPID;

        // Play timer.
        static const std::string m_mapPlayTimerID;
        std::string m_textMapPlayTimer;
        ImFont* m_fontMapPlayTimer;
        bool m_showMapPlayTimer = false;

        // Smashed count.
        static const std::string m_smashedCountTextureID;
        std::unique_ptr<Beryll::Texture> m_smashedCountTexture;
        static const std::string m_smashedCountTextID;
        ImFont* m_fontSmashedCount;

        // Speed.
        static const std::string m_speedTextureID;
        std::unique_ptr<Beryll::Texture> m_speedTexture;
        static const std::string m_speedTextID;
        ImFont* m_fontSpeed;

        // Button pause.
        static const std::string m_buttonPauseID;
        std::unique_ptr<Beryll::Texture> m_buttonPauseTexture;
        bool m_buttonPauseClicked = false;

        // Button resume.
        static const std::string m_buttonResumeID;
        std::unique_ptr<Beryll::Texture> m_buttonResumeTexture;
        bool m_buttonResumeEnabled = false;
        bool m_buttonResumeClicked = false;

        // Button exit.
        static const std::string m_buttonExitID;
        std::unique_ptr<Beryll::Texture> m_buttonExitTexture;
        bool m_buttonExitEnabled = false;
        bool m_buttonExitClicked = false;
        float m_buttonExitLeft = 0.0f;
        const float m_buttonExitTop = 0.5f;

        // Map0Tutorial.
        static const std::string m_tutorialSwipeID;
        std::unique_ptr<Beryll::Texture> m_tutorialSwipeTexture;

        static const std::string m_tutorialHowToSwipeID;
        std::unique_ptr<Beryll::Texture> m_tutorialHowToSwipeTexture;

        static const std::string m_tutorialSwipeOnWallID;
        std::unique_ptr<Beryll::Texture> m_tutorialSwipeOnWallTexture;

        // Resurrect.
        static const std::string m_resurrectTextureID;
        std::unique_ptr<Beryll::Texture> m_resurrectTexture;
        static const std::string m_resurrectButtonOkID;
        std::unique_ptr<Beryll::Texture> m_resurrectButtonOkTexture;
        bool m_menuResurrectEnabled = false;
        bool m_resurrectButtonClicked = false;

        // Resurrect No Crystals.
        static const std::string m_resurrectNoCrystalsTextureID;
        std::unique_ptr<Beryll::Texture> m_resurrectNoCrystalsTexture;
        bool m_menuResurrectNoCrystalsEnabled = false;

        // Lose.
        static const std::string m_loseTextureID;
        std::unique_ptr<Beryll::Texture> m_loseTexture;
        bool m_menuLoseEnabled = false;

        // Kill all enemies before boss.
        static const std::string m_killAllTextureID;
        std::unique_ptr<Beryll::Texture> m_killAllTexture;
        static const std::string m_killAllButtonOkID;
        std::unique_ptr<Beryll::Texture> m_killAllButtonOkTexture;
        bool m_menuKillAllEnabled = false;
        bool m_killAllButtonClicked = false;

        // Win.
        static const std::string m_winTextureID;
        std::unique_ptr<Beryll::Texture> m_winTexture;
        static const std::string m_winButtonOkID;
        std::unique_ptr<Beryll::Texture> m_winButtonOkTexture;
        bool m_menuWinEnabled = false;
        bool m_winButtonClicked = false;

        // Menus before specific bosses.
        // Tank with commander.
        static const std::string m_tankWithCommanderTextureID;
        std::unique_ptr<Beryll::Texture> m_tankWithCommanderTexture;
        static const std::string m_tankWithCommanderButtonOkID;
        std::unique_ptr<Beryll::Texture> m_tankWithCommanderButtonOkTexture;
        bool m_menuTankWithCommanderEnabled = false;
        bool m_tankWithCommanderButtonClicked = false;
    };
}
