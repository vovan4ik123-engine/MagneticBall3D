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

        std::shared_ptr<Beryll::SliderHorizontal> sliderFPS;
        std::shared_ptr<Beryll::SliderHorizontal> sliderFriction;
        std::shared_ptr<Beryll::SliderHorizontal> sliderDamping;
        std::shared_ptr<Beryll::SliderHorizontal> sliderImpulse;
        std::shared_ptr<Beryll::SliderHorizontal> sliderTorque;
        std::shared_ptr<Beryll::SliderHorizontal> sliderCameraDistance;
        std::shared_ptr<Beryll::SliderHorizontal> sliderCameraRotSpeed;
        std::shared_ptr<Beryll::SliderHorizontal> sliderCameraRotStartSpeed;

    private:
        std::vector<std::shared_ptr<Beryll::GUIObject>> m_guiObjects;

        std::shared_ptr<Beryll::Text> m_statistics1;
        std::shared_ptr<Beryll::Text> m_statistics2;
        uint32_t m_statisticsUpdateTime = 0;
    };
}
