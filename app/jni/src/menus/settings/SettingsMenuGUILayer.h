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

        std::shared_ptr<Beryll::Text> m_settingsText;

        // FPS limit.
        void recheckFPSCheckBoxes(int checkedID);
        std::shared_ptr<Beryll::Text> m_FPSLimitText;
        std::shared_ptr<Beryll::CheckBox> m_30FPS;
        std::shared_ptr<Beryll::CheckBox> m_60FPS;
        std::shared_ptr<Beryll::CheckBox> m_120FPS;
        std::shared_ptr<Beryll::CheckBox> m_250FPS;
        std::vector<std::shared_ptr<Beryll::CheckBox>> m_FPSCheckBoxes;

        // Background music.
        std::shared_ptr<Beryll::Text> m_musicText;
        std::shared_ptr<Beryll::CheckBox> m_musicCheckBox;
    };
}
