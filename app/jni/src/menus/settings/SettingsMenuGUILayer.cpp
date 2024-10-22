#include "SettingsMenuGUILayer.h"
#include "EnumsAndVariables.h"
#include "GameStateHelper.h"
#include "DataBaseHelper.h"

namespace MagneticBall3D
{
    SettingsMenuGUILayer::SettingsMenuGUILayer()
    {
        m_backButtonTexture = Beryll::Renderer::createTexture("GUI/menus/LeftArrow.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);

        m_backgroundTexture = Beryll::Renderer::createTexture("GUI/menus/shop/ShopBackground.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_FPSLimitTexture = Beryll::Renderer::createTexture("GUI/menus/settings/FPSLimit.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_FPSTipTexture = Beryll::Renderer::createTexture("GUI/menus/settings/FPSTip.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_musicTexture = Beryll::Renderer::createTexture("GUI/menus/settings/BackgroundMusic.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_meteorParticlesTexture = Beryll::Renderer::createTexture("GUI/menus/settings/MeteorParticles.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);

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
    }

    void SettingsMenuGUILayer::updateAfterPhysics()
    {

    }

    void SettingsMenuGUILayer::draw()
    {
        const float GUIWidth = Beryll::MainImGUI::getInstance()->getGUIWidth();
        const float GUIHeight = Beryll::MainImGUI::getInstance()->getGUIHeight();

        ImGui::SetNextWindowPos(ImVec2(-0.005f * GUIWidth, -0.005f * GUIHeight));
        ImGui::SetNextWindowSize(ImVec2(1.01f * GUIWidth, 1.01f * GUIHeight));
        ImGui::Begin("shopMenu", nullptr, m_noBackgroundNoFrameNoFocus);

        // Background.
        ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
        ImGui::Image(reinterpret_cast<ImTextureID>(m_backgroundTexture->getID()),
                     ImVec2(1.01f * GUIWidth, 1.01f * GUIHeight));

        // Back.
        ImGui::SetCursorPos(ImVec2(0.005f * GUIWidth, 0.855f * GUIHeight));
        m_backButtonClicked = ImGui::ImageButton("backButton", reinterpret_cast<ImTextureID>(m_backButtonTexture->getID()),
                                                 ImVec2(0.15f * GUIWidth, 0.15f * GUIHeight));

        // FPS limit.
        ImGui::SetCursorPos(ImVec2(0.255f * GUIWidth, 0.205f * GUIHeight));
        ImGui::Image(reinterpret_cast<ImTextureID>(m_FPSLimitTexture->getID()),
                     ImVec2(0.5f * GUIWidth, 0.1f * GUIHeight));

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
        ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4{ 0.0f, 0.0f, 0.0f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f });
        ImGui::PushFont(m_fontForAllCheckBoxes);

        // FPS check boxes.
        ImGui::SetCursorPos(ImVec2(0.4f * GUIWidth, 0.22f * GUIHeight));
        if(ImGui::Checkbox("030", &m_30FPSChecked, false))
            resetFPS(30);

        ImGui::SetCursorPos(ImVec2(0.5f * GUIWidth, 0.22f * GUIHeight));
        if(ImGui::Checkbox("060", &m_60FPSChecked, false))
            resetFPS(60);

        ImGui::SetCursorPos(ImVec2(0.6f * GUIWidth, 0.22f * GUIHeight));
        if(ImGui::Checkbox("120", &m_120FPSChecked, false))
            resetFPS(120);

        ImGui::SetCursorPos(ImVec2(0.7f * GUIWidth, 0.22f * GUIHeight));
        if(ImGui::Checkbox("250", &m_250FPSChecked, false))
            resetFPS(250);

        // FPS tip.
        ImGui::SetCursorPos(ImVec2(0.255f * GUIWidth, 0.325f * GUIHeight));
        ImGui::Image(reinterpret_cast<ImTextureID>(m_FPSTipTexture->getID()),
                     ImVec2(0.5f * GUIWidth, 0.1f * GUIHeight));

        // Background music.
        ImGui::SetCursorPos(ImVec2(0.255f * GUIWidth, 0.445f * GUIHeight));
        ImGui::Image(reinterpret_cast<ImTextureID>(m_musicTexture->getID()),
                     ImVec2(0.5f * GUIWidth, 0.1f * GUIHeight));

        ImGui::SetCursorPos(ImVec2(0.5f * GUIWidth, 0.46f * GUIHeight));
        ImGui::Checkbox("music", &m_musicCheckBoxChecked, false);

        // Meteor particles.
        ImGui::SetCursorPos(ImVec2(0.255f * GUIWidth, 0.565f * GUIHeight));
        ImGui::Image(reinterpret_cast<ImTextureID>(m_meteorParticlesTexture->getID()),
                     ImVec2(0.5f * GUIWidth, 0.1f * GUIHeight));

        ImGui::SetCursorPos(ImVec2(0.5f * GUIWidth, 0.58f * GUIHeight));
        ImGui::Checkbox("meteor", &m_meteorParticlesCheckBoxChecked, false);

        ImGui::PopFont();
        ImGui::PopStyleColor(5);
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
