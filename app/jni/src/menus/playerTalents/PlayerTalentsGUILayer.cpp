#include "PlayerTalentsGUILayer.h"
#include "EnumsAndVariables.h"
#include "GameStateHelper.h"

namespace MagneticBall3D
{
    const std::string PlayerTalentsGUILayer::m_backButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayerTalentsGUILayer::m_talentsHeaderID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayerTalentsGUILayer::m_allTalentsMenuID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayerTalentsGUILayer::m_maxSpeedButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayerTalentsGUILayer::m_magneticRadiusButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayerTalentsGUILayer::m_amountOfMagnetizedGarbageButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayerTalentsGUILayer::m_accelerateFasterButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayerTalentsGUILayer::m_ballAndGarbageProtectionButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayerTalentsGUILayer::m_resurrectionAttemptsButtonID = std::to_string(BeryllUtils::Common::generateID());

    PlayerTalentsGUILayer::PlayerTalentsGUILayer()
    {
        m_backButtonTexture = Beryll::Renderer::createTexture("GUI/menus/LeftArrow.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);

        m_talentsHeaderTexture = Beryll::Renderer::createTexture("GUI/menus/playerTalents/TalentsHeader.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_maxSpeedButtonTexture = Beryll::Renderer::createTexture("GUI/menus/playerTalents/MaxSpeed.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_magneticRadiusButtonTexture = Beryll::Renderer::createTexture("GUI/menus/playerTalents/MagneticRadius.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_amountOfMagnetizedGarbageButtonTexture = Beryll::Renderer::createTexture("GUI/menus/playerTalents/AmountOfMagnetizedGarbage.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_accelerateFasterButtonTexture = Beryll::Renderer::createTexture("GUI/menus/playerTalents/AccelerateFaster.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_ballAndGarbageProtectionButtonTexture = Beryll::Renderer::createTexture("GUI/menus/playerTalents/BallAndGarbageProtection.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_resurrectionAttemptsButtonTexture = Beryll::Renderer::createTexture("GUI/menus/playerTalents/ResurrectionAttempts.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
    }

    PlayerTalentsGUILayer::~PlayerTalentsGUILayer()
    {

    }

    void PlayerTalentsGUILayer::updateBeforePhysics()
    {
        if (m_backButtonClicked)
        {
            m_backButtonClicked = false;
            GameStateHelper::popState();
            return;
        }
        else if(m_maxSpeedButtonClicked)
        {
            m_maxSpeedButtonClicked = false;
            BR_INFO("%s", "Max speed clicked.")
        }
        else if(m_magneticRadiusButtonClicked)
        {
            m_magneticRadiusButtonClicked = false;
            BR_INFO("%s", "Magnetic radius clicked.")
        }
        else if(m_amountOfMagnetizedGarbageButtonClicked)
        {
            m_amountOfMagnetizedGarbageButtonClicked = false;
            BR_INFO("%s", "Amount of magnetized clicked.")
        }
        else if(m_accelerateFasterButtonClicked)
        {
            m_accelerateFasterButtonClicked= false;
            BR_INFO("%s", "Accelerate faster clicked.")
        }
        else if(m_ballAndGarbageProtectionButtonClicked)
        {
            m_ballAndGarbageProtectionButtonClicked = false;
            BR_INFO("%s", "Ball and garbage protection clicked.")
        }
        else if(m_resurrectionAttemptsButtonClicked)
        {
            m_resurrectionAttemptsButtonClicked= false;
            BR_INFO("%s", "Resurrection attempts clicked.")
        }
    }

    void PlayerTalentsGUILayer::updateAfterPhysics()
    {

    }

    void PlayerTalentsGUILayer::draw()
    {
        // Back.
        ImGui::SetNextWindowPos(ImVec2(-0.01f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.9f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.

        ImGui::Begin(m_backButtonID.c_str(), nullptr, m_noBackgroundNoFrameNoFocus);
        m_backButtonClicked = ImGui::ImageButton(m_backButtonID.c_str(), reinterpret_cast<ImTextureID>(m_backButtonTexture->getID()),
                                                 ImVec2(0.34f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.105f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::End();

        // Talents header.
        ImGui::SetNextWindowPos(ImVec2(0.3f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.0f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.

        ImGui::Begin(m_talentsHeaderID.c_str(), nullptr, m_noBackgroundNoFrameNoFocus);
        ImGui::Image(reinterpret_cast<ImTextureID>(m_talentsHeaderTexture->getID()),
                     ImVec2(0.4f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.06f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::End();

        // All talents.
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{ 0.5f, 0.0f, 0.0f, 1.0f });
        ImGui::SetNextWindowPos(ImVec2(0.0f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.1f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(1.0f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.65f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::Begin(m_allTalentsMenuID.c_str(), nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                                                             ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
        // Max speed.
        ImGui::SetCursorPos(ImVec2(0.02f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.05f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        m_maxSpeedButtonClicked = ImGui::ImageButton(m_maxSpeedButtonID.c_str(), reinterpret_cast<ImTextureID>(m_maxSpeedButtonTexture->getID()),
                                                     ImVec2(0.3f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.15f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        // Magnetic radius.
        ImGui::SetCursorPos(ImVec2(0.35f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.05f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        m_magneticRadiusButtonClicked = ImGui::ImageButton(m_magneticRadiusButtonID.c_str(), reinterpret_cast<ImTextureID>(m_magneticRadiusButtonTexture->getID()),
                                                           ImVec2(0.3f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.15f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        // Amount of magnetized garbage.
        ImGui::SetCursorPos(ImVec2(0.68f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.05f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        m_amountOfMagnetizedGarbageButtonClicked = ImGui::ImageButton(m_amountOfMagnetizedGarbageButtonID.c_str(), reinterpret_cast<ImTextureID>(m_amountOfMagnetizedGarbageButtonTexture->getID()),
                                                                      ImVec2(0.3f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.15f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        // Accelerate faster.
        ImGui::SetCursorPos(ImVec2(0.02f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.225f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        m_accelerateFasterButtonClicked = ImGui::ImageButton(m_accelerateFasterButtonID.c_str(), reinterpret_cast<ImTextureID>(m_accelerateFasterButtonTexture->getID()),
                                                             ImVec2(0.3f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.15f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        // Ball and garbage protection.
        ImGui::SetCursorPos(ImVec2(0.35f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.225f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        m_ballAndGarbageProtectionButtonClicked = ImGui::ImageButton(m_ballAndGarbageProtectionButtonID.c_str(), reinterpret_cast<ImTextureID>(m_ballAndGarbageProtectionButtonTexture->getID()),
                                                             ImVec2(0.3f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.15f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        //Resurrection attempts.
        ImGui::SetCursorPos(ImVec2(0.68f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.225f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        m_resurrectionAttemptsButtonClicked = ImGui::ImageButton(m_resurrectionAttemptsButtonID.c_str(), reinterpret_cast<ImTextureID>(m_resurrectionAttemptsButtonTexture->getID()),
                                                             ImVec2(0.3f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.15f * Beryll::MainImGUI::getInstance()->getGUIHeight()));

        ImGui::End();
        ImGui::PopStyleColor(1);
    }
}
