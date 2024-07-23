#include "SettingsMenuGUILayer.h"
#include "EnumsAndVariables.h"
#include "GameStateHelper.h"
#include "DataBaseHelper.h"

namespace MagneticBall3D
{
    // All there IDs as strings required by ImGUI.
    const std::string SettingsMenuGUILayer::m_buttonBackID = std::to_string(BeryllUtils::Common::generateID());
    const std::string SettingsMenuGUILayer::m_settingsTextureID = std::to_string(BeryllUtils::Common::generateID());
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
        m_buttonBackTexture = Beryll::Renderer::createTexture("GUI/menus/LeftArrow.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);

        m_settingsTexture = Beryll::Renderer::createTexture("GUI/menus/settings/Settings.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
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
        if(m_buttonBackClicked)
        {
            m_buttonBackClicked = false;
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
        // Button back.
        ImGui::SetNextWindowPos(ImVec2(0.0f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.9f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.

        ImGui::Begin(m_buttonBackID.c_str(), nullptr, m_noBackgroundNoFrame);
        m_buttonBackClicked = ImGui::ImageButton(m_buttonBackID.c_str(),reinterpret_cast<ImTextureID>(m_buttonBackTexture->getID()),
                                                 ImVec2(0.3f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.1f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::End();

        // Settings texture.
        ImGui::SetNextWindowPos(ImVec2(0.3f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.0f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.

        ImGui::Begin(m_settingsTextureID.c_str(), nullptr, m_noBackgroundNoFrame);
        ImGui::Image(reinterpret_cast<ImTextureID>(m_settingsTexture->getID()),
                     ImVec2(0.4f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.06f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::End();

        // FPS limit.
        ImGui::SetNextWindowPos(ImVec2(0.01f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.08f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.

        ImGui::Begin(m_FPSLimitTextureID.c_str(), nullptr, m_noBackgroundNoFrame);
        ImGui::Image(reinterpret_cast<ImTextureID>(m_FPSLimitTexture->getID()),
                     ImVec2(0.24f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.03f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::End();

        // FPS check boxes.
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 0.0f, 0.0f, 0.0f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4{ 0.0f, 0.0f, 0.0f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f });

        ImGui::SetNextWindowPos(ImVec2(0.26f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.08f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));

        ImGui::Begin(m_30FPSCheckBoxID.c_str(), nullptr, m_noBackgroundNoFrame);
        ImGui::PushFont(m_fontForAllCheckBoxes);
        if(ImGui::Checkbox("30", &m_30FPSChecked, false))
        {
            resetFPS(30);
        }
        ImGui::PopFont();
        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(0.41f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.08f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));

        ImGui::Begin(m_60FPSCheckBoxID.c_str(), nullptr, m_noBackgroundNoFrame);
        ImGui::PushFont(m_fontForAllCheckBoxes);
        if(ImGui::Checkbox("60", &m_60FPSChecked, false))
        {
            resetFPS(60);
        }
        ImGui::PopFont();
        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(0.56f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.08f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));

        ImGui::Begin(m_120FPSCheckBoxID.c_str(), nullptr, m_noBackgroundNoFrame);
        ImGui::PushFont(m_fontForAllCheckBoxes);
        if(ImGui::Checkbox("120", &m_120FPSChecked, false))
        {
            resetFPS(120);
        }
        ImGui::PopFont();
        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(0.74f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.08f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
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
        ImGui::SetNextWindowPos(ImVec2(0.01f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.12f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.

        ImGui::Begin(m_FPSTipTextureID.c_str(), nullptr, m_noBackgroundNoFrame);
        ImGui::Image(reinterpret_cast<ImTextureID>(m_FPSTipTexture->getID()),
                     ImVec2(0.97f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.03f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::End();

        // Background music.
        ImGui::SetNextWindowPos(ImVec2(0.01f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.18f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.

        ImGui::Begin(m_musicTextureID.c_str(), nullptr, m_noBackgroundNoFrame);
        ImGui::Image(reinterpret_cast<ImTextureID>(m_musicTexture->getID()),
                     ImVec2(0.49f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.03f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::End();

        // Background music check box.
        ImGui::SetNextWindowPos(ImVec2(0.51f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.18f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));

        ImGui::Begin(m_musicCheckBoxID.c_str(), nullptr, m_noBackgroundNoFrame);
        ImGui::PushFont(m_fontForAllCheckBoxes);
        ImGui::Checkbox((std::string("##ImGUILibrarySpecificID") + m_musicCheckBoxID).c_str(), &m_musicCheckBoxChecked, false);
        ImGui::PopFont();
        ImGui::End();

        // Meteor particles.
        ImGui::SetNextWindowPos(ImVec2(0.01f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.24f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.

        ImGui::Begin(m_meteorParticlesTextureID.c_str(), nullptr, m_noBackgroundNoFrame);
        ImGui::Image(reinterpret_cast<ImTextureID>(m_meteorParticlesTexture->getID()),
                     ImVec2(0.45f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.03f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::End();

        // Meteor particles check box.
        ImGui::SetNextWindowPos(ImVec2(0.47f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.24f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
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
