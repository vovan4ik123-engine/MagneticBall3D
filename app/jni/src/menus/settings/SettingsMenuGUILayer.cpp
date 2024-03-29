#include "SettingsMenuGUILayer.h"
#include "EnumsAndVariables.h"
#include "GameStateHelper.h"
#include "DataBaseHelper.h"

namespace MagneticBall3D
{
    SettingsMenuGUILayer::SettingsMenuGUILayer()
    {
        m_buttonBack = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/leftArrow.jpg", "", -1, 90, 30, 10);
        m_guiObjects.push_back(m_buttonBack);

        m_settingsText = std::make_shared<Beryll::Text>("Settings", EnumsAndVariables::FontsPath::ROBOTO, 5, 32.3f, 0);
        m_guiObjects.push_back(m_settingsText);

        m_FPSLimitText = std::make_shared<Beryll::Text>("FPS limit:", EnumsAndVariables::FontsPath::ROBOTO, 3, 1, 7);
        m_guiObjects.push_back(m_FPSLimitText);

        m_30FPS = std::make_shared<Beryll::CheckBox>("30 ", EnumsAndVariables::FontsPath::ROBOTO, 3, 26, 7);
        m_guiObjects.push_back(m_30FPS);
        m_FPSCheckBoxes.push_back(m_30FPS);
        if(EnumsAndVariables::SettingsMenu::FPSLimit == 30)
            recheckFPSCheckBoxes(m_30FPS->getID());

        m_60FPS = std::make_shared<Beryll::CheckBox>("60 ", EnumsAndVariables::FontsPath::ROBOTO, 3, 41, 7);
        m_guiObjects.push_back(m_60FPS);
        m_FPSCheckBoxes.push_back(m_60FPS);
        if(EnumsAndVariables::SettingsMenu::FPSLimit == 60)
            recheckFPSCheckBoxes(m_60FPS->getID());

        m_120FPS = std::make_shared<Beryll::CheckBox>("120 ", EnumsAndVariables::FontsPath::ROBOTO, 3, 56, 7);
        m_guiObjects.push_back(m_120FPS);
        m_FPSCheckBoxes.push_back(m_120FPS);
        if(EnumsAndVariables::SettingsMenu::FPSLimit == 120)
            recheckFPSCheckBoxes(m_120FPS->getID());

        m_250FPS = std::make_shared<Beryll::CheckBox>("250 ", EnumsAndVariables::FontsPath::ROBOTO, 3, 74, 7);
        m_guiObjects.push_back(m_250FPS);
        m_FPSCheckBoxes.push_back(m_250FPS);
        if(EnumsAndVariables::SettingsMenu::FPSLimit == 250)
            recheckFPSCheckBoxes(m_250FPS->getID());

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
            MagneticBall3D::GameStateHelper::popState();
            return;
        }

        for(auto& checkBox : m_FPSCheckBoxes)
        {
            if(checkBox->getIsValueChangingToMarked())
            {
                EnumsAndVariables::SettingsMenu::FPSLimit = std::stoi(checkBox->text);
                BR_INFO("EnumsAndVariables::SettingsMenu::FPSLimit:%d", EnumsAndVariables::SettingsMenu::FPSLimit);
                Beryll::GameLoop::setFPSLimit(EnumsAndVariables::SettingsMenu::FPSLimit);

                // Also store to data base.
                DataBaseHelper::storeFPSLimit(EnumsAndVariables::SettingsMenu::FPSLimit);
                break;
            }
        }

        // Dont allow all check boxes be unchecked. Always keep one checked.
        if(EnumsAndVariables::SettingsMenu::FPSLimit == 30)
            recheckFPSCheckBoxes(m_30FPS->getID());
        else if(EnumsAndVariables::SettingsMenu::FPSLimit == 60)
            recheckFPSCheckBoxes(m_60FPS->getID());
        else if(EnumsAndVariables::SettingsMenu::FPSLimit == 120)
            recheckFPSCheckBoxes(m_120FPS->getID());
        else if(EnumsAndVariables::SettingsMenu::FPSLimit == 250)
            recheckFPSCheckBoxes(m_250FPS->getID());
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
