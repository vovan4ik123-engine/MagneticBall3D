#include "SettingsMenuGUILayer.h"
#include "EnumsAndVariables.h"
#include "GameStateHelper.h"
#include "DataBaseHelper.h"

namespace MagneticBall3D
{
    SettingsMenuGUILayer::SettingsMenuGUILayer()
    {
        // My GUI.
        const float screenAR = Beryll::Window::getInstance()->getScreenAspectRation();

        auto background = std::make_shared<Beryll::GUITexture>("GUI/menus/settings/SettingsBackground.jpg",
                                                                                     glm::vec3{0.0f, 0.0f, 0.8f}, glm::vec2{100.0f, 100.0f});
        m_guiObjects.push_back(background);

        auto FPSLimit = std::make_shared<Beryll::GUITexture>("GUI/menus/settings/FPSLimit.jpg",
                                                                                   glm::vec3{25.0f, 65.0f, 0.9f}, glm::vec2{50.0f, 10.0f});
        m_guiObjects.push_back(FPSLimit);
        auto FPSTip = std::make_shared<Beryll::GUITexture>("GUI/menus/settings/FPSTip.jpg",
                                                                                 glm::vec3{25.0f, 53.0f, 0.9f}, glm::vec2{50.0f, 10.0f});
        m_guiObjects.push_back(FPSTip);
        auto backMusic = std::make_shared<Beryll::GUITexture>("GUI/menus/settings/BackgroundMusic.jpg",
                                                                                    glm::vec3{25.0f, 41.0f, 0.9f}, glm::vec2{50.0f, 10.0f});
        m_guiObjects.push_back(backMusic);
        auto meteorParticles = std::make_shared<Beryll::GUITexture>("GUI/menus/settings/MeteorParticles.jpg",
                                                                                          glm::vec3{25.0f, 29.0f, 0.9f}, glm::vec2{50.0f, 10.0f});
        m_guiObjects.push_back(meteorParticles);
        auto interfaceGUI = std::make_shared<Beryll::GUITexture>("GUI/menus/settings/InterfaceGUI.jpg",
                                                                                       glm::vec3{25.0f, 17.0f, 0.9f}, glm::vec2{50.0f, 10.0f});
        m_guiObjects.push_back(interfaceGUI);

        m_buttonBack = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/LeftArrow.png", "",
                                                                   glm::vec3{3.0f, 5.0f, 1.0f}, glm::vec2{15.0f, 15.0f});
        m_guiObjects.push_back(m_buttonBack);

        m_checkBox30Fps = std::make_shared<Beryll::CheckBox>("GUI/CheckBoxUnMarked.png", "GUI/CheckBoxMarked.png",
                                                             glm::vec3{40.1f, 66.0f, 1.0f}, glm::vec2{8.0f / screenAR, 8.0f});
        m_guiObjects.push_back(m_checkBox30Fps);
        m_checkBox60Fps = std::make_shared<Beryll::CheckBox>("GUI/CheckBoxUnMarked.png", "GUI/CheckBoxMarked.png",
                                                             glm::vec3{48.7f, 66.0f, 1.0f}, glm::vec2{8.0f / screenAR, 8.0f});
        m_guiObjects.push_back(m_checkBox60Fps);
        m_checkBox120Fps = std::make_shared<Beryll::CheckBox>("GUI/CheckBoxUnMarked.png", "GUI/CheckBoxMarked.png",
                                                              glm::vec3{57.4f, 66.0f, 1.0f}, glm::vec2{8.0f / screenAR, 8.0f});
        m_guiObjects.push_back(m_checkBox120Fps);

        if(EnumsAndVars::SettingsMenu::FPSLimit == 30)
            m_checkBox30Fps->marked = true;
        else if(EnumsAndVars::SettingsMenu::FPSLimit == 60)
            m_checkBox60Fps->marked = true;
        else if(EnumsAndVars::SettingsMenu::FPSLimit == 120)
            m_checkBox120Fps->marked = true;

        m_checkBoxMusic = std::make_shared<Beryll::CheckBox>("GUI/CheckBoxUnMarked.png", "GUI/CheckBoxMarked.png",
                                                             glm::vec3{49.0f, 42.0f, 1.0f}, glm::vec2{8.0f / screenAR, 8.0f});
        m_guiObjects.push_back(m_checkBoxMusic);
        m_checkBoxMeteorPart = std::make_shared<Beryll::CheckBox>("GUI/CheckBoxUnMarked.png", "GUI/CheckBoxMarked.png",
                                                                  glm::vec3{47.0f, 30.0f, 1.0f}, glm::vec2{8.0f / screenAR, 8.0f});
        m_guiObjects.push_back(m_checkBoxMeteorPart);
        m_checkBoxInterface = std::make_shared<Beryll::CheckBox>("GUI/CheckBoxUnMarked.png", "GUI/CheckBoxMarked.png",
                                                                 glm::vec3{52.0f, 18.0f, 1.0f}, glm::vec2{8.0f / screenAR, 8.0f});
        m_guiObjects.push_back(m_checkBoxInterface);

        m_checkBoxMusic->marked = EnumsAndVars::SettingsMenu::backgroundMusic;
        m_checkBoxMeteorPart->marked = EnumsAndVars::SettingsMenu::meteorParticles;
        m_checkBoxInterface->marked = EnumsAndVars::SettingsMenu::interfaceGUI;
    }

    SettingsMenuGUILayer::~SettingsMenuGUILayer()
    {

    }

    void SettingsMenuGUILayer::updateBeforePhysics()
    {
        // My GUI.
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
        else if(m_checkBox30Fps->getIsMarking() || m_checkBox30Fps->getIsUnMarking())
        {
            m_checkBox30Fps->marked = true;
            m_checkBox60Fps->marked = false;
            m_checkBox120Fps->marked = false;
            EnumsAndVars::SettingsMenu::FPSLimit = 30;
            BR_INFO("Set FPSLimit: %d", EnumsAndVars::SettingsMenu::FPSLimit);
            Beryll::GameLoop::setFPSLimit(EnumsAndVars::SettingsMenu::FPSLimit);
            DataBaseHelper::storeSettingsFPSLimit(EnumsAndVars::SettingsMenu::FPSLimit);
        }
        else if(m_checkBox60Fps->getIsMarking() || m_checkBox60Fps->getIsUnMarking())
        {
            m_checkBox30Fps->marked = false;
            m_checkBox60Fps->marked = true;
            m_checkBox120Fps->marked = false;
            EnumsAndVars::SettingsMenu::FPSLimit = 60;
            BR_INFO("Set FPSLimit: %d", EnumsAndVars::SettingsMenu::FPSLimit);
            Beryll::GameLoop::setFPSLimit(EnumsAndVars::SettingsMenu::FPSLimit);
            DataBaseHelper::storeSettingsFPSLimit(EnumsAndVars::SettingsMenu::FPSLimit);
        }
        else if(m_checkBox120Fps->getIsMarking() || m_checkBox120Fps->getIsUnMarking())
        {
            m_checkBox30Fps->marked = false;
            m_checkBox60Fps->marked = false;
            m_checkBox120Fps->marked = true;
            EnumsAndVars::SettingsMenu::FPSLimit = 120;
            BR_INFO("Set FPSLimit: %d", EnumsAndVars::SettingsMenu::FPSLimit);
            Beryll::GameLoop::setFPSLimit(EnumsAndVars::SettingsMenu::FPSLimit);
            DataBaseHelper::storeSettingsFPSLimit(EnumsAndVars::SettingsMenu::FPSLimit);
        }
        else if(m_checkBoxMusic->getIsMarking() || m_checkBoxMusic->getIsUnMarking())
        {
            BR_INFO("%s", "Enable/disable background music.");
            EnumsAndVars::SettingsMenu::backgroundMusic = m_checkBoxMusic->marked;
            DataBaseHelper::storeSettingsBackgroundMusic(EnumsAndVars::SettingsMenu::backgroundMusic);
        }
        else if(m_checkBoxMeteorPart->getIsMarking() || m_checkBoxMeteorPart->getIsUnMarking())
        {
            BR_INFO("%s", "Enable/disable meteor particles.");
            EnumsAndVars::SettingsMenu::meteorParticles = m_checkBoxMeteorPart->marked;
            DataBaseHelper::storeSettingsMeteorParticles(EnumsAndVars::SettingsMenu::meteorParticles);
        }

        if(m_checkBoxInterface->getIsMarking() || m_checkBoxInterface->getIsUnMarking())
        {
            BR_INFO("%s", "Enable/disable interface GUI.");
            EnumsAndVars::SettingsMenu::interfaceGUI = m_checkBoxInterface->marked;
            DataBaseHelper::storeSettingsInterfaceGUI(EnumsAndVars::SettingsMenu::interfaceGUI);
        }
    }

    void SettingsMenuGUILayer::updateAfterPhysics()
    {

    }

    void SettingsMenuGUILayer::draw()
    {
        // My GUI.
        for(const std::shared_ptr<Beryll::GUIObject>& go : m_guiObjects)
        {
            if(go->getIsEnabled())
            {
                go->draw();
            }
        }
    }
}
