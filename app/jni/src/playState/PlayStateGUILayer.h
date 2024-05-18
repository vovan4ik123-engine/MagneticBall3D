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

        void showMenuResurrect();
        void showMenuResurrectNoCrystals();
        void showMenuKillAllBeforeBoss();
        void showMenuLose();
        void showMenuWin();

        // Menus before specific bosses.
        void showMenuBossTankWithCommander();

        //std::shared_ptr<Beryll::SliderHorizontal> sliderImpulse;
        //std::shared_ptr<Beryll::SliderHorizontal> sliderTorque;
        std::shared_ptr<Beryll::SliderHorizontal> sliderAmbient;
        std::shared_ptr<Beryll::SliderHorizontal> sliderSunPower;
        std::shared_ptr<Beryll::SliderHorizontal> sliderSpecularPower;

        std::shared_ptr<Beryll::ProgressBar> progressBarHP;
        std::shared_ptr<Beryll::ProgressBar> progressBarXP;

        std::shared_ptr<Beryll::ButtonWithText> buttonExit;

        // Pause.
        std::shared_ptr<Beryll::ButtonWithText> buttonPause;
        std::shared_ptr<Beryll::ButtonWithText> buttonPauseResume;
        // + buttonExit.

        // Resurrect.
        std::shared_ptr<Beryll::GUITexture> textureResurrect;
        std::shared_ptr<Beryll::ButtonWithText> buttonResurrectOk;
        // + buttonExit.

        // Resurrect No Crystals.
        std::shared_ptr<Beryll::GUITexture> textureResurrectNoCrystals;
        // + buttonExit.

        // Lose.
        std::shared_ptr<Beryll::GUITexture> textureLose;
        // + buttonExit.

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
    };
}
