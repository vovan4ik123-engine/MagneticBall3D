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

//        std::shared_ptr<Beryll::SliderHorizontal> sliderAmbient;
//        std::shared_ptr<Beryll::SliderHorizontal> sliderSpecularPower;
//        std::shared_ptr<Beryll::SliderHorizontal> sliderSunPower;

        float progressBarHP = 1.0f;
        float progressBarXP = 0.0f;

        // Map0Tutorial.
        std::shared_ptr<Beryll::GUITexture> textureTutorialSwipe;
        std::shared_ptr<Beryll::GUITexture> textureTutorialSwipeFaster;
        std::shared_ptr<Beryll::GUITexture> textureTutorialSwipeOnBuilding;

        // Resurrect.
        std::shared_ptr<Beryll::GUITexture> textureResurrect;
        std::shared_ptr<Beryll::ButtonWithText> buttonResurrectOk;

        // Resurrect No Crystals.
        std::shared_ptr<Beryll::GUITexture> textureResurrectNoCrystals;

        // Lose.
        std::shared_ptr<Beryll::GUITexture> textureLose;

        // Kill all enemies before boss.
        std::shared_ptr<Beryll::GUITexture> textureKillAll;
        std::shared_ptr<Beryll::ButtonWithText> buttonKillAllOk;

        // Win.
        std::shared_ptr<Beryll::GUITexture> textureWin;
        std::shared_ptr<Beryll::ButtonWithText> buttonWinOk;

        // Menus before specific bosses.
        // Tank with commander.
        std::shared_ptr<Beryll::GUITexture> textureTankWithCommander;
        std::shared_ptr<Beryll::ButtonWithText> buttonTankWithCommanderOk;

        bool resurrectButtonPressed = false;

    private:
        std::vector<std::shared_ptr<Beryll::GUIObject>> m_guiObjects;

        std::shared_ptr<Beryll::Text> m_statistics1;
        std::shared_ptr<Beryll::Text> m_statistics2;
        std::shared_ptr<Beryll::Text> m_swipeCount;
        uint32_t m_statisticsUpdateTime = 0;

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

        // Text map timer.
        static const std::string m_mapPlayTimerID;
        std::string m_textMapPlayTimer;
        ImFont* m_fontMapPlayTimer;
        bool m_showMapPlayTimer = true;

        // Text smashed count.
        static const std::string m_smashedCountID;
        ImFont* m_fontSmashedCount;

        // Text speed.
        static const std::string m_speedID;
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
    };
}
