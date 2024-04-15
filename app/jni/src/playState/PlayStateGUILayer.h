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

        //std::shared_ptr<Beryll::SliderHorizontal> sliderImpulse;
        //std::shared_ptr<Beryll::SliderHorizontal> sliderTorque;
        std::shared_ptr<Beryll::SliderHorizontal> sliderEnemy;
        std::shared_ptr<Beryll::SliderHorizontal> sliderAmbient;
        std::shared_ptr<Beryll::SliderHorizontal> sliderSunPower;
        std::shared_ptr<Beryll::SliderHorizontal> sliderSpecularPower;
        std::shared_ptr<Beryll::Text> swipeCount;
        std::shared_ptr<Beryll::ProgressBar> progressBarHP;
        std::shared_ptr<Beryll::ProgressBar> progressBarXP;
        std::shared_ptr<Beryll::ButtonWithText> buttonA;
        std::shared_ptr<Beryll::ButtonWithText> buttonPause;
        std::shared_ptr<Beryll::ButtonWithText> buttonResume;
        std::shared_ptr<Beryll::ButtonWithText> buttonExit;

        int swipeCounter = 0;

    private:
        std::vector<std::shared_ptr<Beryll::GUIObject>> m_guiObjects;

        std::shared_ptr<Beryll::Text> m_statistics1;
        std::shared_ptr<Beryll::Text> m_statistics2;
        uint32_t m_statisticsUpdateTime = 0;
    };
}
