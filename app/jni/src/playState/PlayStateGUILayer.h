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
        void showMenuKillAllToSpawnBoss();
        void showMenuKillAllToWin();
        void showMenuLose();
        void showMenuWin();

        // Menus before specific bosses.
        void showMenuBossTankWithCommander();

        std::shared_ptr<Beryll::SliderHorizontal> sliderAmbient;
        std::shared_ptr<Beryll::SliderHorizontal> sliderSunPower;
        std::shared_ptr<Beryll::SliderHorizontal> sliderSpecularPower;

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

        int m_noFrame = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar;

        // HP bar.
        static const std::string m_progressBarHPID;
        // XP bar.
        static const std::string m_progressBarXPID;

        // Play timer.
        static const std::string m_mapPlayTimerID;
        std::string m_mapPlayTimerText;
        ImFont* m_mapPlayTimerFont;
        bool m_showMapPlayTimer = false;

        // Smashed count.
        static const std::string m_smashedCountTextureID;
        std::unique_ptr<Beryll::Texture> m_smashedCountTexture;
        static const std::string m_smashedCountTextID;
        ImFont* m_smashedCountFont;

        // Speed.
        static const std::string m_speedTextureID;
        std::unique_ptr<Beryll::Texture> m_speedTexture;
        static const std::string m_speedTextID;
        ImFont* m_speedFont;

        // Pause.
        static const std::string m_pauseButtonID;
        std::unique_ptr<Beryll::Texture> m_pauseButtonTexture;
        bool m_pauseButtonClicked = false;

        // Resume.
        static const std::string m_resumeButtonID;
        std::unique_ptr<Beryll::Texture> m_resumeButtonTexture;
        bool m_resumeButtonEnabled = false;
        bool m_resumeButtonClicked = false;

        // Exit.
        static const std::string m_exitButtonID;
        std::unique_ptr<Beryll::Texture> m_exitButtonTexture;
        bool m_exitButtonEnabled = false;
        bool m_exitButtonClicked = false;
        float m_exitButtonTop = 0.0f;
        float m_exitButtonLeft = 0.0f;

        // Map0Tutorial.
        static const std::string m_tutorialSwipeID;
        std::unique_ptr<Beryll::Texture> m_tutorialSwipeTexture;

        static const std::string m_tutorialHowToSwipeID;
        std::unique_ptr<Beryll::Texture> m_tutorialHowToSwipeTexture;

        static const std::string m_tutorialSwipeOnWallID;
        std::unique_ptr<Beryll::Texture> m_tutorialSwipeOnWallTexture;

        // Resurrect.
        static const std::string m_resurrectMenuID;
        std::unique_ptr<Beryll::Texture> m_resurrectTexture;
        static const std::string m_resurrectByCrystalsButtonID;
        std::unique_ptr<Beryll::Texture> m_resurrectByCrystalsButtonTexture;
        static const std::string m_resurrectByAdButtonID;
        std::unique_ptr<Beryll::Texture> m_resurrectByAdButtonTexture;
        bool m_menuResurrectEnabled = false;
        bool m_resurrectByCrystalsButtonClicked = false;
        bool m_resurrectByAdButtonClicked = false;

        // Not enough crystals menu.
        static const std::string m_noCrystalsMenuID;
        std::unique_ptr<Beryll::Texture> m_noCrystalsTexture;
        static const std::string m_noCrystalsButtonOkID;
        std::unique_ptr<Beryll::Texture> m_noCrystalsButtonOkTexture;
        bool m_noCrystalsButtonOkClicked = false;
        bool m_showNoCrystalsMenu = false;

        // Lose.
        static const std::string m_loseTextureID;
        std::unique_ptr<Beryll::Texture> m_loseTexture;
        bool m_menuLoseEnabled = false;

        // Last wave kill all enemies.
        static const std::string m_killAllTextureID;
        std::unique_ptr<Beryll::Texture> m_killAllToSpawnBossTexture; // If map has boss.
        std::unique_ptr<Beryll::Texture> m_killAllToWinTexture; // Without boss.
        static const std::string m_killAllButtonOkID;
        std::unique_ptr<Beryll::Texture> m_killAllButtonOkTexture;
        bool m_menuKillAllEnabled = false;
        bool m_killAllButtonClicked = false;
        bool m_killAllToSpawnBoss = false;

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
