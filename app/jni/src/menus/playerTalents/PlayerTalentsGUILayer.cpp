#include "PlayerTalentsGUILayer.h"
#include "EnumsAndVariables.h"
#include "GameStateHelper.h"
#include "DataBaseHelper.h"
#include "SendStatisticsHelper.h"

namespace MagneticBall3D
{
    int PlayerTalentsGUILayer::m_selectedIndex = 0;

    const std::string PlayerTalentsGUILayer::m_backButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayerTalentsGUILayer::m_talentsHeaderID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayerTalentsGUILayer::m_allTalentsMenuID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayerTalentsGUILayer::m_maxSpeedButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayerTalentsGUILayer::m_magneticRadiusButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayerTalentsGUILayer::m_amountOfMagnetizedGarbageButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayerTalentsGUILayer::m_accelerateFasterButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayerTalentsGUILayer::m_ballAndGarbageProtectionButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayerTalentsGUILayer::m_resurrectionAttemptsButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayerTalentsGUILayer::m_selectedTalentMenuID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayerTalentsGUILayer::m_improveTalentMenuID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayerTalentsGUILayer::m_improveTalentByAdButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayerTalentsGUILayer::m_improveTalentByCrystalsButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayerTalentsGUILayer::m_noCrystalsMenuID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayerTalentsGUILayer::m_noCrystalsButtonOkID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayerTalentsGUILayer::m_adLoadingMenuID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayerTalentsGUILayer::m_adErrorMenuID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayerTalentsGUILayer::m_adErrorButtonOkID = std::to_string(BeryllUtils::Common::generateID());

    std::atomic<bool> PlayerTalentsGUILayer::m_rewardedAdSuccess = false;
    std::atomic<bool> PlayerTalentsGUILayer::m_rewardedAdError = false;

    PlayerTalentsGUILayer::PlayerTalentsGUILayer()
    {
        //float m_screenAR = Beryll::MainImGUI::getInstance()->getGUIScreenAspectRation();
        //BR_ASSERT((m_screenAR > 0.0f), "%s", "m_screenAR = 0");

        m_transparentTexture = Beryll::Renderer::createTexture("GUI/FullTransparent.png", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_backButtonTexture = Beryll::Renderer::createTexture("GUI/menus/LeftArrow.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);

        m_talentsHeaderTexture = Beryll::Renderer::createTexture("GUI/menus/playerTalents/TalentsHeader.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_maxSpeedButtonTexture = Beryll::Renderer::createTexture("GUI/menus/playerTalents/MaxSpeed.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_magneticRadiusButtonTexture = Beryll::Renderer::createTexture("GUI/menus/playerTalents/MagneticRadius.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_amountOfMagnetizedGarbageButtonTexture = Beryll::Renderer::createTexture("GUI/menus/playerTalents/AmountOfMagnetizedGarbage.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_accelerateFasterButtonTexture = Beryll::Renderer::createTexture("GUI/menus/playerTalents/AccelerateFaster.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_ballAndGarbageProtectionButtonTexture = Beryll::Renderer::createTexture("GUI/menus/playerTalents/BallAndGarbageProtection.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_resurrectionAttemptsButtonTexture = Beryll::Renderer::createTexture("GUI/menus/playerTalents/ResurrectionAttempts.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);


        m_improveByAdTexture = Beryll::Renderer::createTexture("GUI/menus/playerTalents/AdImage.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_improveByAdTimerTexture = Beryll::Renderer::createTexture("GUI/menus/playerTalents/AdImageTimer.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_improveByCrystalsTexture = Beryll::Renderer::createTexture("GUI/menus/playerTalents/CrystalsImage.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_maxLevelReachedTexture = Beryll::Renderer::createTexture("GUI/menus/playerTalents/MaxLevelReached.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);

        m_noCrystalsTexture = Beryll::Renderer::createTexture("GUI/NotEnoughCrystals.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_noCrystalsButtonOkTexture = Beryll::Renderer::createTexture("GUI/Ok.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_adLoadingTexture = Beryll::Renderer::createTexture("GUI/AdLoading.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_adErrorTexture = Beryll::Renderer::createTexture("GUI/SomethingWentWrong.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_adErrorButtonOkTexture = Beryll::Renderer::createTexture("GUI/Ok.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);

        m_selectedDescriptionFont = Beryll::MainImGUI::getInstance()->createFont(EnumsAndVars::FontsPath::roboto, 0.024f);
        m_selectedValueLevelFont = Beryll::MainImGUI::getInstance()->createFont(EnumsAndVars::FontsPath::roboto, 0.021f);
        m_valueToAddFont = Beryll::MainImGUI::getInstance()->createFont(EnumsAndVars::FontsPath::roboto, 0.03f);
        m_adTimerFont = Beryll::MainImGUI::getInstance()->createFont(EnumsAndVars::FontsPath::roboto, 0.035f);

        selectTalent(m_selectedIndex);

        // These callbacks are called from different thread.
        m_rewardedAdSuccessCallback = []() -> void { BR_INFO("%s", "m_resurrectAdSuccessCallback()"); PlayerTalentsGUILayer::m_rewardedAdSuccess = true; };
        m_rewardedAdErrorCallback = []() -> void { BR_INFO("%s", "m_rewardedAdErrorCallback()"); PlayerTalentsGUILayer::m_rewardedAdError = true; };
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
            BR_INFO("%s", "Max speed clicked.");
            selectTalent(0);
        }
        else if(m_magneticRadiusButtonClicked)
        {
            m_magneticRadiusButtonClicked = false;
            BR_INFO("%s", "Magnetic radius clicked.");
            selectTalent(1);
        }
        else if(m_amountOfMagnetizedGarbageButtonClicked)
        {
            m_amountOfMagnetizedGarbageButtonClicked = false;
            BR_INFO("%s", "Amount of magnetized clicked.");
            selectTalent(2);
        }
        else if(m_accelerateFasterButtonClicked)
        {
            m_accelerateFasterButtonClicked = false;
            BR_INFO("%s", "Accelerate faster clicked.");
            selectTalent(3);
        }
        else if(m_ballAndGarbageProtectionButtonClicked)
        {
            m_ballAndGarbageProtectionButtonClicked = false;
            BR_INFO("%s", "Ball and garbage protection clicked.");
            selectTalent(4);
        }
        else if(m_resurrectionAttemptsButtonClicked)
        {
            m_resurrectionAttemptsButtonClicked = false;
            BR_INFO("%s", "Resurrection attempts clicked.");
            selectTalent(5);
        }
        else if(m_improveByAdClicked)
        {
            m_improveByAdClicked = false;
            BR_INFO("%s", "m_improveByAdClicked.");
            m_adLoadingMenuShow = true;
            Beryll::Ads::getInstance()->showRewardedAd(m_rewardedAdSuccessCallback, m_rewardedAdErrorCallback, false);
        }
        else if(m_improveByCrystalsClicked)
        {
            m_improveByCrystalsClicked = false;
            BR_INFO("%s", "m_improveByCrystalsClicked.");

            if(EnumsAndVars::allPlayerTalents[m_selectedIndex].priceCrystals <= EnumsAndVars::CurrencyBalance::crystals)
            {
                EnumsAndVars::allPlayerTalents[m_selectedIndex].improveLevel(EnumsAndVars::PlayerTalentCurrency::CRYSTALS);
                selectTalent(m_selectedIndex); // Recalculate values.
                SendStatisticsHelper::sendTalentImproved(EnumsAndVars::allPlayerTalents[m_selectedIndex].name, "crystal");
            }
            else
            {
                m_showNoCrystalsMenu = true;
            }
        }

        if(m_noCrystalsButtonOkClicked)
        {
            m_noCrystalsButtonOkClicked = false;
            m_showNoCrystalsMenu = false;
        }

        if(m_adErrorButtonOkClicked)
        {
            m_adErrorButtonOkClicked = false;
            m_adErrorMenuShow = false;
        }

        if(PlayerTalentsGUILayer::m_rewardedAdSuccess)
        {
            PlayerTalentsGUILayer::m_rewardedAdSuccess = false;
            m_adLoadingMenuShow = false;
            EnumsAndVars::allPlayerTalents[m_selectedIndex].improveLevel(EnumsAndVars::PlayerTalentCurrency::AD);
            selectTalent(m_selectedIndex); // Recalculate values.
            SendStatisticsHelper::sendTalentImproved(EnumsAndVars::allPlayerTalents[m_selectedIndex].name, "ad");

            EnumsAndVars::Ads::rewardedAdTime = Beryll::TimeStep::getSecSinceEpoch();
            DataBaseHelper::storeAdsRewardedAdTime(EnumsAndVars::Ads::rewardedAdTime);
        }

        if(PlayerTalentsGUILayer::m_rewardedAdError)
        {
            PlayerTalentsGUILayer::m_rewardedAdError = false;
            m_adLoadingMenuShow = false;
            m_adErrorMenuShow = true;
        }
    }

    void PlayerTalentsGUILayer::updateAfterPhysics()
    {

    }

    void PlayerTalentsGUILayer::draw()
    {
        const float GUIWidth = Beryll::MainImGUI::getInstance()->getGUIWidth();
        const float GUIHeight = Beryll::MainImGUI::getInstance()->getGUIHeight();
        // Back.
        ImGui::SetNextWindowPos(ImVec2(-0.01f * GUIWidth, 0.9f * GUIHeight));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.

        ImGui::Begin(m_backButtonID.c_str(), nullptr, m_noBackgroundNoFrameNoFocus);
        m_backButtonClicked = ImGui::ImageButton(m_backButtonID.c_str(), reinterpret_cast<ImTextureID>(m_backButtonTexture->getID()),
                                                 ImVec2(0.34f * GUIWidth, 0.105f * GUIHeight));
        ImGui::End();

        // Talents header.
        ImGui::SetNextWindowPos(ImVec2(0.3f * GUIWidth, 0.0f));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.

        ImGui::Begin(m_talentsHeaderID.c_str(), nullptr, m_noBackgroundNoFrameNoFocus);
        ImGui::Image(reinterpret_cast<ImTextureID>(m_talentsHeaderTexture->getID()),
                     ImVec2(0.4f * GUIWidth, 0.06f * GUIHeight));
        ImGui::End();

        // All talents.
        float iconSidePixels = 0.3f * GUIWidth;
        float row1TopPos = 0.05f * GUIHeight;
        float row2TopPos = row1TopPos + iconSidePixels + (0.02f * GUIHeight);

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{ 0.5f, 0.0f, 0.0f, 1.0f });
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.1f * GUIHeight));
        ImGui::SetNextWindowSize(ImVec2(GUIWidth, 0.7f * GUIHeight));
        ImGui::Begin(m_allTalentsMenuID.c_str(), nullptr, m_noFrameNoFocus);
        // Max speed.
        ImGui::SetCursorPos(ImVec2(0.02f * GUIWidth, row1TopPos));
        m_maxSpeedButtonClicked = ImGui::ImageButton(m_maxSpeedButtonID.c_str(), reinterpret_cast<ImTextureID>(m_maxSpeedButtonTexture->getID()),
                                                     ImVec2(iconSidePixels, iconSidePixels));
        // Magnetic radius.
        ImGui::SetCursorPos(ImVec2(0.35f * GUIWidth, row1TopPos));
        m_magneticRadiusButtonClicked = ImGui::ImageButton(m_magneticRadiusButtonID.c_str(), reinterpret_cast<ImTextureID>(m_magneticRadiusButtonTexture->getID()),
                                                           ImVec2(iconSidePixels, iconSidePixels));
        // Amount of magnetized garbage.
        ImGui::SetCursorPos(ImVec2(0.68f * GUIWidth, row1TopPos));
        m_amountOfMagnetizedGarbageButtonClicked = ImGui::ImageButton(m_amountOfMagnetizedGarbageButtonID.c_str(), reinterpret_cast<ImTextureID>(m_amountOfMagnetizedGarbageButtonTexture->getID()),
                                                                      ImVec2(iconSidePixels, iconSidePixels));
        // Accelerate faster.
        ImGui::SetCursorPos(ImVec2(0.02f * GUIWidth, row2TopPos));
        m_accelerateFasterButtonClicked = ImGui::ImageButton(m_accelerateFasterButtonID.c_str(), reinterpret_cast<ImTextureID>(m_accelerateFasterButtonTexture->getID()),
                                                             ImVec2(iconSidePixels, iconSidePixels));
        // Ball and garbage protection.
        ImGui::SetCursorPos(ImVec2(0.35f * GUIWidth, row2TopPos));
        m_ballAndGarbageProtectionButtonClicked = ImGui::ImageButton(m_ballAndGarbageProtectionButtonID.c_str(), reinterpret_cast<ImTextureID>(m_ballAndGarbageProtectionButtonTexture->getID()),
                                                                     ImVec2(iconSidePixels, iconSidePixels));
        //Resurrection attempts.
        ImGui::SetCursorPos(ImVec2(0.68f * GUIWidth, row2TopPos));
        m_resurrectionAttemptsButtonClicked = ImGui::ImageButton(m_resurrectionAttemptsButtonID.c_str(), reinterpret_cast<ImTextureID>(m_resurrectionAttemptsButtonTexture->getID()),
                                                                 ImVec2(iconSidePixels, iconSidePixels));

        ImGui::End();
        ImGui::PopStyleColor(1);

        // Menu selected talent.
        float menuSelectedHeight = 0.2f * GUIWidth;
        float text1TopPos = menuSelectedHeight * 0.05f;
        float text2TopPos = menuSelectedHeight * 0.75f;

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{ 0.75f, 0.75f, 0.75f, 1.0f });
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.8f * GUIHeight));
        ImGui::SetNextWindowSize(ImVec2(GUIWidth, menuSelectedHeight));
        ImGui::Begin(m_selectedTalentMenuID.c_str(), nullptr, m_noFrameNoFocus);

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0.0f, 0.0f, 0.0f, 1.0f });
        ImGui::PushFont(m_selectedDescriptionFont);
        ImGui::SetCursorPos(ImVec2(0.01f * GUIWidth, text1TopPos));
        ImGui::Text("%s", m_selectedDescription.c_str());
        ImGui::PopFont();

        ImGui::PushFont(m_selectedValueLevelFont);
        ImGui::SetCursorPos(ImVec2(0.01f * GUIWidth, text2TopPos));
        ImGui::Text("Value:%s  Lvl:%d/%d", m_selectedValue.c_str(), m_selectedCurrentLevel, m_selectedMaxLevel);
        ImGui::PopFont();
        ImGui::PopStyleColor(1);

        float textureAdLeftPos = 0.55f * GUIWidth;
        float textureCrystalsLeftPos = 0.81f * GUIWidth;
        float textureWidth = 0.19f * GUIWidth;
        if(m_selectedCurrentLevel < m_selectedMaxLevel) // Not max level.
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0.0f, 0.5f, 0.0f, 1.0f });
            ImGui::PushFont(m_valueToAddFont);
            ImGui::SetCursorPos(ImVec2(0.44f * GUIWidth, 0.03f * GUIHeight));
            ImGui::Text("%s", m_selectedValueToAdd.c_str());
            ImGui::PopFont();
            ImGui::PopStyleColor(1);

            if(m_selectedCanBeImprovedByAds)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0.0f, 0.4f, 0.0f, 1.0f });
                ImGui::PushFont(m_valueToAddFont);
                ImGui::SetCursorPos(ImVec2(0.75f * GUIWidth, 0.03f * GUIHeight));
                ImGui::Text("or");
                ImGui::PopFont();
                ImGui::PopStyleColor(1);

                if(EnumsAndVars::Ads::rewardedAdTime + EnumsAndVars::Ads::rewardedAdTimeDelay <= Beryll::TimeStep::getSecSinceEpoch())
                {
                    ImGui::SetCursorPos(ImVec2(textureAdLeftPos, 0.0f));
                    ImGui::Image(reinterpret_cast<ImTextureID>(m_improveByAdTexture->getID()),
                                 ImVec2(textureWidth, menuSelectedHeight));
                }
                else
                {
                    ImGui::SetCursorPos(ImVec2(textureAdLeftPos, 0.0f));
                    ImGui::Image(reinterpret_cast<ImTextureID>(m_improveByAdTimerTexture->getID()),
                                 ImVec2(textureWidth, menuSelectedHeight));

                    // Show timer.
                    m_adTimerText = "";
                    uint64_t secLeft = (EnumsAndVars::Ads::rewardedAdTime + EnumsAndVars::Ads::rewardedAdTimeDelay) - Beryll::TimeStep::getSecSinceEpoch();
                    if(secLeft > EnumsAndVars::Ads::rewardedAdTimeDelay)
                        secLeft = EnumsAndVars::Ads::rewardedAdTimeDelay;

                    int min = secLeft / 60;
                    int sec = secLeft % 60;

                    if(min < 10)
                        m_adTimerText += "0";

                    m_adTimerText += std::to_string(min);
                    m_adTimerText += ":";

                    if(sec < 10)
                        m_adTimerText += "0";

                    m_adTimerText += std::to_string(sec);

                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0.0f, 0.0f, 0.0f, 1.0f });
                    ImGui::PushFont(m_adTimerFont);
                    ImGui::SetCursorPos(ImVec2(0.565f * GUIWidth, 0.029f * GUIHeight));
                    ImGui::Text("%s", m_adTimerText.c_str());
                    ImGui::PopFont();
                    ImGui::PopStyleColor(1);
                }
            }

            ImGui::SetCursorPos(ImVec2(textureCrystalsLeftPos, 0.0f));
            ImGui::Image(reinterpret_cast<ImTextureID>(m_improveByCrystalsTexture->getID()),
                         ImVec2(textureWidth, menuSelectedHeight));

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0.0f, 0.0f, 0.0f, 1.0f });
            ImGui::PushFont(m_valueToAddFont);
            ImGui::SetCursorPos(ImVec2(0.875f * GUIWidth, 0.055f * GUIHeight));
            ImGui::Text("%d", m_selectedPriceCrystals);
            ImGui::PopFont();
            ImGui::PopStyleColor(1);
        }
        else
        {
            ImGui::SetCursorPos(ImVec2(textureAdLeftPos, 0.0f));
            ImGui::Image(reinterpret_cast<ImTextureID>(m_maxLevelReachedTexture->getID()),
                         ImVec2(GUIWidth - textureAdLeftPos, menuSelectedHeight));
        }

        ImGui::End();
        ImGui::PopStyleColor(1);

        // Two transparent buttons.
        if(m_selectedCurrentLevel < m_selectedMaxLevel)
        {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f }); // Lost focus.
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f }); // On focus.
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.0f, 0.0f, 0.0f, 0.5f }); // Clicked.
            ImGui::SetNextWindowPos(ImVec2(0.0f, 0.8f * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(GUIWidth, menuSelectedHeight));
            ImGui::Begin(m_improveTalentMenuID.c_str(), nullptr, m_noBackgroundNoFrame);

            if(m_selectedCanBeImprovedByAds &&
               EnumsAndVars::Ads::rewardedAdTime + EnumsAndVars::Ads::rewardedAdTimeDelay <= Beryll::TimeStep::getSecSinceEpoch())
            {
                ImGui::SetCursorPos(ImVec2(textureAdLeftPos, 0.0f));
                m_improveByAdClicked = ImGui::ImageButton(m_improveTalentByAdButtonID.c_str(), reinterpret_cast<ImTextureID>(m_transparentTexture->getID()),
                                                          ImVec2(textureWidth, menuSelectedHeight));
            }

            ImGui::SetCursorPos(ImVec2(textureCrystalsLeftPos, 0.0f));
            m_improveByCrystalsClicked = ImGui::ImageButton(m_improveTalentByCrystalsButtonID.c_str(), reinterpret_cast<ImTextureID>(m_transparentTexture->getID()),
                                                      ImVec2(textureWidth, menuSelectedHeight));

            ImGui::End();
            ImGui::PopStyleColor(3);
        }

        // Not enough crystals menu.
        if(m_showNoCrystalsMenu)
        {
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{ 0.0f, 0.0f, 0.0f, 0.92f });
            ImGui::SetNextWindowFocus();
            ImGui::SetNextWindowPos(ImVec2(0.0f * GUIWidth, -0.01f * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(1.0f * GUIWidth, 1.02f * GUIHeight));
            ImGui::Begin(m_noCrystalsMenuID.c_str(), nullptr, m_noFrame);

            ImGui::SetCursorPos(ImVec2(0.2f * GUIWidth, 0.25f * GUIHeight));
            ImGui::Image(reinterpret_cast<ImTextureID>(m_noCrystalsTexture->getID()),
                         ImVec2(0.6f * GUIWidth, 0.25f * GUIHeight));

            ImGui::SetCursorPos(ImVec2(0.35f * GUIWidth, 0.505f * GUIHeight));
            m_noCrystalsButtonOkClicked = ImGui::ImageButton(m_noCrystalsButtonOkID.c_str(),reinterpret_cast<ImTextureID>(m_noCrystalsButtonOkTexture->getID()),
                                                             ImVec2(0.3f * GUIWidth, 0.07f * GUIHeight));
            ImGui::End();
            ImGui::PopStyleColor(1);
        }

        // Ad loading.
        if(m_adLoadingMenuShow)
        {
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{ 0.0f, 0.0f, 0.0f, 0.92f });
            ImGui::SetNextWindowFocus();
            ImGui::SetNextWindowPos(ImVec2(0.0f * GUIWidth, -0.01f * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(1.0f * GUIWidth, 1.02f * GUIHeight));
            ImGui::Begin(m_adLoadingMenuID.c_str(), nullptr, m_noFrame);

            ImGui::SetCursorPos(ImVec2(0.2f * GUIWidth, 0.25f * GUIHeight));
            ImGui::Image(reinterpret_cast<ImTextureID>(m_adLoadingTexture->getID()),
                         ImVec2(0.6f * GUIWidth, 0.25f * GUIHeight));

            ImGui::End();
            ImGui::PopStyleColor(1);
        }

        // Ad error.
        if(m_adErrorMenuShow)
        {
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{ 0.0f, 0.0f, 0.0f, 0.92f });
            ImGui::SetNextWindowFocus();
            ImGui::SetNextWindowPos(ImVec2(0.0f * GUIWidth, -0.01f * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(1.0f * GUIWidth, 1.02f * GUIHeight));
            ImGui::Begin(m_adErrorMenuID.c_str(), nullptr, m_noFrame);

            ImGui::SetCursorPos(ImVec2(0.2f * GUIWidth, 0.25f * GUIHeight));
            ImGui::Image(reinterpret_cast<ImTextureID>(m_adErrorTexture->getID()),
                         ImVec2(0.6f * GUIWidth, 0.25f * GUIHeight));

            ImGui::SetCursorPos(ImVec2(0.35f * GUIWidth, 0.505f * GUIHeight));
            m_adErrorButtonOkClicked = ImGui::ImageButton(m_adErrorButtonOkID.c_str(),reinterpret_cast<ImTextureID>(m_adErrorButtonOkTexture->getID()),
                                                          ImVec2(0.3f * GUIWidth, 0.07f * GUIHeight));
            ImGui::End();
            ImGui::PopStyleColor(1);
        }
    }

    void PlayerTalentsGUILayer::selectTalent(int index)
    {
        BR_ASSERT((index < EnumsAndVars::allPlayerTalents.size()), "%s", "selectTalent(int index) index wrong.");
        m_selectedIndex = index;

        float currentValue = 0.0f;
        if(m_selectedIndex == 0) // Max speed. = 0
        {
            m_selectedTalentTexture = m_maxSpeedButtonTexture;
            currentValue = EnumsAndVars::playerMaxSpeedXZDefault * (1.0f + EnumsAndVars::allPlayerTalents[m_selectedIndex].getPercentsToImprove() / 100.0f);
        }
        else if(m_selectedIndex == 1) // Magnetic radius. = 1
        {
            m_selectedTalentTexture = m_magneticRadiusButtonTexture;
            currentValue = EnumsAndVars::playerMagneticRadiusDefault * (1.0f + EnumsAndVars::allPlayerTalents[m_selectedIndex].getPercentsToImprove() / 100.0f);
        }
        else if(m_selectedIndex == 2) // Amount of magnetized garbage. = 2
        {
            m_selectedTalentTexture = m_amountOfMagnetizedGarbageButtonTexture;
            currentValue = EnumsAndVars::garbageMaxCountMagnetizedDefault * (1.0f + EnumsAndVars::allPlayerTalents[m_selectedIndex].getPercentsToImprove() / 100.0f);
        }
        else if(m_selectedIndex == 3) // Accelerate faster. = 3
        {
            m_selectedTalentTexture = m_accelerateFasterButtonTexture;
            currentValue = EnumsAndVars::playerImpulseFactorOnGroundDefault * (1.0f + EnumsAndVars::allPlayerTalents[m_selectedIndex].getPercentsToImprove() / 100.0f);

            currentValue *= 100.0f; // Because original value too small. Show it for user in range 10...15.
        }
        else if(m_selectedIndex == 4) // Ball and garbage protection. = 4
        {
            m_selectedTalentTexture = m_ballAndGarbageProtectionButtonTexture;
            currentValue = EnumsAndVars::garbageStartHPDefault * (1.0f + EnumsAndVars::allPlayerTalents[m_selectedIndex].getPercentsToImprove() / 100.0f);
        }
        else if(m_selectedIndex == 5) // Resurrection attempts. = 5
        {
            m_selectedTalentTexture = m_resurrectionAttemptsButtonTexture;
            currentValue = EnumsAndVars::playerResurrectionAttemptsDefault * int(1.0f + std::roundf(EnumsAndVars::allPlayerTalents[m_selectedIndex].getPercentsToImprove() / 100.0f));
        }
        else
        {
            BR_ASSERT(false, "%s", "Unknown m_selectedIndex.");
        }

        m_selectedDescription = EnumsAndVars::allPlayerTalents[m_selectedIndex].description;
        m_selectedValueToAdd = EnumsAndVars::allPlayerTalents[m_selectedIndex].increasePerLevelText;
        m_selectedPriceCrystals = EnumsAndVars::allPlayerTalents[m_selectedIndex].priceCrystals;
        m_selectedCanBeImprovedByAds = EnumsAndVars::allPlayerTalents[m_selectedIndex].canBeImprovedByAd;

        std::stringstream stream;
        stream << std::fixed << std::setprecision(1) << currentValue;
        m_selectedValue = stream.str();

        m_selectedCurrentLevel = EnumsAndVars::allPlayerTalents[m_selectedIndex].currentLevel;
        m_selectedMaxLevel = EnumsAndVars::allPlayerTalents[m_selectedIndex].maxLevel;
    }
}
