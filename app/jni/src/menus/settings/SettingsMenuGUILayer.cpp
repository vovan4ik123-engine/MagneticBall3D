#include "SettingsMenuGUILayer.h"
#include "EnumsAndVariables.h"
#include "GameStateHelper.h"
#include "DataBaseHelper.h"

namespace MagneticBall3D
{
    SettingsMenuGUILayer::SettingsMenuGUILayer()
    {
        m_backButtonTexture = Beryll::Renderer::createTexture("GUI/menus/LeftArrow.png", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);

        m_backgroundTexture = Beryll::Renderer::createTexture("GUI/menus/settings/SettingsBackground.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_FPSLimitTexture = Beryll::Renderer::createTexture("GUI/menus/settings/FPSLimit.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_FPSTipTexture = Beryll::Renderer::createTexture("GUI/menus/settings/FPSTip.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_musicTexture = Beryll::Renderer::createTexture("GUI/menus/settings/BackgroundMusic.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_meteorParticlesTexture = Beryll::Renderer::createTexture("GUI/menus/settings/MeteorParticles.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_interfaceGUITexture = Beryll::Renderer::createTexture("GUI/menus/settings/InterfaceGUI.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);

        m_fontForAllCheckBoxes = Beryll::MainImGUI::getInstance()->createFont(EnumsAndVars::FontsPath::roboto, 0.07f);

        if(EnumsAndVars::SettingsMenu::FPSLimit == 30)
            m_30FPSChecked = true;
        else if(EnumsAndVars::SettingsMenu::FPSLimit == 60)
            m_60FPSChecked = true;
        else if(EnumsAndVars::SettingsMenu::FPSLimit == 120)
            m_120FPSChecked = true;
        else if(EnumsAndVars::SettingsMenu::FPSLimit == 250)
            m_250FPSChecked = true;

        m_musicCheckBoxChecked = EnumsAndVars::SettingsMenu::backgroundMusic;
        m_meteorParticlesCheckBoxChecked = EnumsAndVars::SettingsMenu::meteorParticles;
        m_interfaceGUICheckBoxChecked = EnumsAndVars::SettingsMenu::interfaceGUI;
    }

    SettingsMenuGUILayer::~SettingsMenuGUILayer()
    {

    }

    void SettingsMenuGUILayer::updateBeforePhysics()
    {
        if(m_backButtonClicked)
        {
            m_backButtonClicked = false;
            GameStateHelper::popState();
            return;
        }

        if(m_musicCheckBoxChecked != EnumsAndVars::SettingsMenu::backgroundMusic)
        {
            BR_INFO("%s", "Enable/disable background music.");
            EnumsAndVars::SettingsMenu::backgroundMusic = m_musicCheckBoxChecked;
            DataBaseHelper::storeSettingsBackgroundMusic(EnumsAndVars::SettingsMenu::backgroundMusic);
        }

        if(m_meteorParticlesCheckBoxChecked != EnumsAndVars::SettingsMenu::meteorParticles)
        {
            BR_INFO("%s", "Enable/disable meteor particles.");
            EnumsAndVars::SettingsMenu::meteorParticles = m_meteorParticlesCheckBoxChecked;
            DataBaseHelper::storeSettingsMeteorParticles(EnumsAndVars::SettingsMenu::meteorParticles);
        }

        if(m_interfaceGUICheckBoxChecked != EnumsAndVars::SettingsMenu::interfaceGUI)
        {
            BR_INFO("%s", "Enable/disable interface GUI.");
            EnumsAndVars::SettingsMenu::interfaceGUI = m_interfaceGUICheckBoxChecked;
            DataBaseHelper::storeSettingsInterfaceGUI(EnumsAndVars::SettingsMenu::interfaceGUI);
        }
    }

    void SettingsMenuGUILayer::updateAfterPhysics()
    {

    }

    void SettingsMenuGUILayer::draw()
    {
        const float GUIWidth = Beryll::MainImGUI::getInstance()->getGUIWidth();
        const float GUIHeight = Beryll::MainImGUI::getInstance()->getGUIHeight();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}); // Lost focus.
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}); // On focus.
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}); // Clicked.
        ImGui::SetNextWindowPos(ImVec2(-0.005f * GUIWidth, -0.005f * GUIHeight));
        ImGui::SetNextWindowSize(ImVec2(1.01f * GUIWidth, 1.01f * GUIHeight));
        ImGui::Begin("settingsMenu", nullptr, m_noBackgroundNoFrameNoFocus);

        // Background.
        ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
        ImGui::Image(reinterpret_cast<ImTextureID>(m_backgroundTexture->getID()),
                     ImVec2(1.01f * GUIWidth, 1.01f * GUIHeight));

        // Back.
        ImGui::SetCursorPos(ImVec2(0.005f * GUIWidth, 0.855f * GUIHeight));
        m_backButtonClicked = ImGui::ImageButton("backButton", reinterpret_cast<ImTextureID>(m_backButtonTexture->getID()),
                                                 ImVec2(0.15f * GUIWidth, 0.15f * GUIHeight));

        // FPS limit.
        ImGui::SetCursorPos(ImVec2(0.255f * GUIWidth, 0.245f * GUIHeight));
        ImGui::Image(reinterpret_cast<ImTextureID>(m_FPSLimitTexture->getID()),
                     ImVec2(0.5f * GUIWidth, 0.1f * GUIHeight));

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0.0f, 0.0f, 0.0f, 0.0f});
        ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4{1.0f, 1.0f, 1.0f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{0.164f, 0.3047f, 0.195f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4{0.164f, 0.3047f, 0.195f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4{0.164f, 0.3047f, 0.195f, 1.0f});
        ImGui::PushFont(m_fontForAllCheckBoxes);

        // FPS check boxes.
        ImGui::SetCursorPos(ImVec2(0.392f * GUIWidth, 0.26f * GUIHeight));
        if(ImGui::Checkbox("30", &m_30FPSChecked, false))
            resetFPS(30);

        ImGui::SetCursorPos(ImVec2(0.466f * GUIWidth, 0.26f * GUIHeight));
        if(ImGui::Checkbox("60", &m_60FPSChecked, false))
            resetFPS(60);

        ImGui::SetCursorPos(ImVec2(0.541f * GUIWidth, 0.26f * GUIHeight));
        if(ImGui::Checkbox("120", &m_120FPSChecked, false))
            resetFPS(120);

        ImGui::SetCursorPos(ImVec2(0.629f * GUIWidth, 0.26f * GUIHeight));
        if(ImGui::Checkbox("250", &m_250FPSChecked, false))
            resetFPS(250);

        // FPS tip.
        ImGui::SetCursorPos(ImVec2(0.255f * GUIWidth, 0.365f * GUIHeight));
        ImGui::Image(reinterpret_cast<ImTextureID>(m_FPSTipTexture->getID()),
                     ImVec2(0.5f * GUIWidth, 0.1f * GUIHeight));

        // Background music.
        ImGui::SetCursorPos(ImVec2(0.255f * GUIWidth, 0.485f * GUIHeight));
        ImGui::Image(reinterpret_cast<ImTextureID>(m_musicTexture->getID()),
                     ImVec2(0.5f * GUIWidth, 0.1f * GUIHeight));

        ImGui::SetCursorPos(ImVec2(0.47f * GUIWidth, 0.5f * GUIHeight));
        ImGui::Checkbox("music", &m_musicCheckBoxChecked, false);

        // Meteor particles.
        ImGui::SetCursorPos(ImVec2(0.255f * GUIWidth, 0.605f * GUIHeight));
        ImGui::Image(reinterpret_cast<ImTextureID>(m_meteorParticlesTexture->getID()),
                     ImVec2(0.5f * GUIWidth, 0.1f * GUIHeight));

        ImGui::SetCursorPos(ImVec2(0.46f * GUIWidth, 0.62f * GUIHeight));
        ImGui::Checkbox("meteor", &m_meteorParticlesCheckBoxChecked, false);

        // Interface GUI.
        ImGui::SetCursorPos(ImVec2(0.255f * GUIWidth, 0.725f * GUIHeight));
        ImGui::Image(reinterpret_cast<ImTextureID>(m_interfaceGUITexture->getID()),
                     ImVec2(0.5f * GUIWidth, 0.1f * GUIHeight));

        ImGui::SetCursorPos(ImVec2(0.5f * GUIWidth, 0.74f * GUIHeight));
        ImGui::Checkbox("interfaceGUI", &m_interfaceGUICheckBoxChecked, false);

        ImGui::PopFont();
        ImGui::PopStyleColor(8);
        ImGui::End();
    }

    void SettingsMenuGUILayer::resetFPS(int fps)
    {
        m_30FPSChecked = false;
        m_60FPSChecked = false;
        m_120FPSChecked = false;
        m_250FPSChecked = false;

        if(fps == 30)
        {
            m_30FPSChecked = true;
            EnumsAndVars::SettingsMenu::FPSLimit = 30;
        }
        else if (fps == 60)
        {
            m_60FPSChecked = true;
            EnumsAndVars::SettingsMenu::FPSLimit = 60;
        }
        else if (fps == 120)
        {
            m_120FPSChecked = true;
            EnumsAndVars::SettingsMenu::FPSLimit = 120;
        }
        else if (fps == 250)
        {
            m_250FPSChecked = true;
            EnumsAndVars::SettingsMenu::FPSLimit = 250;
        }

        BR_INFO("Set FPSLimit: %d", EnumsAndVars::SettingsMenu::FPSLimit);
        Beryll::GameLoop::setFPSLimit(EnumsAndVars::SettingsMenu::FPSLimit);

        DataBaseHelper::storeSettingsFPSLimit(EnumsAndVars::SettingsMenu::FPSLimit);
    }
}
