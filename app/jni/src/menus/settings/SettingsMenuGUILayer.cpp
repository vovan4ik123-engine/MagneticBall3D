#include "SettingsMenuGUILayer.h"
#include "EnumsAndVariables.h"
#include "GameStateHelper.h"
#include "DataBaseHelper.h"

namespace MagneticBall3D
{
    // All there IDs as strings required by ImGUI.
    const std::string SettingsMenuGUILayer::m_backButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string SettingsMenuGUILayer::m_settingsHeaderID = std::to_string(BeryllUtils::Common::generateID());
    const std::string SettingsMenuGUILayer::m_FPSLimitTextureID = std::to_string(BeryllUtils::Common::generateID());
    const std::string SettingsMenuGUILayer::m_FPSTipTextureID = std::to_string(BeryllUtils::Common::generateID());
    const std::string SettingsMenuGUILayer::m_30FPSCheckBoxID = std::to_string(BeryllUtils::Common::generateID());
    const std::string SettingsMenuGUILayer::m_60FPSCheckBoxID = std::to_string(BeryllUtils::Common::generateID());
    const std::string SettingsMenuGUILayer::m_120FPSCheckBoxID = std::to_string(BeryllUtils::Common::generateID());
    const std::string SettingsMenuGUILayer::m_250FPSCheckBoxID = std::to_string(BeryllUtils::Common::generateID());
    const std::string SettingsMenuGUILayer::m_musicTextureID = std::to_string(BeryllUtils::Common::generateID());
    const std::string SettingsMenuGUILayer::m_musicCheckBoxID = std::to_string(BeryllUtils::Common::generateID());
    const std::string SettingsMenuGUILayer::m_meteorParticlesTextureID = std::to_string(BeryllUtils::Common::generateID());
    const std::string SettingsMenuGUILayer::m_meteorParticlesCheckBoxID = std::to_string(BeryllUtils::Common::generateID());

    SettingsMenuGUILayer::SettingsMenuGUILayer()
    {
        m_backButtonTexture = Beryll::Renderer::createTexture("GUI/menus/LeftArrow.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);

        m_settingsHeaderTexture = Beryll::Renderer::createTexture("GUI/menus/settings/SettingsHeader.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_FPSLimitTexture = Beryll::Renderer::createTexture("GUI/menus/settings/FPSLimit.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_FPSTipTexture = Beryll::Renderer::createTexture("GUI/menus/settings/FPSTip.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_musicTexture = Beryll::Renderer::createTexture("GUI/menus/settings/BackgroundMusic.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_meteorParticlesTexture = Beryll::Renderer::createTexture("GUI/menus/settings/MeteorParticles.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);

        m_fontForAllCheckBoxes = Beryll::MainImGUI::getInstance()->createFont(EnumsAndVars::FontsPath::roboto, 0.03f);

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
        float GUIWidth = Beryll::MainImGUI::getInstance()->getGUIWidth();
        float GUIHeight = Beryll::MainImGUI::getInstance()->getGUIHeight();
        // Back.
        ImGui::SetNextWindowPos(ImVec2(-0.01f * GUIWidth, 0.9f * GUIHeight));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.

        ImGui::Begin(m_backButtonID.c_str(), nullptr, m_noBackgroundNoFrame);
        m_backButtonClicked = ImGui::ImageButton(m_backButtonID.c_str(), reinterpret_cast<ImTextureID>(m_backButtonTexture->getID()),
                                                 ImVec2(0.34f * GUIWidth, 0.105f * GUIHeight));
        ImGui::End();

        // Settings header.
        ImGui::SetNextWindowPos(ImVec2(0.3f * GUIWidth, 0.0f * GUIHeight));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.

        ImGui::Begin(m_settingsHeaderID.c_str(), nullptr, m_noBackgroundNoFrame);
        ImGui::Image(reinterpret_cast<ImTextureID>(m_settingsHeaderTexture->getID()),
                     ImVec2(0.4f * GUIWidth, 0.06f * GUIHeight));
        ImGui::End();

        // FPS limit.
        ImGui::SetNextWindowPos(ImVec2(0.01f * GUIWidth, 0.08f * GUIHeight));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.

        ImGui::Begin(m_FPSLimitTextureID.c_str(), nullptr, m_noBackgroundNoFrame);
        ImGui::Image(reinterpret_cast<ImTextureID>(m_FPSLimitTexture->getID()),
                     ImVec2(0.24f * GUIWidth, 0.03f * GUIHeight));
        ImGui::End();

        // FPS check boxes.
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 0.0f, 0.0f, 0.0f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4{ 0.0f, 0.0f, 0.0f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f });

        ImGui::SetNextWindowPos(ImVec2(0.26f * GUIWidth, 0.08f * GUIHeight));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));

        ImGui::Begin(m_30FPSCheckBoxID.c_str(), nullptr, m_noBackgroundNoFrame);
        ImGui::PushFont(m_fontForAllCheckBoxes);
        if(ImGui::Checkbox("30", &m_30FPSChecked, false))
        {
            resetFPS(30);
        }
        ImGui::PopFont();
        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(0.41f * GUIWidth, 0.08f * GUIHeight));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));

        ImGui::Begin(m_60FPSCheckBoxID.c_str(), nullptr, m_noBackgroundNoFrame);
        ImGui::PushFont(m_fontForAllCheckBoxes);
        if(ImGui::Checkbox("60", &m_60FPSChecked, false))
        {
            resetFPS(60);
        }
        ImGui::PopFont();
        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(0.56f * GUIWidth, 0.08f * GUIHeight));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));

        ImGui::Begin(m_120FPSCheckBoxID.c_str(), nullptr, m_noBackgroundNoFrame);
        ImGui::PushFont(m_fontForAllCheckBoxes);
        if(ImGui::Checkbox("120", &m_120FPSChecked, false))
        {
            resetFPS(120);
        }
        ImGui::PopFont();
        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(0.74f * GUIWidth, 0.08f * GUIHeight));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));

        ImGui::Begin(m_250FPSCheckBoxID.c_str(), nullptr, m_noBackgroundNoFrame);
        ImGui::PushFont(m_fontForAllCheckBoxes);
        if(ImGui::Checkbox("250", &m_250FPSChecked, false))
        {
            resetFPS(250);
        }
        ImGui::PopFont();
        ImGui::End();

        // FPS tip.
        ImGui::SetNextWindowPos(ImVec2(0.01f * GUIWidth, 0.12f * GUIHeight));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.

        ImGui::Begin(m_FPSTipTextureID.c_str(), nullptr, m_noBackgroundNoFrame);
        ImGui::Image(reinterpret_cast<ImTextureID>(m_FPSTipTexture->getID()),
                     ImVec2(0.97f * GUIWidth, 0.03f * GUIHeight));
        ImGui::End();

        // Background music.
        ImGui::SetNextWindowPos(ImVec2(0.01f * GUIWidth, 0.18f * GUIHeight));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.

        ImGui::Begin(m_musicTextureID.c_str(), nullptr, m_noBackgroundNoFrame);
        ImGui::Image(reinterpret_cast<ImTextureID>(m_musicTexture->getID()),
                     ImVec2(0.49f * GUIWidth, 0.03f * GUIHeight));
        ImGui::End();

        // Background music check box.
        ImGui::SetNextWindowPos(ImVec2(0.51f * GUIWidth, 0.18f * GUIHeight));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));

        ImGui::Begin(m_musicCheckBoxID.c_str(), nullptr, m_noBackgroundNoFrame);
        ImGui::PushFont(m_fontForAllCheckBoxes);
        ImGui::Checkbox((std::string("##ImGUILibrarySpecificID") + m_musicCheckBoxID).c_str(), &m_musicCheckBoxChecked, false);
        ImGui::PopFont();
        ImGui::End();

        // Meteor particles.
        ImGui::SetNextWindowPos(ImVec2(0.01f * GUIWidth, 0.24f * GUIHeight));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.

        ImGui::Begin(m_meteorParticlesTextureID.c_str(), nullptr, m_noBackgroundNoFrame);
        ImGui::Image(reinterpret_cast<ImTextureID>(m_meteorParticlesTexture->getID()),
                     ImVec2(0.45f * GUIWidth, 0.03f * GUIHeight));
        ImGui::End();

        // Meteor particles check box.
        ImGui::SetNextWindowPos(ImVec2(0.47f * GUIWidth, 0.24f * GUIHeight));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));

        ImGui::Begin(m_meteorParticlesCheckBoxID.c_str(), nullptr, m_noBackgroundNoFrame);
        ImGui::PushFont(m_fontForAllCheckBoxes);
        ImGui::Checkbox((std::string("##ImGUILibrarySpecificID") + m_meteorParticlesCheckBoxID).c_str(), &m_meteorParticlesCheckBoxChecked, false);
        ImGui::PopFont();
        ImGui::End();

        ImGui::PopStyleColor(5);
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
