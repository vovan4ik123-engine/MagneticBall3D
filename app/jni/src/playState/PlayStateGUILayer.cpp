#include "PlayStateGUILayer.h"
#include "EnumsAndVariables.h"
#include "GameStateHelper.h"
#include "DataBaseHelper.h"
#include "SendStatisticsHelper.h"
#include "Sounds.h"

namespace MagneticBall3D
{
    std::atomic<bool> PlayStateGUILayer::m_resurrectAdSuccess = false;
    std::atomic<bool> PlayStateGUILayer::m_winPrize1AdSuccess = false;
    std::atomic<bool> PlayStateGUILayer::m_winPrize2AdSuccess = false;
    std::atomic<bool> PlayStateGUILayer::m_exitAdSuccess = false;
    std::atomic<bool> PlayStateGUILayer::m_killAllAdSuccess = false;
    std::atomic<bool> PlayStateGUILayer::m_commonAdError = false;

    PlayStateGUILayer::PlayStateGUILayer()
    {
        m_ID = Beryll::LayerID::PLAY_GUI;

        const float screenAR = Beryll::MainImGUI::getInstance()->getGUIScreenAspectRation();

        if(m_statisticsShow)
        {
            m_statistics1 = std::make_shared<Beryll::Text>("Frame: 00000  FPS: 00000", EnumsAndVars::FontsPath::roboto, 0.035f, 0.06f, 0, 0.5f, 0.045f);
            m_guiObjects.push_back(m_statistics1);
            m_statistics2 = std::make_shared<Beryll::Text>("Phys: 00000  Logic: 00000  GPU: 00000", EnumsAndVars::FontsPath::roboto, 0.035f, 0.06f, 0.04f, 0.5f, 0.045f);
            m_guiObjects.push_back(m_statistics2);
            m_statistics3 = std::make_shared<Beryll::Text>("Time: 00000", EnumsAndVars::FontsPath::roboto, 0.03f, 0.06f, 0.08f, 0.5f, 0.04f);
            m_guiObjects.push_back(m_statistics3);
        }

//        slider1 = std::make_shared<Beryll::SliderHorizontal>("velos", EnumsAndVars::FontsPath::roboto, 0.04f, 0.0f, 0.07f, 0.2f, 0.04f, 1, 2);
//        m_guiObjects.push_back(slider1);
//        slider1->setValue(1.5f);

//        slider2 = std::make_shared<Beryll::SliderHorizontal>("Y offset", EnumsAndVars::FontsPath::roboto, 0.04f, 0.0f, 0.13f, 0.2f, 0.04f, 1, 10);
//        m_guiObjects.push_back(slider2);
//        slider2->setValue(5.0f);

//        slider3 = std::make_shared<Beryll::SliderHorizontal>("FOV", EnumsAndVars::FontsPath::roboto, 0.04f, 0.0f, 0.19f, 0.2f, 0.04f, 30, 110);
//        m_guiObjects.push_back(slider3);
//        slider3->setValue(45);

        playerJoystick = std::make_shared<Beryll::Joystick>("GUI/playState/Joystick.png","",
                                                            0.0f, 0.25f, 1.2f / screenAR, 1.2f);
        m_guiObjects.push_back(playerJoystick);
        playerJoystick->disable();

        m_shotButton = std::make_shared<Beryll::ButtonWithTexture>("GUI/playState/ShotButton.png", "",
                                                                   0.8f, 0.33f, 0.15f / screenAR, 0.15f);

        m_countersFont = Beryll::MainImGUI::getInstance()->createFont(EnumsAndVars::FontsPath::roboto, 0.032f);

        // Common textures.
        m_okTexture = Beryll::Renderer::createTexture("GUI/Ok.png", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_exitTexture = Beryll::Renderer::createTexture("GUI/Exit.png", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);

        m_countersTexture = Beryll::Renderer::createTexture("GUI/playState/Counters.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_pauseButtonTexture = Beryll::Renderer::createTexture("GUI/playState/Pause.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_resumeButtonTexture = Beryll::Renderer::createTexture("GUI/playState/Resume.png", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_tutorialMoveTexture = Beryll::Renderer::createTexture("GUI/playState/TutorialMove.png", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_tutorialCameraTexture = Beryll::Renderer::createTexture("GUI/playState/TutorialCamera.png", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);

        m_resurrectTexture = Beryll::Renderer::createTexture("GUI/playState/CanResurrect.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_resurrectByCrystalsButtonTexture = Beryll::Renderer::createTexture("GUI/playState/ResurrectByCrystals.png", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_resurrectByAdButtonTexture = Beryll::Renderer::createTexture("GUI/playState/ResurrectByAd.png", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);

        m_noCrystalsTexture = Beryll::Renderer::createTexture("GUI/NotEnoughCrystals.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_adLoadingTexture = Beryll::Renderer::createTexture("GUI/AdLoading.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_adErrorTexture = Beryll::Renderer::createTexture("GUI/SomethingWentWrong.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);

        m_loseTexture = Beryll::Renderer::createTexture("GUI/playState/Lose.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_killAllToSpawnBossTexture = Beryll::Renderer::createTexture("GUI/playState/KillAllEnemiesToSpawnBoss.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_killAllToWinTexture = Beryll::Renderer::createTexture("GUI/playState/KillAllEnemiesToWin.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_winTexture = Beryll::Renderer::createTexture("GUI/playState/Win.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_winPrize1ButtonTexture = Beryll::Renderer::createTexture("GUI/playState/WinPrize1.png", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_winPrize2ButtonTexture = Beryll::Renderer::createTexture("GUI/playState/WinPrize2.png", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_tankWithCommanderTexture = Beryll::Renderer::createTexture("GUI/playState/BossTankWithCommander.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);

        PlayStateGUILayer::m_resurrectAdSuccess = false;
        PlayStateGUILayer::m_winPrize1AdSuccess = false;
        PlayStateGUILayer::m_winPrize2AdSuccess = false;
        PlayStateGUILayer::m_exitAdSuccess = false;
        PlayStateGUILayer::m_killAllAdSuccess = false;
        PlayStateGUILayer::m_commonAdError = false;

        // These callbacks are called from different thread.
        m_resurrectAdSuccessCallback = []() -> void { BR_INFO("%s", "m_resurrectAdSuccessCallback()"); PlayStateGUILayer::m_resurrectAdSuccess = true; };
        m_winPrize1AdSuccessCallback = []() -> void { BR_INFO("%s", "m_winPrize1AdSuccessCallback()"); PlayStateGUILayer::m_winPrize1AdSuccess = true; };
        m_winPrize2AdSuccessCallback = []() -> void { BR_INFO("%s", "m_winPrize2AdSuccessCallback()"); PlayStateGUILayer::m_winPrize2AdSuccess = true; };
        m_exitAdSuccessCallback = []() -> void { BR_INFO("%s", "m_exitAdSuccessCallback()"); PlayStateGUILayer::m_exitAdSuccess = true; };
        m_killAllAdSuccessCallback = []() -> void { BR_INFO("%s", "m_killAllAdSuccessCallback()"); PlayStateGUILayer::m_killAllAdSuccess = true; };
        m_commonAdErrorCallback = []() -> void { BR_INFO("%s", "m_commonAdErrorCallback()"); PlayStateGUILayer::m_commonAdError = true; };
    }

    PlayStateGUILayer::~PlayStateGUILayer()
    {

    }

    void PlayStateGUILayer::updateBeforePhysics()
    {
        if(EnumsAndVars::improvementSystemOnScreen)
            return;

        for(const std::shared_ptr<Beryll::GUIObject>& go : m_guiObjects)
        {
            if(go->getIsEnabled())
            {
                go->updateBeforePhysics();
            }
        }
        if(EnumsAndVars::SettingsMenu::interfaceGUI && EnumsAndVars::MapsProgress::currentMapIndex > 0)
        {
            m_shotButton->updateBeforePhysics();
        }

        if(m_statisticsShow && Beryll::TimeStep::getMilliSecFromStart() > m_statisticsUpdateTime + 200) // Update every 200 ms.
        {
            std::stringstream stream;
            stream << std::fixed << std::setprecision(1);
            stream << "Frame: " << Beryll::GameLoop::getFrameTime() << "  FPS: " << Beryll::GameLoop::getFPS();
            m_statistics1->text = stream.str();

            stream.str(""); // Way to clear std::stringstream.
            stream << std::fixed << std::setprecision(1);
            stream << "Phys: " << Beryll::Physics::getSimulationTime();
            stream << "  Logic: " << (Beryll::GameLoop::getCPUTime() - Beryll::Physics::getSimulationTime());
            stream << "  GPU: " << Beryll::GameLoop::getGPUTime();
            m_statistics2->text = stream.str();

            stream.str("");
            stream << "Time: " << int(EnumsAndVars::mapPlayTimeSec / 60.0f) << ":" << int(std::fmod(EnumsAndVars::mapPlayTimeSec, 60.0f));
            m_statistics3->text = stream.str();

            m_statisticsUpdateTime = Beryll::TimeStep::getMilliSecFromStart();
        }

        if(m_pauseButtonClicked)
        {
            m_pauseButtonClicked = false;

            if(!EnumsAndVars::gameOnPause)
            {
                m_pauseMenuShow = true;

                Sounds::pauseBackgroundMusic();
                GameStateHelper::pauseGame();
            }
        }
        else if(m_resumeButtonClicked)
        {
            m_resumeButtonClicked = false;
            m_pauseMenuShow = false;

            Sounds::resumeBackgroundMusic();
            GameStateHelper::resumeGame();
        }
        else if(m_exitButtonClicked)
        {
            m_exitButtonClicked = false;
            Sounds::stopBackgroundMusic();

            if(EnumsAndVars::Shop::adsOnMapsDisabled)
            {
                m_exitAdSuccessCallback();
            }
            else
            {
                m_adLoadingMenuShow = true;
                SendStatisticsHelper::sendCustomMessage("attempt_show_ad");
                Beryll::Ads::getInstance()->showInterstitialAd(m_exitAdSuccessCallback, m_commonAdErrorCallback);
            }
        }
        else if(m_killAllButtonClicked)
        {
            m_killAllButtonClicked = false;
            Sounds::pauseBackgroundMusic();
            SendStatisticsHelper::sendCustomMessage("kill_all_map_" + std::to_string(EnumsAndVars::MapsProgress::currentMapIndex));

            if(EnumsAndVars::Shop::adsOnMapsDisabled)
            {
                m_killAllAdSuccessCallback();
            }
            else
            {
                m_adLoadingMenuShow = true;
                SendStatisticsHelper::sendCustomMessage("attempt_show_ad");
                Beryll::Ads::getInstance()->showInterstitialAd(m_killAllAdSuccessCallback, m_commonAdErrorCallback);
            }
        }
        else if(m_winPrize1ButtonClicked)
        {
            BR_INFO("%s", "m_winPrize1ButtonClicked");
            m_winPrize1ButtonClicked = false;

            if(EnumsAndVars::Shop::adsOnMapsDisabled)
            {
                m_winPrize1AdSuccessCallback();
            }
            else
            {
                m_adLoadingMenuShow = true;
                SendStatisticsHelper::sendCustomMessage("attempt_show_ad");
                Beryll::Ads::getInstance()->showInterstitialAd(m_winPrize1AdSuccessCallback, m_commonAdErrorCallback);
            }
        }
        else if(m_winPrize2ButtonClicked)
        {
            BR_INFO("%s", "m_winPrize2ButtonClicked");
            m_winPrize2ButtonClicked = false;

            if(EnumsAndVars::Shop::adsOnMapsDisabled)
            {
                m_winPrize2AdSuccessCallback();
            }
            else
            {
                m_adLoadingMenuShow = true;
                SendStatisticsHelper::sendCustomMessage("attempt_show_ad");
                Beryll::Ads::getInstance()->showRewardedAd(m_winPrize2AdSuccessCallback, m_commonAdErrorCallback, true);
            }
        }
        else if(m_tankWithCommanderButtonClicked)
        {
            m_tankWithCommanderButtonClicked = false;
            m_tankWithCommanderMenuShow = false;

            GameStateHelper::resumeGame();
        }
        else if(m_resurrectByAdButtonClicked)
        {
            BR_INFO("%s", "m_resurrectByAdButtonClicked");
            m_resurrectByAdButtonClicked = false;
            Sounds::pauseBackgroundMusic();

            if(EnumsAndVars::Shop::adsOnMapsDisabled)
            {
                m_resurrectAdSuccessCallback();
            }
            else
            {
                m_adLoadingMenuShow = true;
                SendStatisticsHelper::sendCustomMessage("attempt_show_ad");
                Beryll::Ads::getInstance()->showRewardedAd(m_resurrectAdSuccessCallback, m_commonAdErrorCallback, true);
            }
        }
        else if(m_resurrectByCrystalsButtonClicked)
        {
            BR_INFO("%s", "m_resurrectByCrystalsButtonClicked");
            m_resurrectByCrystalsButtonClicked = false;

            if(EnumsAndVars::CurrencyBalance::crystals >= EnumsAndVars::playerCostOfResurrectionCrystals)
            {
                m_resurrectMenuShow = false;

                EnumsAndVars::CurrencyBalance::crystals -= EnumsAndVars::playerCostOfResurrectionCrystals;
                DataBaseHelper::storeCurrencyBalanceCrystals(EnumsAndVars::CurrencyBalance::crystals);
                resurrectPlayer = true; // Will handled in BaseMap.cpp
                GameStateHelper::resumeGame();
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

        if(PlayStateGUILayer::m_resurrectAdSuccess)
        {
            PlayStateGUILayer::m_resurrectAdSuccess = false;
            m_adLoadingMenuShow = false;
            m_resurrectMenuShow = false;

            resurrectPlayer = true; // Will handled in BaseMap.cpp
            GameStateHelper::resumeGame();
            Sounds::resumeBackgroundMusic();
        }

        if(PlayStateGUILayer::m_winPrize1AdSuccess)
        {
            PlayStateGUILayer::m_winPrize1AdSuccess = false;
            m_adLoadingMenuShow = false;
            m_winMenuShow = false;

            EnumsAndVars::CurrencyBalance::crystals += 25;
            DataBaseHelper::storeCurrencyBalanceCrystals(EnumsAndVars::CurrencyBalance::crystals);

            GameStateHelper::popState();
            GameStateHelper::pushStartMenuState();
            return;
        }

        if(PlayStateGUILayer::m_winPrize2AdSuccess)
        {
            PlayStateGUILayer::m_winPrize2AdSuccess = false;
            m_adLoadingMenuShow = false;
            m_winMenuShow = false;

            EnumsAndVars::CurrencyBalance::crystals += 50;
            DataBaseHelper::storeCurrencyBalanceCrystals(EnumsAndVars::CurrencyBalance::crystals);

            GameStateHelper::popState();
            GameStateHelper::pushStartMenuState();
            return;
        }

        if(PlayStateGUILayer::m_exitAdSuccess)
        {
            PlayStateGUILayer::m_exitAdSuccess = false;

            GameStateHelper::popState();
            GameStateHelper::pushStartMenuState();
            return;
        }

        if(PlayStateGUILayer::m_killAllAdSuccess)
        {
            PlayStateGUILayer::m_killAllAdSuccess = false;
            m_killAllMenuShow = false;
            m_adLoadingMenuShow = false;
            Sounds::resumeBackgroundMusic();
            GameStateHelper::resumeGame();
        }

        if(PlayStateGUILayer::m_commonAdError)
        {
            PlayStateGUILayer::m_commonAdError = false;
            m_adLoadingMenuShow = false;
            m_adErrorMenuShow = true;
        }

        if(m_adErrorButtonOkClicked)
        {
            m_adErrorButtonOkClicked = false;
            m_adErrorMenuShow = false;
            Sounds::resumeBackgroundMusic();
        }
    }

    void PlayStateGUILayer::updateAfterPhysics()
    {
        // Dont need update.
    }

    void PlayStateGUILayer::draw()
    {
        const float GUIWidth = Beryll::MainImGUI::getInstance()->getGUIWidth();
        const float GUIHeight = Beryll::MainImGUI::getInstance()->getGUIHeight();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}); // Lost focus.
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}); // On focus.
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}); // Clicked.

        if(EnumsAndVars::SettingsMenu::interfaceGUI)
        {
            for(const std::shared_ptr<Beryll::GUIObject>& go : m_guiObjects)
            {
                if(go->getIsEnabled())
                {
                    go->draw();
                }
            }

            if(EnumsAndVars::MapsProgress::currentMapIndex > 0)
            {
                m_shotButton->draw();
            }

            // Counters texture.
            ImGui::SetNextWindowPos(ImVec2(0.912f * GUIWidth, -0.002f * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.
            ImGui::Begin("smashedSpeedTexture", nullptr, m_noBackgroundNoFrame | ImGuiWindowFlags_NoBringToFrontOnFocus);
            ImGui::SetCursorPos(ImVec2(0.005f * GUIWidth, 0.0f));
            ImGui::Image(reinterpret_cast<ImTextureID>(m_countersTexture->getID()),
                         ImVec2(0.088f * GUIWidth, 0.144f * GUIHeight));
            ImGui::End();

            // Counters texts.
            ImGui::SetNextWindowPos(ImVec2(0.94f * GUIWidth, 0.012f * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
            ImGui::Begin("smashedSpeedTexts", nullptr, m_noBackgroundNoFrame);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0.0625f, 0.0586f, 0.0898f, 1.0f});
            ImGui::PushFont(m_countersFont);
            ImGui::SetCursorPos(ImVec2(0.005f * GUIWidth, 0.0f * GUIHeight));
            ImGui::Text("%d", EnumsAndVars::enemiesKilledCount);
            ImGui::SetCursorPos(ImVec2(0.005f * GUIWidth, 0.046f * GUIHeight));
            ImGui::Text("%d/%d", int(std::roundf(EnumsAndVars::playerCurrentSpeed)), int(std::roundf(EnumsAndVars::playerMaxSpeedXZ)));

            m_mapPlayTimerText = "";
            const int min = int(EnumsAndVars::mapPlayTimeSec / 60.0f);
            const int sec = int(std::fmod(EnumsAndVars::mapPlayTimeSec, 60.0f));
            if(min < 10)
                m_mapPlayTimerText += "0";

            m_mapPlayTimerText += std::to_string(min);
            m_mapPlayTimerText += ":";

            if(sec < 10)
                m_mapPlayTimerText += "0";

            m_mapPlayTimerText += std::to_string(sec);

            ImGui::SetCursorPos(ImVec2(0.005f * GUIWidth, 0.092f * GUIHeight));
            ImGui::Text("%s", m_mapPlayTimerText.c_str()); // ImGUI ignores "%s". Modify void ImFormatStringToTempBufferV( to avoid that.
            ImGui::PopFont();
            ImGui::PopStyleColor(1);
            ImGui::End();

            // HP XP.
            ImGui::SetNextWindowPos(ImVec2(0.912f * GUIWidth, 0.14f * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
            ImGui::Begin("HPXP", nullptr, m_noBackgroundNoFrame);
            ImGui::SetCursorPos(ImVec2(0.005f * GUIWidth, 0.0f * GUIHeight));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0.0f, 0.0f, 0.0f, 0.0f});
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4{0.6289f, 1.0f, 0.3086f, 1.0f});
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{0.8516f, 0.0859f, 0.1641f, 1.0f});
            ImGui::ProgressBar(progressBarHP, ImVec2(0.084f * GUIWidth, 0.03f * GUIHeight));
            ImGui::PopStyleColor(3);
            ImGui::SetCursorPos(ImVec2(0.005f * GUIWidth, 0.03f * GUIHeight));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0.0f, 0.0f, 0.0f, 0.0f});
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4{0.1367f, 0.6016f, 0.953f, 1.0f});
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{0.1953f, 0.1953f, 0.2422f, 1.0f});
            ImGui::ProgressBar(progressBarXP, ImVec2(0.084f * GUIWidth, 0.03f * GUIHeight));
            ImGui::PopStyleColor(3);
            ImGui::End();

            // Pause button.
            ImGui::SetNextWindowPos(ImVec2(-0.003f * GUIWidth, -0.002f * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.
            ImGui::Begin("pauseButton", nullptr, m_noBackgroundNoFrame);
            ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
            m_pauseButtonClicked = ImGui::ImageButton("pauseButton", reinterpret_cast<ImTextureID>(m_pauseButtonTexture->getID()),
                                                      ImVec2(0.06f * GUIWidth, 0.12f * GUIHeight));
            ImGui::End();

            // Pause menu.
            if(m_pauseMenuShow)
            {
                ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f});
                ImGui::SetNextWindowPos(ImVec2(-0.005f * GUIWidth, -0.005f * GUIHeight));
                ImGui::SetNextWindowSize(ImVec2(1.01f * GUIWidth, 1.01f * GUIHeight));
                ImGui::Begin("pauseMenu", nullptr, m_noBackgroundNoFrame);

                ImGui::SetCursorPos(ImVec2(0.39f * GUIWidth, 0.417f * GUIHeight));
                m_resumeButtonClicked = ImGui::ImageButton("pauseMenuResumeButton", reinterpret_cast<ImTextureID>(m_resumeButtonTexture->getID()),
                                                           ImVec2(0.23f * GUIWidth, 0.176f * GUIHeight));
                ImGui::SetCursorPos(ImVec2(0.435f * GUIWidth, 0.705f * GUIHeight));
                m_exitButtonClicked = ImGui::ImageButton("pauseMenuExitButton", reinterpret_cast<ImTextureID>(m_exitTexture->getID()),
                                                         ImVec2(0.14f * GUIWidth, 0.1528f * GUIHeight));
                ImGui::End();
                ImGui::PopStyleColor(1);
            }
        }

        // Messages about map progress / win / lose ... should be shown even if interface disabled.

        // Map0Tutorial.
        if(tutorialMoveShow)
        {
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f});
            ImGui::SetNextWindowPos(ImVec2(-0.005f * GUIWidth, 0.4f * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(1.01f * GUIWidth, 0.55f * GUIHeight));
            ImGui::Begin("tutorialMoveAndCamera", nullptr, m_noBackgroundNoFrame);

            ImGui::SetCursorPos(ImVec2(0.055f * GUIWidth, 0.0f * GUIHeight));
            ImGui::Image(reinterpret_cast<ImTextureID>(m_tutorialMoveTexture->getID()),
                         ImVec2(0.3f * GUIWidth, 0.5f * GUIHeight));

            ImGui::SetCursorPos(ImVec2(0.655f * GUIWidth, 0.0f * GUIHeight));
            ImGui::Image(reinterpret_cast<ImTextureID>(m_tutorialCameraTexture->getID()),
                         ImVec2(0.3f * GUIWidth, 0.5f * GUIHeight));
            ImGui::End();
            ImGui::PopStyleColor(1);
        }

        if(m_resurrectMenuShow)
        {
            ImGui::SetNextWindowPos(ImVec2(-0.005f * GUIWidth, -0.005f * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(1.01f * GUIWidth, 1.01f * GUIHeight));
            ImGui::Begin("resurrectMenu", nullptr, m_noBackgroundNoFrame);

            ImGui::SetCursorPos(ImVec2(0.355f * GUIWidth, 0.155f * GUIHeight));
            ImGui::Image(reinterpret_cast<ImTextureID>(m_resurrectTexture->getID()),
                         ImVec2(0.3f * GUIWidth, 0.34f * GUIHeight));

            ImGui::SetCursorPos(ImVec2(0.355f * GUIWidth, 0.52f * GUIHeight));
            m_resurrectByAdButtonClicked = ImGui::ImageButton("resurrectByAdButton",reinterpret_cast<ImTextureID>(m_resurrectByAdButtonTexture->getID()),
                                                              ImVec2(0.14f * GUIWidth, 0.1528f * GUIHeight));

            ImGui::SetCursorPos(ImVec2(0.515f * GUIWidth, 0.52f * GUIHeight));
            m_resurrectByCrystalsButtonClicked = ImGui::ImageButton("resurrectByCrystalsButton", reinterpret_cast<ImTextureID>(m_resurrectByCrystalsButtonTexture->getID()),
                                                                    ImVec2(0.14f * GUIWidth, 0.1528f * GUIHeight));

            ImGui::SetCursorPos(ImVec2(0.435f * GUIWidth, 0.705f * GUIHeight));
            m_exitButtonClicked = ImGui::ImageButton("resurrectExitButton", reinterpret_cast<ImTextureID>(m_exitTexture->getID()),
                                                     ImVec2(0.14f * GUIWidth, 0.1528f * GUIHeight));
            ImGui::End();
        }

        if(m_noCrystalsMenuShow)
        {
            //ImGui::SetNextWindowFocus();
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{0.0f, 0.0f, 0.0f, 0.92f});
            ImGui::SetNextWindowPos(ImVec2(-0.005f * GUIWidth, -0.005f * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(1.01f * GUIWidth, 1.01f * GUIHeight));
            ImGui::Begin("noCrystalsMenu", nullptr, m_noFrame);

            ImGui::SetCursorPos(ImVec2(0.355f * GUIWidth, 0.155f * GUIHeight));
            ImGui::Image(reinterpret_cast<ImTextureID>(m_noCrystalsTexture->getID()),
                         ImVec2(0.3f * GUIWidth, 0.5f * GUIHeight));

            ImGui::SetCursorPos(ImVec2(0.435f * GUIWidth, 0.705f * GUIHeight));
            m_noCrystalsButtonOkClicked = ImGui::ImageButton("noCrystalsOkButton",reinterpret_cast<ImTextureID>(m_okTexture->getID()),
                                                             ImVec2(0.14f * GUIWidth, 0.1528f * GUIHeight));
            ImGui::End();
            ImGui::PopStyleColor(1);
        }

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
            m_adErrorButtonOkClicked = ImGui::ImageButton("adErrorOkButton",reinterpret_cast<ImTextureID>(m_okTexture->getID()),
                                                          ImVec2(0.14f * GUIWidth, 0.1528f * GUIHeight));

            ImGui::End();
            ImGui::PopStyleColor(1);
        }

        if(m_killAllMenuShow) // Can be before boss or without boss.
        {
            ImGui::SetNextWindowPos(ImVec2(-0.005f * GUIWidth, -0.005f * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(1.01f * GUIWidth, 1.01f * GUIHeight));
            ImGui::Begin("killAllMenu", nullptr, m_noBackgroundNoFrame);

            ImGui::SetCursorPos(ImVec2(0.355f * GUIWidth, 0.155f * GUIHeight));
            if(m_killAllToSpawnBoss) // Show message to spawn boss.
                ImGui::Image(reinterpret_cast<ImTextureID>(m_killAllToSpawnBossTexture->getID()),
                             ImVec2(0.3f * GUIWidth, 0.5f * GUIHeight));
            else // Show message to win without boss.
                ImGui::Image(reinterpret_cast<ImTextureID>(m_killAllToWinTexture->getID()),
                             ImVec2(0.3f * GUIWidth, 0.5f * GUIHeight));

            ImGui::SetCursorPos(ImVec2(0.435f * GUIWidth, 0.705f * GUIHeight));
            m_killAllButtonClicked = ImGui::ImageButton("killAllOkButton",reinterpret_cast<ImTextureID>(m_okTexture->getID()),
                                                          ImVec2(0.14f * GUIWidth, 0.1528f * GUIHeight));
            ImGui::End();
        }

        if(m_winMenuShow)
        {
            ImGui::SetNextWindowPos(ImVec2(-0.005f * GUIWidth, -0.005f * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(1.01f * GUIWidth, 1.01f * GUIHeight));
            ImGui::Begin("winMenu", nullptr, m_noBackgroundNoFrame);

            ImGui::SetCursorPos(ImVec2(0.355f * GUIWidth, 0.155f * GUIHeight));
            ImGui::Image(reinterpret_cast<ImTextureID>(m_winTexture->getID()),
                         ImVec2(0.3f * GUIWidth, 0.34f * GUIHeight));

            ImGui::SetCursorPos(ImVec2(0.355f * GUIWidth, 0.52f * GUIHeight));
            m_winPrize1ButtonClicked = ImGui::ImageButton("winPrize1Button",reinterpret_cast<ImTextureID>(m_winPrize1ButtonTexture->getID()),
                                                          ImVec2(0.14f * GUIWidth, 0.1528f * GUIHeight));

            ImGui::SetCursorPos(ImVec2(0.515f * GUIWidth, 0.52f * GUIHeight));
            m_winPrize2ButtonClicked = ImGui::ImageButton("winPrize2Button", reinterpret_cast<ImTextureID>(m_winPrize2ButtonTexture->getID()),
                                                          ImVec2(0.14f * GUIWidth, 0.1528f * GUIHeight));
            ImGui::End();
        }

        if(m_loseMenuShow)
        {
            ImGui::SetNextWindowPos(ImVec2(-0.005f * GUIWidth, -0.005f * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(1.01f * GUIWidth, 1.01f * GUIHeight));
            ImGui::Begin("loseMenu", nullptr, m_noBackgroundNoFrame);

            ImGui::SetCursorPos(ImVec2(0.355f * GUIWidth, 0.155f * GUIHeight));
            ImGui::Image(reinterpret_cast<ImTextureID>(m_loseTexture->getID()),
                         ImVec2(0.3f * GUIWidth, 0.34f * GUIHeight));

            ImGui::SetCursorPos(ImVec2(0.435f * GUIWidth, 0.52f * GUIHeight));
            m_exitButtonClicked = ImGui::ImageButton("loseMenuExitButton", reinterpret_cast<ImTextureID>(m_exitTexture->getID()),
                                                     ImVec2(0.14f * GUIWidth, 0.1528f * GUIHeight));
            ImGui::End();
        }

        if(m_tankWithCommanderMenuShow)
        {
            ImGui::SetNextWindowPos(ImVec2(-0.005f * GUIWidth, -0.005f * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(1.01f * GUIWidth, 1.01f * GUIHeight));
            ImGui::Begin("tankWithCommanderMenu", nullptr, m_noBackgroundNoFrame);

            ImGui::SetCursorPos(ImVec2(0.355f * GUIWidth, 0.155f * GUIHeight));
            ImGui::Image(reinterpret_cast<ImTextureID>(m_tankWithCommanderTexture->getID()),
                         ImVec2(0.3f * GUIWidth, 0.5f * GUIHeight));

            ImGui::SetCursorPos(ImVec2(0.435f * GUIWidth, 0.705f * GUIHeight));
            m_tankWithCommanderButtonClicked = ImGui::ImageButton("tankWithCommanderOkButton",reinterpret_cast<ImTextureID>(m_okTexture->getID()),
                                                                  ImVec2(0.14f * GUIWidth, 0.1528f * GUIHeight));
            ImGui::End();
        }

        ImGui::PopStyleColor(3);
    }

    void PlayStateGUILayer::showMenuResurrect()
    {
        if(m_resurrectMenuShow)
            return;

        m_resurrectMenuShow = true;

        GameStateHelper::pauseGame();
    }

    void PlayStateGUILayer::showMenuKillAllToSpawnBoss()
    {
        if(m_killAllMenuShow)
            return;

        m_killAllMenuShow = true;
        m_killAllToSpawnBoss = true;

        GameStateHelper::pauseGame();
    }

    void PlayStateGUILayer::showMenuKillAllToWin()
    {
        if(m_killAllMenuShow)
            return;

        m_killAllMenuShow = true;
        m_killAllToSpawnBoss = false;

        GameStateHelper::pauseGame();
    }

    void PlayStateGUILayer::showMenuLose()
    {
        if(m_loseMenuShow)
            return;

        m_loseMenuShow = true;
        Sounds::stopBackgroundMusic();
        GameStateHelper::pauseGame();
    }

    void PlayStateGUILayer::showMenuWin()
    {
        if(m_winMenuShow)
            return;

        m_winMenuShow = true;

        Sounds::stopBackgroundMusic();
        GameStateHelper::pauseGame();
    }

    void PlayStateGUILayer::showMenuBossTankWithCommander()
    {
        if(m_tankWithCommanderMenuShow)
            return;

        m_tankWithCommanderMenuShow = true;

        GameStateHelper::pauseGame();
    }
}
