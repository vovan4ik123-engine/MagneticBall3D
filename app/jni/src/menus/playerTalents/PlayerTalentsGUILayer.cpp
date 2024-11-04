#include "PlayerTalentsGUILayer.h"
#include "EnumsAndVariables.h"
#include "GameStateHelper.h"
#include "DataBaseHelper.h"
#include "SendStatisticsHelper.h"

namespace MagneticBall3D
{
    int PlayerTalentsGUILayer::m_selectedIndex = 0;
    std::atomic<bool> PlayerTalentsGUILayer::m_adSuccess = false;
    std::atomic<bool> PlayerTalentsGUILayer::m_adError = false;

    PlayerTalentsGUILayer::PlayerTalentsGUILayer()
    {
        //float m_screenAR = Beryll::MainImGUI::getInstance()->getGUIScreenAspectRation();
        //BR_ASSERT((m_screenAR > 0.0f), "%s", "m_screenAR = 0");

        m_transparentTexture = Beryll::Renderer::createTexture("GUI/FullTransparent.png", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_backButtonTexture = Beryll::Renderer::createTexture("GUI/menus/LeftArrow.png", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);

        m_backgroundTexture = Beryll::Renderer::createTexture("GUI/menus/playerTalents/TalentsBackground.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_selectedTalentBackground = Beryll::Renderer::createTexture("GUI/menus/playerTalents/SelectedTalentBackground.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
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
        m_noCrystalsButtonOkTexture = Beryll::Renderer::createTexture("GUI/Ok.png", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_adLoadingTexture = Beryll::Renderer::createTexture("GUI/AdLoading.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_adErrorTexture = Beryll::Renderer::createTexture("GUI/SomethingWentWrong.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_adErrorButtonOkTexture = Beryll::Renderer::createTexture("GUI/Ok.png", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);

        m_selectedDescriptionFont = Beryll::MainImGUI::getInstance()->createFont(EnumsAndVars::FontsPath::roboto, 0.055f);
        m_selectedValueLevelFont = Beryll::MainImGUI::getInstance()->createFont(EnumsAndVars::FontsPath::roboto, 0.045f);
        m_valueToAddFont = Beryll::MainImGUI::getInstance()->createFont(EnumsAndVars::FontsPath::roboto, 0.07f);
        m_adTimerFont = Beryll::MainImGUI::getInstance()->createFont(EnumsAndVars::FontsPath::roboto, 0.08f);

        selectTalent(m_selectedIndex);

        // These callbacks are called from different thread.
        m_adSuccessCallback = []() -> void { BR_INFO("%s", "m_resurrectAdSuccessCallback()"); PlayerTalentsGUILayer::m_adSuccess = true; };
        m_adErrorCallback = []() -> void { BR_INFO("%s", "m_adErrorCallback()"); PlayerTalentsGUILayer::m_adError = true; };
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
            SendStatisticsHelper::sendCustomMessage("attempt_show_ad");
            Beryll::Ads::getInstance()->showInterstitialAd(m_adSuccessCallback, m_adErrorCallback);
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
                m_noCrystalsMenuShow = true;
            }
        }

        if(m_noCrystalsButtonOkClicked)
        {
            m_noCrystalsButtonOkClicked = false;
            m_noCrystalsMenuShow = false;
        }

        if(m_adErrorButtonOkClicked)
        {
            m_adErrorButtonOkClicked = false;
            m_adErrorMenuShow = false;
        }

        if(PlayerTalentsGUILayer::m_adSuccess)
        {
            PlayerTalentsGUILayer::m_adSuccess = false;
            m_adLoadingMenuShow = false;
            EnumsAndVars::allPlayerTalents[m_selectedIndex].improveLevel(EnumsAndVars::PlayerTalentCurrency::AD);
            selectTalent(m_selectedIndex); // Recalculate values.
            SendStatisticsHelper::sendTalentImproved(EnumsAndVars::allPlayerTalents[m_selectedIndex].name, "ad");

            EnumsAndVars::Ads::rewardedAdTime = Beryll::TimeStep::getSecSinceEpoch();
            DataBaseHelper::storeAdsRewardedAdTime(EnumsAndVars::Ads::rewardedAdTime);
        }

        if(PlayerTalentsGUILayer::m_adError)
        {
            PlayerTalentsGUILayer::m_adError = false;
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

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}); // Lost focus.
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}); // On focus.
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}); // Clicked.
        ImGui::SetNextWindowPos(ImVec2(-0.005f * GUIWidth, -0.005f * GUIHeight));
        ImGui::SetNextWindowSize(ImVec2(1.01f * GUIWidth, 1.01f * GUIHeight));
        ImGui::Begin("talentsMenu", nullptr, m_noBackgroundNoFrameNoFocus);

        // Background.
        ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
        ImGui::Image(reinterpret_cast<ImTextureID>(m_backgroundTexture->getID()),
                     ImVec2(1.01f * GUIWidth, 1.01f * GUIHeight));

        // Back.
        ImGui::SetCursorPos(ImVec2(0.005f * GUIWidth, 0.855f * GUIHeight));
        m_backButtonClicked = ImGui::ImageButton("backButton", reinterpret_cast<ImTextureID>(m_backButtonTexture->getID()),
                                                 ImVec2(0.15f * GUIWidth, 0.15f * GUIHeight));

        // Max speed.
        ImGui::SetCursorPos(ImVec2(0.028f * GUIWidth, 0.245f * GUIHeight));
        m_maxSpeedButtonClicked = ImGui::ImageButton("maxSpeedButton", reinterpret_cast<ImTextureID>(m_maxSpeedButtonTexture->getID()),
                                                     ImVec2(0.14f * GUIWidth, 0.305f * GUIHeight));
        // Magnetic radius.
        ImGui::SetCursorPos(ImVec2(0.191f * GUIWidth, 0.245f * GUIHeight));
        m_magneticRadiusButtonClicked = ImGui::ImageButton("magneticRadiusButton", reinterpret_cast<ImTextureID>(m_magneticRadiusButtonTexture->getID()),
                                                           ImVec2(0.14f * GUIWidth, 0.305f * GUIHeight));
        // Amount of magnetized garbage.
        ImGui::SetCursorPos(ImVec2(0.354f * GUIWidth, 0.245f * GUIHeight));
        m_amountOfMagnetizedGarbageButtonClicked = ImGui::ImageButton("amountOfMagnetizedGarbageButton", reinterpret_cast<ImTextureID>(m_amountOfMagnetizedGarbageButtonTexture->getID()),
                                                                      ImVec2(0.14f * GUIWidth, 0.305f * GUIHeight));
        // Accelerate faster.
        ImGui::SetCursorPos(ImVec2(0.517f * GUIWidth, 0.245f * GUIHeight));
        m_accelerateFasterButtonClicked = ImGui::ImageButton("accelerateFasterButton", reinterpret_cast<ImTextureID>(m_accelerateFasterButtonTexture->getID()),
                                                             ImVec2(0.14f * GUIWidth, 0.305f * GUIHeight));
        // Ball and garbage protection.
        ImGui::SetCursorPos(ImVec2(0.68f * GUIWidth, 0.245f * GUIHeight));
        m_ballAndGarbageProtectionButtonClicked = ImGui::ImageButton("ballAndGarbageProtectionButton", reinterpret_cast<ImTextureID>(m_ballAndGarbageProtectionButtonTexture->getID()),
                                                                     ImVec2(0.14f * GUIWidth, 0.305f * GUIHeight));
        //Resurrection attempts.
        ImGui::SetCursorPos(ImVec2(0.843f * GUIWidth, 0.245f * GUIHeight));
        m_resurrectionAttemptsButtonClicked = ImGui::ImageButton("resurrectionAttemptsButton", reinterpret_cast<ImTextureID>(m_resurrectionAttemptsButtonTexture->getID()),
                                                                 ImVec2(0.14f * GUIWidth, 0.305f * GUIHeight));

        // Menu selected talent.
        ImGui::SetCursorPos(ImVec2(0.245f * GUIWidth, 0.655f * GUIHeight));
        ImGui::Image(reinterpret_cast<ImTextureID>(m_selectedTalentBackground->getID()),
                     ImVec2(0.52f * GUIWidth, 0.2f * GUIHeight));

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0.9726f, 0.996f, 0.996f, 1.0f});
        ImGui::PushFont(m_selectedDescriptionFont);
        ImGui::SetCursorPos(ImVec2(0.255f * GUIWidth, 0.67f * GUIHeight));
        ImGui::Text("%s", m_selectedDescription.c_str());
        ImGui::PopFont();

        ImGui::PushFont(m_selectedValueLevelFont);
        ImGui::SetCursorPos(ImVec2(0.255f * GUIWidth, 0.8f * GUIHeight));
        ImGui::Text("Value:%s  Level:%d/%d", m_selectedValue.c_str(), m_selectedCurrentLevel, m_selectedMaxLevel);
        ImGui::PopFont();
        ImGui::PopStyleColor(4);

        float improveIconWidthPixels = 0.092f * GUIWidth;
        float improveIconHeightPixels = 0.2f * GUIHeight;
        if(m_selectedCurrentLevel < m_selectedMaxLevel) // Not max level.
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0.9726f, 0.996f, 0.996f, 1.0f});
            ImGui::PushFont(m_valueToAddFont);
            ImGui::SetCursorPos(ImVec2(0.475f * GUIWidth, 0.72f * GUIHeight));
            ImGui::Text("%s", m_selectedValueToAdd.c_str());
            ImGui::PopFont();
            ImGui::PopStyleColor(1);

            if(m_selectedCanBeImprovedByAds)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0.9726f, 0.996f, 0.996f, 1.0f});
                ImGui::PushFont(m_valueToAddFont);
                ImGui::SetCursorPos(ImVec2(0.637f * GUIWidth, 0.715f * GUIHeight));
                ImGui::Text("or");
                ImGui::PopFont();
                ImGui::PopStyleColor(1);

                if(EnumsAndVars::Ads::rewardedAdTime + EnumsAndVars::Ads::rewardedAdTimeDelay <= Beryll::TimeStep::getSecSinceEpoch())
                {
                    ImGui::SetCursorPos(ImVec2(0.535f * GUIWidth, 0.655f * GUIHeight));
                    ImGui::Image(reinterpret_cast<ImTextureID>(m_improveByAdTexture->getID()),
                                 ImVec2(improveIconWidthPixels, improveIconHeightPixels));
                }
                else
                {
                    ImGui::SetCursorPos(ImVec2(0.535f * GUIWidth, 0.655f * GUIHeight));
                    ImGui::Image(reinterpret_cast<ImTextureID>(m_improveByAdTimerTexture->getID()),
                                 ImVec2(improveIconWidthPixels, improveIconHeightPixels));

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

                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0.9726f, 0.996f, 0.996f, 1.0f});
                    ImGui::PushFont(m_adTimerFont);
                    ImGui::SetCursorPos(ImVec2(0.542f * GUIWidth, 0.715f * GUIHeight));
                    ImGui::Text("%s", m_adTimerText.c_str());
                    ImGui::PopFont();
                    ImGui::PopStyleColor(1);
                }
            }

            ImGui::SetCursorPos(ImVec2(0.675f * GUIWidth, 0.655f * GUIHeight));
            ImGui::Image(reinterpret_cast<ImTextureID>(m_improveByCrystalsTexture->getID()),
                         ImVec2(improveIconWidthPixels, improveIconHeightPixels));

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0.0742f, 0.0742f, 0.0742f, 1.0f});
            ImGui::PushFont(m_valueToAddFont);
            ImGui::SetCursorPos(ImVec2(0.705f * GUIWidth, 0.77f * GUIHeight));
            ImGui::Text("%d", m_selectedPriceCrystals);
            ImGui::PopFont();
            ImGui::PopStyleColor(1);
        }
        else
        {
            ImGui::SetCursorPos(ImVec2(0.535f * GUIWidth, 0.655f * GUIHeight));
            ImGui::Image(reinterpret_cast<ImTextureID>(m_maxLevelReachedTexture->getID()),
                         ImVec2(0.232f * GUIWidth, improveIconHeightPixels));
        }

        // Two transparent buttons.
        if(m_selectedCurrentLevel < m_selectedMaxLevel)
        {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}); // Lost focus.
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}); // On focus.
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.0f, 0.0f, 0.0f, 0.5f}); // Clicked.

            if(m_selectedCanBeImprovedByAds &&
               EnumsAndVars::Ads::rewardedAdTime + EnumsAndVars::Ads::rewardedAdTimeDelay <= Beryll::TimeStep::getSecSinceEpoch())
            {
                ImGui::SetCursorPos(ImVec2(0.535f * GUIWidth, 0.655f * GUIHeight));
                m_improveByAdClicked = ImGui::ImageButton("improveTalentByAdButton", reinterpret_cast<ImTextureID>(m_transparentTexture->getID()),
                                                          ImVec2(improveIconWidthPixels, improveIconHeightPixels));
            }

            ImGui::SetCursorPos(ImVec2(0.675f * GUIWidth, 0.655f * GUIHeight));
            m_improveByCrystalsClicked = ImGui::ImageButton("improveTalentByCrystalsButton", reinterpret_cast<ImTextureID>(m_transparentTexture->getID()),
                                                      ImVec2(improveIconWidthPixels, improveIconHeightPixels));

            ImGui::PopStyleColor(3);
        }

        ImGui::End();

        // Not enough crystals menu.
        if(m_noCrystalsMenuShow)
        {
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{0.0f, 0.0f, 0.0f, 0.92f});
            ImGui::SetNextWindowPos(ImVec2(-0.005f * GUIWidth, -0.005f * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(1.01f * GUIWidth, 1.01f * GUIHeight));
            ImGui::Begin("noCrystalsMenu", nullptr, m_noFrame);

            ImGui::SetCursorPos(ImVec2(0.355f * GUIWidth, 0.155f * GUIHeight));
            ImGui::Image(reinterpret_cast<ImTextureID>(m_noCrystalsTexture->getID()),
                         ImVec2(0.3f * GUIWidth, 0.5f * GUIHeight));

            ImGui::SetCursorPos(ImVec2(0.435f * GUIWidth, 0.705f * GUIHeight));
            m_noCrystalsButtonOkClicked = ImGui::ImageButton("noCrystalsButtonOk",reinterpret_cast<ImTextureID>(m_noCrystalsButtonOkTexture->getID()),
                                                             ImVec2(0.14f * GUIWidth, 0.1528f * GUIHeight));
            ImGui::End();
            ImGui::PopStyleColor(1);
        }

        // Ad loading.
        if(m_adLoadingMenuShow)
        {
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{0.0f, 0.0f, 0.0f, 0.92f});
            ImGui::SetNextWindowPos(ImVec2(-0.005f * GUIWidth, -0.005f * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(1.01f * GUIWidth, 1.01f * GUIHeight));
            ImGui::Begin("adLoadingMenu", nullptr, m_noFrame);

            ImGui::SetCursorPos(ImVec2(0.355f * GUIWidth, 0.155f * GUIHeight));
            ImGui::Image(reinterpret_cast<ImTextureID>(m_adLoadingTexture->getID()),
                         ImVec2(0.3f * GUIWidth, 0.5f * GUIHeight));

            ImGui::End();
            ImGui::PopStyleColor(1);
        }

        // Ad error.
        if(m_adErrorMenuShow)
        {
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{0.0f, 0.0f, 0.0f, 0.92f});
            ImGui::SetNextWindowPos(ImVec2(-0.005f * GUIWidth, -0.005f * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(1.01f * GUIWidth, 1.01f * GUIHeight));
            ImGui::Begin("adErrorMenu", nullptr, m_noFrame);

            ImGui::SetCursorPos(ImVec2(0.355f * GUIWidth, 0.155f * GUIHeight));
            ImGui::Image(reinterpret_cast<ImTextureID>(m_adErrorTexture->getID()),
                         ImVec2(0.3f * GUIWidth, 0.5f * GUIHeight));

            ImGui::SetCursorPos(ImVec2(0.435f * GUIWidth, 0.705f * GUIHeight));
            m_adErrorButtonOkClicked = ImGui::ImageButton("adErrorButtonOk",reinterpret_cast<ImTextureID>(m_adErrorButtonOkTexture->getID()),
                                                          ImVec2(0.14f * GUIWidth, 0.1528f * GUIHeight));
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

            currentValue *= 10.0f; // Because original value too small. Show it for user in range 10...15.
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
