#include "EnergySystem.h"
#include "EnumsAndVariables.h"
#include "DataBaseHelper.h"

namespace MagneticBall3D
{
    // All there IDs as strings required by ImGUI.
    const std::string EnergySystem::m_energyTextureID = std::to_string(BeryllUtils::Common::generateID());
    const std::string EnergySystem::m_energyAmountTextID = std::to_string(BeryllUtils::Common::generateID());
    const std::string EnergySystem::m_restoreTimerTextID = std::to_string(BeryllUtils::Common::generateID());
    const std::string EnergySystem::m_energyButtonID = std::to_string(BeryllUtils::Common::generateID());

    EnergySystem::EnergySystem()
    {
        BR_INFO("%s", "EnergySystem::EnergySystem()");

        m_energyTexture = Beryll::Renderer::createTexture("GUI/menus/start/Energy.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_energyButtonTexture = Beryll::Renderer::createTexture("GUI/FullTransparent.png", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);

        m_energyAmountFont = Beryll::MainImGUI::getInstance()->createFont(EnumsAndVars::FontsPath::roboto, 0.025f);
        m_restoreTimerFont = Beryll::MainImGUI::getInstance()->createFont(EnumsAndVars::FontsPath::roboto, 0.015f);
    }

    EnergySystem::~EnergySystem()
    {

    }

    void EnergySystem::update()
    {
        m_currentSec = Beryll::TimeStep::getSecSinceEpoch();

        if(EnumsAndVars::EnergySystem::lastSecUpdated < m_currentSec)
        {
            //BR_INFO("if(EnumsAndVars::EnergySystem::lastSecUpdated < currentSec) %d", m_currentSec);
            EnumsAndVars::EnergySystem::lastSecUpdated = m_currentSec;
            DataBaseHelper::storeEnergySystemLastSecUpdated(EnumsAndVars::EnergySystem::lastSecUpdated);

            if(EnumsAndVars::EnergySystem::currentAmount >= EnumsAndVars::EnergySystem::maxLimitToRestore)
            {
                // Reset timer if we have max or more(more than max can be if user will buy energy for real money. There is no limit)
                // If currentAmount will drop below maxLimitToRestore, energy restore process will  start from currentSec time.
                EnumsAndVars::EnergySystem::lastSecOneEnergyRestored = m_currentSec;
                DataBaseHelper::storeEnergySystemLastSecRestored(EnumsAndVars::EnergySystem::lastSecOneEnergyRestored);
            }
            else if(EnumsAndVars::EnergySystem::lastSecOneEnergyRestored + EnumsAndVars::EnergySystem::secToRestoreOneEnergy <= m_currentSec)
            {
                //BR_INFO("%s", "Need and can restore one or more energy.");
                // Time when one energy can be restored. Or more if app launched after long time inactive.
                const int amountCanBeRestored = (m_currentSec - EnumsAndVars::EnergySystem::lastSecOneEnergyRestored) / EnumsAndVars::EnergySystem::secToRestoreOneEnergy;
                const int newAmount = EnumsAndVars::EnergySystem::currentAmount + amountCanBeRestored;
                if(newAmount >= EnumsAndVars::EnergySystem::maxLimitToRestore)
                {
                    EnumsAndVars::EnergySystem::currentAmount = EnumsAndVars::EnergySystem::maxLimitToRestore;
                    EnumsAndVars::EnergySystem::lastSecOneEnergyRestored = m_currentSec;
                }
                else
                {
                    int amountRestored = newAmount - EnumsAndVars::EnergySystem::currentAmount;
                    EnumsAndVars::EnergySystem::currentAmount = newAmount;
                    EnumsAndVars::EnergySystem::lastSecOneEnergyRestored += EnumsAndVars::EnergySystem::secToRestoreOneEnergy * amountRestored;
                }

                //BR_INFO("amountCanBeRestored %d after restore %d", amountCanBeRestored, EnumsAndVars::EnergySystem::currentAmount);
                DataBaseHelper::storeEnergySystemCurrentAmount(EnumsAndVars::EnergySystem::currentAmount);
                DataBaseHelper::storeEnergySystemLastSecRestored(EnumsAndVars::EnergySystem::lastSecOneEnergyRestored);
            }
        }

        if(m_energyButtonClicked)
        {
            BR_INFO("%s", "m_energyButtonClicked");
        }
    }

    void EnergySystem::draw()
    {
        float GUIWidth = Beryll::MainImGUI::getInstance()->getGUIWidth();
        float GUIHeight = Beryll::MainImGUI::getInstance()->getGUIHeight();
        // Text amount.
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 0.0f, 0.0f, 0.0f, 1.0f });
        ImGui::SetNextWindowPos(ImVec2(0.45f * GUIWidth, 0.008f * GUIHeight));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));

        ImGui::Begin(m_energyAmountTextID.c_str(), nullptr, m_noBackgroundNoFrame | ImGuiWindowFlags_NoBringToFrontOnFocus);

        ImGui::PushFont(m_energyAmountFont);
        ImGui::Text("%d/%d", EnumsAndVars::EnergySystem::currentAmount, EnumsAndVars::EnergySystem::maxLimitToRestore);
        ImGui::PopFont();

        ImGui::End();
        ImGui::PopStyleColor(1);

        // Text restore timer.
        m_restoreTimerText = "";
        if(EnumsAndVars::EnergySystem::currentAmount < EnumsAndVars::EnergySystem::maxLimitToRestore &&
           EnumsAndVars::EnergySystem::lastSecOneEnergyRestored + EnumsAndVars::EnergySystem::secToRestoreOneEnergy >= m_currentSec)
        {
            // Set timer about restore one energy.
            uint64_t secLeft = (EnumsAndVars::EnergySystem::lastSecOneEnergyRestored + EnumsAndVars::EnergySystem::secToRestoreOneEnergy) - m_currentSec;
            if(secLeft > EnumsAndVars::EnergySystem::secToRestoreOneEnergy)
                secLeft = EnumsAndVars::EnergySystem::secToRestoreOneEnergy;

            int min = secLeft / 60;
            int sec = secLeft % 60;

            if(min < 10)
                m_restoreTimerText += "0";

            m_restoreTimerText += std::to_string(min);
            m_restoreTimerText += ":";

            if(sec < 10)
                m_restoreTimerText += "0";

            m_restoreTimerText += std::to_string(sec);
        }
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 0.0f, 0.0f, 0.0f, 1.0f });
        ImGui::SetNextWindowPos(ImVec2(0.47f * GUIWidth, 0.033f * GUIHeight));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));

        ImGui::Begin(m_restoreTimerTextID.c_str(), nullptr, m_noBackgroundNoFrame | ImGuiWindowFlags_NoBringToFrontOnFocus);

        ImGui::PushFont(m_restoreTimerFont);
        ImGui::Text("%s", m_restoreTimerText.c_str()); // ImGUI ignores "%s". Modify void ImFormatStringToTempBufferV( to avoid that.
        ImGui::PopFont();

        ImGui::End();
        ImGui::PopStyleColor(1);

        // Texture energy.
        ImGui::SetNextWindowPos(ImVec2(0.39f * GUIWidth, 0.0f * GUIHeight));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.

        ImGui::Begin(m_energyTextureID.c_str(), nullptr, m_noBackgroundNoFrame | ImGuiWindowFlags_NoBringToFrontOnFocus);

        ImGui::Image(reinterpret_cast<ImTextureID>(m_energyTexture->getID()),
                     ImVec2(0.2f * GUIWidth, 0.05f * GUIHeight));

        ImGui::End();

        // Button energy. Transparent. On top of texture + texts.
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::SetNextWindowPos(ImVec2(0.39f * GUIWidth, 0.0f * GUIHeight));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.

        ImGui::Begin(m_energyButtonID.c_str(), nullptr, m_noBackgroundNoFrame);

        m_energyButtonClicked = ImGui::ImageButton(m_energyButtonID.c_str(), reinterpret_cast<ImTextureID>(m_energyButtonTexture->getID()),
                                                   ImVec2(0.2f * GUIWidth, 0.05f * GUIHeight));

        ImGui::End();
        ImGui::PopStyleColor(3);
    }

    bool EnergySystem::isEnoughForPlay()
    {
        return EnumsAndVars::EnergySystem::currentAmount >= EnumsAndVars::EnergySystem::playCost;
    }

    void EnergySystem::handlePlay()
    {
        EnumsAndVars::EnergySystem::currentAmount -= EnumsAndVars::EnergySystem::playCost;
        DataBaseHelper::storeEnergySystemCurrentAmount(EnumsAndVars::EnergySystem::currentAmount);
    }
}
