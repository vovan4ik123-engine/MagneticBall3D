#include "SettingsMenuGUILayer.h"
#include "EnumsAndVariables.h"
#include "GameStateHelper.h"
#include "DataBaseHelper.h"

namespace MagneticBall3D
{
    SettingsMenuGUILayer::SettingsMenuGUILayer()
    {
        m_buttonBack = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/LeftArrow.jpg", "", 0, 0.9f, 0.3f, 0.1f);
        m_guiObjects.push_back(m_buttonBack);

        m_settingsText = std::make_shared<Beryll::Text>("Settings", EnAndVars::FontsPath::ROBOTO, 0.05f, 0.323f, 0, 0.38f, 0.055f);
        m_guiObjects.push_back(m_settingsText);

        m_FPSLimitText = std::make_shared<Beryll::Text>("FPS limit:", EnAndVars::FontsPath::ROBOTO, 0.03f, 0.01f, 0.07f, 0.3f, 0.035f);
        m_guiObjects.push_back(m_FPSLimitText);

        m_30FPS = std::make_shared<Beryll::CheckBox>("30 ", EnAndVars::FontsPath::ROBOTO, 0.03f, 0.26f, 0.07f);
        m_guiObjects.push_back(m_30FPS);
        m_FPSCheckBoxes.push_back(m_30FPS);
        if(EnAndVars::SettingsMenu::FPSLimit == 30)
            recheckFPSCheckBoxes(m_30FPS->getID());

        m_60FPS = std::make_shared<Beryll::CheckBox>("60 ", EnAndVars::FontsPath::ROBOTO, 0.03f, 0.41f, 0.07f);
        m_guiObjects.push_back(m_60FPS);
        m_FPSCheckBoxes.push_back(m_60FPS);
        if(EnAndVars::SettingsMenu::FPSLimit == 60)
            recheckFPSCheckBoxes(m_60FPS->getID());

        m_120FPS = std::make_shared<Beryll::CheckBox>("120 ", EnAndVars::FontsPath::ROBOTO, 0.03f, 0.56f, 0.07f);
        m_guiObjects.push_back(m_120FPS);
        m_FPSCheckBoxes.push_back(m_120FPS);
        if(EnAndVars::SettingsMenu::FPSLimit == 120)
            recheckFPSCheckBoxes(m_120FPS->getID());

        m_250FPS = std::make_shared<Beryll::CheckBox>("250 ", EnAndVars::FontsPath::ROBOTO, 0.03f, 0.74f, 0.07f);
        m_guiObjects.push_back(m_250FPS);
        m_FPSCheckBoxes.push_back(m_250FPS);
        if(EnAndVars::SettingsMenu::FPSLimit == 250)
            recheckFPSCheckBoxes(m_250FPS->getID());

        m_musicText = std::make_shared<Beryll::Text>("Background music:", EnAndVars::FontsPath::ROBOTO, 0.03f, 0.01f, 0.12f, 0.5f, 0.035f);
        m_guiObjects.push_back(m_musicText);

        m_musicCheckBox = std::make_shared<Beryll::CheckBox>("", EnAndVars::FontsPath::ROBOTO, 0.03f, 0.5f, 0.12f);
        m_guiObjects.push_back(m_musicCheckBox);
        m_musicCheckBox->checked = EnAndVars::SettingsMenu::backgroundMusic;
    }

    SettingsMenuGUILayer::~SettingsMenuGUILayer()
    {

    }

    void SettingsMenuGUILayer::updateBeforePhysics()
    {
        for(const std::shared_ptr<Beryll::GUIObject>& go : m_guiObjects)
        {
            if(go->getIsEnabled())
            {
                go->updateBeforePhysics();
            }
        }

        if(m_buttonBack->getIsPressed())
        {
            GameStateHelper::popState();
            return;
        }

        for(auto& checkBox : m_FPSCheckBoxes)
        {
            if(checkBox->getIsValueChangingToMarked())
            {
                EnAndVars::SettingsMenu::FPSLimit = std::stoi(checkBox->text);
                BR_INFO("EnumsAndVariables::SettingsMenu::FPSLimit:%d", EnAndVars::SettingsMenu::FPSLimit);
                Beryll::GameLoop::setFPSLimit(EnAndVars::SettingsMenu::FPSLimit);

                // Also store to data base.
                DataBaseHelper::storeSettingsFPSLimit(EnAndVars::SettingsMenu::FPSLimit);
                break;
            }
        }

        // Dont allow all check boxes be unchecked. Always keep one checked.
        if(EnAndVars::SettingsMenu::FPSLimit == 30)
            recheckFPSCheckBoxes(m_30FPS->getID());
        else if(EnAndVars::SettingsMenu::FPSLimit == 60)
            recheckFPSCheckBoxes(m_60FPS->getID());
        else if(EnAndVars::SettingsMenu::FPSLimit == 120)
            recheckFPSCheckBoxes(m_120FPS->getID());
        else if(EnAndVars::SettingsMenu::FPSLimit == 250)
            recheckFPSCheckBoxes(m_250FPS->getID());

        if(m_musicCheckBox->checked != EnAndVars::SettingsMenu::backgroundMusic)
        {
            BR_INFO("%s", "Change play music.");
            EnAndVars::SettingsMenu::backgroundMusic = m_musicCheckBox->checked;
            DataBaseHelper::storeSettingsBackgroundMusic(EnAndVars::SettingsMenu::backgroundMusic);
        }
    }

    void SettingsMenuGUILayer::updateAfterPhysics()
    {

    }

    void SettingsMenuGUILayer::draw()
    {
        for(const std::shared_ptr<Beryll::GUIObject>& go : m_guiObjects)
        {
            if(go->getIsEnabled())
            {
                go->draw();
            }
        }
    }

    void SettingsMenuGUILayer::recheckFPSCheckBoxes(int checkedID)
    {
        for(auto& checkBox : m_FPSCheckBoxes)
        {
            if(checkBox->getID() == checkedID)
                checkBox->checked = true;
            else
                checkBox->checked = false;
        }
    }
}
