#pragma once

#include "EngineHeaders.h"

namespace MagneticBall3D
{
    class SettingsMenuGUILayer : public Beryll::Layer
    {
    public:
        SettingsMenuGUILayer();
        ~SettingsMenuGUILayer() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

    private:
        std::vector<std::shared_ptr<Beryll::GUIObject>> m_guiObjects;

        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonBack;

        std::shared_ptr<Beryll::CheckBox> m_checkBox30Fps;
        std::shared_ptr<Beryll::CheckBox> m_checkBox60Fps;
        std::shared_ptr<Beryll::CheckBox> m_checkBox120Fps;
        std::shared_ptr<Beryll::CheckBox> m_checkBoxMusic;
        std::shared_ptr<Beryll::CheckBox> m_checkBoxMeteorPart;
        std::shared_ptr<Beryll::CheckBox> m_checkBoxInterface;
    };
}
