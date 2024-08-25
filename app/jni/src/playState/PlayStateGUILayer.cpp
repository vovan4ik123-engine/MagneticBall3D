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
    std::atomic<bool> PlayStateGUILayer::m_commonAdError = false;

    PlayStateGUILayer::PlayStateGUILayer()
    {
        m_ID = Beryll::LayerID::PLAY_GUI;

        const float screenAR = Beryll::MainImGUI::getInstance()->getGUIScreenAspectRation();

        if(m_statisticsShow)
        {
            m_statistics1 = std::make_shared<Beryll::Text>("Frame: 00000  FPS: 00000", EnumsAndVars::FontsPath::roboto, 0.025f, 0.1f, 0, 0.55f, 0.03f);
            m_guiObjects.push_back(m_statistics1);
            m_statistics2 = std::make_shared<Beryll::Text>("Phys: 00000  Logic: 00000  GPU: 00000", EnumsAndVars::FontsPath::roboto, 0.025f, 0.1f, 0.025f, 0.75f, 0.03f);
            m_guiObjects.push_back(m_statistics2);
            m_swipeCount = std::make_shared<Beryll::Text>("Swipe: 0000 Time: 00000", EnumsAndVars::FontsPath::roboto, 0.02f, 0.1f, 0.0475f, 0.5f, 0.025f);
            m_guiObjects.push_back(m_swipeCount);
        }

//        slider1 = std::make_shared<Beryll::SliderHorizontal>("swipe power", EnumsAndVars::FontsPath::roboto, 0.02f, 0.02f, 0.07f, 0.4f, 0.02f, 2, 3);
//        m_guiObjects.push_back(slider1);
//        slider1->setValue(2.5f);
//
//        slider2 = std::make_shared<Beryll::SliderHorizontal>("turn power", EnumsAndVars::FontsPath::roboto, 0.02f, 0.02f, 0.13f, 0.4f, 0.02f, 0.01f, 0.02f);
//        m_guiObjects.push_back(slider2);
//        slider2->setValue(0.02f);

//        slider3 = std::make_shared<Beryll::SliderHorizontal>("specular power", EnumsAndVars::FontsPath::roboto, 0.02f, 0.02f, 0.1f, 0.4f, 0.02f, 0, 2.0f);
//        m_guiObjects.push_back(slider3);
//        slider3->setValue(1.0f);


        m_mapPlayTimerFont = Beryll::MainImGUI::getInstance()->createFont(EnumsAndVars::FontsPath::roboto, 0.03f);
        m_smashedSpeedFont = Beryll::MainImGUI::getInstance()->createFont(EnumsAndVars::FontsPath::roboto, 0.02f);

        m_smashedSpeedTexture = Beryll::Renderer::createTexture("GUI/playState/SmashedSpeed.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_pauseButtonTexture = Beryll::Renderer::createTexture("GUI/playState/Pause.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_resumeButtonTexture = Beryll::Renderer::createTexture("GUI/playState/Resume.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_exitButtonTexture = Beryll::Renderer::createTexture("GUI/Exit.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_tutorialSwipeTexture = Beryll::Renderer::createTexture("GUI/playState/TutorialSwipeToMove.png", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_tutorialSwipeOnWallTexture = Beryll::Renderer::createTexture("GUI/playState/TutorialSwipeOnWall.png", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);

        m_resurrectTexture = Beryll::Renderer::createTexture("GUI/playState/CanResurrect.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_resurrectByCrystalsButtonTexture = Beryll::Renderer::createTexture("GUI/playState/ResurrectByCrystals.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_resurrectByAdButtonTexture = Beryll::Renderer::createTexture("GUI/playState/ResurrectByAd.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);

        m_noCrystalsTexture = Beryll::Renderer::createTexture("GUI/NotEnoughCrystals.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_noCrystalsButtonOkTexture = Beryll::Renderer::createTexture("GUI/Ok.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_adLoadingTexture = Beryll::Renderer::createTexture("GUI/AdLoading.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_adErrorTexture = Beryll::Renderer::createTexture("GUI/SomethingWentWrong.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_adErrorButtonOkTexture = Beryll::Renderer::createTexture("GUI/Ok.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);

        m_loseTexture = Beryll::Renderer::createTexture("GUI/playState/Lose.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_killAllToSpawnBossTexture = Beryll::Renderer::createTexture("GUI/playState/KillAllEnemiesToSpawnBoss.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_killAllToWinTexture = Beryll::Renderer::createTexture("GUI/playState/KillAllEnemiesToWin.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_killAllButtonOkTexture = Beryll::Renderer::createTexture("GUI/Ok.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_winTexture = Beryll::Renderer::createTexture("GUI/playState/Win.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_winPrize1ButtonTexture = Beryll::Renderer::createTexture("GUI/playState/WinPrize1.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_winPrize2ButtonTexture = Beryll::Renderer::createTexture("GUI/playState/WinPrize2.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_tankWithCommanderTexture = Beryll::Renderer::createTexture("GUI/playState/BossTankWithCommander.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_tankWithCommanderButtonOkTexture = Beryll::Renderer::createTexture("GUI/Ok.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);

        PlayStateGUILayer::m_resurrectAdSuccess = false;
        PlayStateGUILayer::m_winPrize1AdSuccess = false;
        PlayStateGUILayer::m_winPrize2AdSuccess = false;
        PlayStateGUILayer::m_exitAdSuccess = false;
        PlayStateGUILayer::m_commonAdError = false;

        // These callbacks are called from different thread.
        m_resurrectAdSuccessCallback = []() -> void { BR_INFO("%s", "m_resurrectAdSuccessCallback()"); PlayStateGUILayer::m_resurrectAdSuccess = true; };
        m_winPrize1AdSuccessCallback = []() -> void { BR_INFO("%s", "m_winPrize1AdSuccessCallback()"); PlayStateGUILayer::m_winPrize1AdSuccess = true; };
        m_winPrize2AdSuccessCallback = []() -> void { BR_INFO("%s", "m_winPrize2AdSuccessCallback()"); PlayStateGUILayer::m_winPrize2AdSuccess = true; };
        m_exitAdSuccessCallback = []() -> void { BR_INFO("%s", "m_exitAdSuccessCallback()"); PlayStateGUILayer::m_exitAdSuccess = true; };
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
            stream << "Swipe: " << EnumsAndVars::mapSwipeCount;
            stream << "  Time: " << int(EnumsAndVars::mapPlayTimeSec / 60.0f) << ":" << int(std::fmod(EnumsAndVars::mapPlayTimeSec, 60.0f));
            m_swipeCount->text = stream.str();
            stream.str("");

            m_statisticsUpdateTime = Beryll::TimeStep::getMilliSecFromStart();
        }


        // Only for buttons. Make delay to prevent accidental click when user swipe during gameplay.
        if(m_timeAppearsOnScreen + m_delayBeforeCanBeClicked > Beryll::TimeStep::getSecFromStart())
            return;

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
            SendStatisticsHelper::sendCustomMessage("click_exit_button");

            if(EnumsAndVars::Shop::adsOnMapsDisabled)
            {
                m_exitAdSuccessCallback();
            }
            else
            {
                m_adLoadingMenuShow = true;
                Beryll::Ads::getInstance()->showInterstitialAd(m_exitAdSuccessCallback, m_commonAdErrorCallback);
            }
        }
        else if(m_killAllButtonClicked)
        {
            m_killAllButtonClicked = false;
            m_killAllMenuShow = false;

            GameStateHelper::resumeGame();
        }
        else if(m_winPrize1ButtonClicked)
        {
            BR_INFO("%s", "m_winPrize1ButtonClicked");
            m_winPrize1ButtonClicked = false;
            SendStatisticsHelper::sendCustomMessage("click_win_prize_1");

            if(EnumsAndVars::Shop::adsOnMapsDisabled)
            {
                m_winPrize1AdSuccessCallback();
            }
            else
            {
                m_adLoadingMenuShow = true;
                Beryll::Ads::getInstance()->showInterstitialAd(m_winPrize1AdSuccessCallback, m_commonAdErrorCallback);
            }
        }
        else if(m_winPrize2ButtonClicked)
        {
            BR_INFO("%s", "m_winPrize2ButtonClicked");
            m_winPrize2ButtonClicked = false;
            SendStatisticsHelper::sendCustomMessage("click_win_prize_2");

            if(EnumsAndVars::Shop::adsOnMapsDisabled)
            {
                m_winPrize2AdSuccessCallback();
            }
            else
            {
                m_adLoadingMenuShow = true;
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
            SendStatisticsHelper::sendCustomMessage("click_resurrect_ad");
            Sounds::pauseBackgroundMusic();

            if(EnumsAndVars::Shop::adsOnMapsDisabled)
            {
                m_resurrectAdSuccessCallback();
            }
            else
            {
                m_adLoadingMenuShow = true;
                Beryll::Ads::getInstance()->showRewardedAd(m_resurrectAdSuccessCallback, m_commonAdErrorCallback, true);
            }
        }
        else if(m_resurrectByCrystalsButtonClicked)
        {
            BR_INFO("%s", "m_resurrectByCrystalsButtonClicked");
            m_resurrectByCrystalsButtonClicked = false;
            SendStatisticsHelper::sendCustomMessage("click_resurrect_crystals");

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

            EnumsAndVars::CurrencyBalance::crystals += 20;
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

            EnumsAndVars::CurrencyBalance::crystals += 40;
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
        for(const std::shared_ptr<Beryll::GUIObject>& go : m_guiObjects)
        {
            if(go->getIsEnabled())
            {
                go->draw();
            }
        }

        const float GUIWidth = Beryll::MainImGUI::getInstance()->getGUIWidth();
        const float GUIHeight = Beryll::MainImGUI::getInstance()->getGUIHeight();

        // HP XP.
        ImGui::SetNextWindowPos(ImVec2(-0.01f * GUIWidth, 0.96f * GUIHeight));
        ImGui::SetNextWindowSize(ImVec2(1.02f * GUIWidth, 0.06f * GUIHeight));
        ImGui::Begin("HPXP", nullptr, m_noBackgroundNoFrame);
        ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0.0f, 0.0f, 0.0f, 0.0f});
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4{0.0f, 1.0f, 0.0f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{1.0f, 0.0f, 0.0f, 1.0f});
        ImGui::ProgressBar(progressBarHP, ImVec2(1.02f * GUIWidth, 0.02f * GUIHeight));
        ImGui::PopStyleColor(3);
        ImGui::SetCursorPos(ImVec2(0.0f, 0.02f * GUIHeight));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0.0f, 0.0f, 0.0f, 0.0f});
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4{0.0f, 0.0f, 1.0f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{0.0f, 0.0f, 0.0f, 1.0f});
        ImGui::ProgressBar(progressBarXP, ImVec2(1.02f * GUIWidth, 0.04f * GUIHeight));
        ImGui::PopStyleColor(3);
        ImGui::End();

        // Play timer.
        if(m_mapPlayTimerShow)
        {
            m_mapPlayTimerText = "";
            int min = int(EnumsAndVars::mapPlayTimeSec / 60.0f);
            int sec = int(std::fmod(EnumsAndVars::mapPlayTimeSec, 60.0f));
            if(min < 10)
                m_mapPlayTimerText += "0";

            m_mapPlayTimerText += std::to_string(min);
            m_mapPlayTimerText += ":";

            if(sec < 10)
                m_mapPlayTimerText += "0";

            m_mapPlayTimerText += std::to_string(sec);

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0.0f, 0.0f, 0.0f, 1.0f });
            ImGui::SetNextWindowPos(ImVec2(0.42f * GUIWidth, -0.005f * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
            ImGui::Begin("mapPlayTimer", nullptr, m_noBackgroundNoFrame);
            ImGui::PushFont(m_mapPlayTimerFont);
            ImGui::Text("%s", m_mapPlayTimerText.c_str()); // ImGUI ignores "%s". Modify void ImFormatStringToTempBufferV( to avoid that.
            ImGui::PopFont();
            ImGui::End();
            ImGui::PopStyleColor(1);
        }

        // Smashed + speed texture.
        ImGui::SetNextWindowPos(ImVec2(0.77f * GUIWidth, 0.0f * GUIHeight));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.
        ImGui::Begin("smashedSpeedTexture", nullptr, m_noBackgroundNoFrame | ImGuiWindowFlags_NoBringToFrontOnFocus);
        ImGui::SetCursorPos(ImVec2(0.01f * GUIWidth, 0.0f));
        ImGui::Image(reinterpret_cast<ImTextureID>(m_smashedSpeedTexture->getID()),
                     ImVec2(0.26f * GUIWidth, 0.06f * GUIHeight));
        ImGui::End();

        // Smashed + speed texts.
        ImGui::SetNextWindowPos(ImVec2(0.84f * GUIWidth, 0.006f * GUIHeight));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
        ImGui::Begin("smashedSpeedTexts", nullptr, m_noBackgroundNoFrame);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0.0f, 0.0f, 0.0f, 1.0f });
        ImGui::PushFont(m_smashedSpeedFont);
        ImGui::SetCursorPos(ImVec2(0.01f * GUIWidth, 0.001f * GUIHeight));
        ImGui::Text("%d", EnumsAndVars::enemiesKilledCount);
        ImGui::SetCursorPos(ImVec2(0.01f * GUIWidth, 0.031f * GUIHeight));
        ImGui::Text("%d/%d", int(std::roundf(EnumsAndVars::playerCurrentSpeed)), int(std::roundf(EnumsAndVars::playerMaxSpeedXZ)));
        ImGui::PopFont();
        ImGui::PopStyleColor(1);
        ImGui::End();

        // Pause button.
        ImGui::SetNextWindowPos(ImVec2(-0.005f * GUIWidth, -0.001f * GUIHeight));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.
        ImGui::Begin("pauseButton", nullptr, m_noBackgroundNoFrame);
        ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
        m_pauseButtonClicked = ImGui::ImageButton("pauseButton", reinterpret_cast<ImTextureID>(m_pauseButtonTexture->getID()),
                                                  ImVec2(0.13f * GUIWidth, 0.05f * GUIHeight));
        ImGui::End();

        // Pause menu.
        if(m_pauseMenuShow)
        {
            ImGui::SetNextWindowPos(ImVec2(0.25f * GUIWidth, 0.505f * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.
            ImGui::Begin("pauseMenu", nullptr, m_noBackgroundNoFrame);
            ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
            m_resumeButtonClicked = ImGui::ImageButton("pauseMenuResumeButton", reinterpret_cast<ImTextureID>(m_resumeButtonTexture->getID()),
                                                       ImVec2(0.5f * GUIWidth, 0.08f * GUIHeight));
            ImGui::SetCursorPos(ImVec2(0.1f * GUIWidth, 0.15f * GUIHeight));
            m_exitButtonClicked = ImGui::ImageButton("pauseMenuExitButton", reinterpret_cast<ImTextureID>(m_exitButtonTexture->getID()),
                                                     ImVec2(0.3f * GUIWidth, 0.07f * GUIHeight));
            ImGui::End();
        }

        // Map0Tutorial.
        if(tutorialSwipeShow)
        {
            ImGui::SetNextWindowPos(ImVec2(0.093f * GUIWidth, 0.55f * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.
            ImGui::Begin("tutorialSwipe", nullptr, m_noBackgroundNoFrame);
            ImGui::Image(reinterpret_cast<ImTextureID>(m_tutorialSwipeTexture->getID()),
                         ImVec2(0.8f * GUIWidth, 0.37f * GUIHeight));
            ImGui::End();
        }

        if(tutorialSwipeOnWallShow)
        {
            ImGui::SetNextWindowPos(ImVec2(0.12f * GUIWidth, 0.07f * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.
            ImGui::Begin("tutorialSwipeOnWall", nullptr, m_noBackgroundNoFrame);
            ImGui::Image(reinterpret_cast<ImTextureID>(m_tutorialSwipeOnWallTexture->getID()),
                         ImVec2(0.76f * GUIWidth, 0.08f * GUIHeight));
            ImGui::End();
        }

        if(m_resurrectMenuShow)
        {
            ImGui::SetNextWindowPos(ImVec2(0.2f * GUIWidth, 0.25f * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
            ImGui::Begin("resurrectMenu", nullptr, m_noBackgroundNoFrame);
            ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
            ImGui::Image(reinterpret_cast<ImTextureID>(m_resurrectTexture->getID()),
                         ImVec2(0.6f * GUIWidth, 0.25f * GUIHeight));
            ImGui::SetCursorPos(ImVec2(0.0f * GUIWidth, 0.255f * GUIHeight));
            m_resurrectByAdButtonClicked = ImGui::ImageButton("resurrectByAdButton",reinterpret_cast<ImTextureID>(m_resurrectByAdButtonTexture->getID()),
                                                              ImVec2(0.295f * GUIWidth, 0.07f * GUIHeight));
            ImGui::SetCursorPos(ImVec2(0.31f * GUIWidth, 0.255f * GUIHeight));
            m_resurrectByCrystalsButtonClicked = ImGui::ImageButton("resurrectByCrystalsButton", reinterpret_cast<ImTextureID>(m_resurrectByCrystalsButtonTexture->getID()),
                                                                    ImVec2(0.295f * GUIWidth, 0.07f * GUIHeight));
            ImGui::SetCursorPos(ImVec2(0.15f * GUIWidth, 0.4f * GUIHeight));
            m_exitButtonClicked = ImGui::ImageButton("resurrectMenuExitButton", reinterpret_cast<ImTextureID>(m_exitButtonTexture->getID()),
                                                     ImVec2(0.3f * GUIWidth, 0.07f * GUIHeight));
            ImGui::End();
        }

        // Not enough crystals menu.
        if(m_noCrystalsMenuShow)
        {
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{ 0.0f, 0.0f, 0.0f, 0.92f });
            ImGui::SetNextWindowFocus();
            ImGui::SetNextWindowPos(ImVec2(0.0f * GUIWidth, -0.01f * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(1.0f * GUIWidth, 1.02f * GUIHeight));
            ImGui::Begin("noCrystalsMenu", nullptr, m_noFrame);
            ImGui::SetCursorPos(ImVec2(0.2f * GUIWidth, 0.25f * GUIHeight));
            ImGui::Image(reinterpret_cast<ImTextureID>(m_noCrystalsTexture->getID()),
                         ImVec2(0.6f * GUIWidth, 0.25f * GUIHeight));
            ImGui::SetCursorPos(ImVec2(0.35f * GUIWidth, 0.505f * GUIHeight));
            m_noCrystalsButtonOkClicked = ImGui::ImageButton("noCrystalsOkButton",reinterpret_cast<ImTextureID>(m_noCrystalsButtonOkTexture->getID()),
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
            ImGui::Begin("adLoadingMenu", nullptr, m_noFrame);
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
            ImGui::Begin("adErrorMenu", nullptr, m_noFrame);
            ImGui::SetCursorPos(ImVec2(0.2f * GUIWidth, 0.25f * GUIHeight));
            ImGui::Image(reinterpret_cast<ImTextureID>(m_adErrorTexture->getID()),
                         ImVec2(0.6f * GUIWidth, 0.25f * GUIHeight));
            ImGui::SetCursorPos(ImVec2(0.35f * GUIWidth, 0.505f * GUIHeight));
            m_adErrorButtonOkClicked = ImGui::ImageButton("adErrorOkButton",reinterpret_cast<ImTextureID>(m_adErrorButtonOkTexture->getID()),
                                                          ImVec2(0.3f * GUIWidth, 0.07f * GUIHeight));
            ImGui::End();
            ImGui::PopStyleColor(1);
        }

        // Menu kill all can be before boss or without boss.
        if(m_killAllMenuShow)
        {
            ImGui::SetNextWindowPos(ImVec2(0.2f * GUIWidth, 0.25f * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
            ImGui::Begin("killAllMenu", nullptr, m_noBackgroundNoFrame);
            ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
            if(m_killAllToSpawnBoss) // Show message to spawn boss.
                ImGui::Image(reinterpret_cast<ImTextureID>(m_killAllToSpawnBossTexture->getID()),
                             ImVec2(0.6f * GUIWidth, 0.25f * GUIHeight));
            else // Show message to win without boss.
                ImGui::Image(reinterpret_cast<ImTextureID>(m_killAllToWinTexture->getID()),
                             ImVec2(0.6f * GUIWidth, 0.25f * GUIHeight));
            ImGui::SetCursorPos(ImVec2(0.15f * GUIWidth, 0.255f * GUIHeight));
            m_killAllButtonClicked = ImGui::ImageButton("killAllOkButton",reinterpret_cast<ImTextureID>(m_killAllButtonOkTexture->getID()),
                                                          ImVec2(0.3f * GUIWidth, 0.07f * GUIHeight));
            ImGui::End();
        }

        if(m_winMenuShow)
        {
            ImGui::SetNextWindowPos(ImVec2(0.2f * GUIWidth, 0.25f * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
            ImGui::Begin("winMenu", nullptr, m_noBackgroundNoFrame);
            ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
            ImGui::Image(reinterpret_cast<ImTextureID>(m_winTexture->getID()),
                         ImVec2(0.6f * GUIWidth, 0.25f * GUIHeight));
            ImGui::SetCursorPos(ImVec2(0.0f * GUIWidth, 0.255f * GUIHeight));
            m_winPrize1ButtonClicked = ImGui::ImageButton("winPrize1Button",reinterpret_cast<ImTextureID>(m_winPrize1ButtonTexture->getID()),
                                                              ImVec2(0.295f * GUIWidth, 0.07f * GUIHeight));
            ImGui::SetCursorPos(ImVec2(0.31f * GUIWidth, 0.255f * GUIHeight));
            m_winPrize2ButtonClicked = ImGui::ImageButton("winPrize2Button", reinterpret_cast<ImTextureID>(m_winPrize2ButtonTexture->getID()),
                                                                    ImVec2(0.295f * GUIWidth, 0.07f * GUIHeight));
            ImGui::End();
        }

        if(m_loseMenuShow)
        {
            ImGui::SetNextWindowPos(ImVec2(0.2f * GUIWidth, 0.25f * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
            ImGui::Begin("loseMenu", nullptr, m_noBackgroundNoFrame);
            ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
            ImGui::Image(reinterpret_cast<ImTextureID>(m_loseTexture->getID()),
                         ImVec2(0.6f * GUIWidth, 0.25f * GUIHeight));
            ImGui::SetCursorPos(ImVec2(0.15f * GUIWidth, 0.255f * GUIHeight));
            m_exitButtonClicked = ImGui::ImageButton("loseMenuExitButton", reinterpret_cast<ImTextureID>(m_exitButtonTexture->getID()),
                                                     ImVec2(0.3f * GUIWidth, 0.07f * GUIHeight));
            ImGui::End();
        }

        if(m_tankWithCommanderMenuShow)
        {
            ImGui::SetNextWindowPos(ImVec2(0.2f * GUIWidth, 0.25f * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
            ImGui::Begin("tankWithCommanderMenu", nullptr, m_noBackgroundNoFrame);
            ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
            ImGui::Image(reinterpret_cast<ImTextureID>(m_tankWithCommanderTexture->getID()),
                         ImVec2(0.6f * GUIWidth, 0.25f * GUIHeight));
            ImGui::SetCursorPos(ImVec2(0.15f * GUIWidth, 0.255f * GUIHeight));
            m_tankWithCommanderButtonClicked = ImGui::ImageButton("tankWithCommanderOkButton",reinterpret_cast<ImTextureID>(m_tankWithCommanderButtonOkTexture->getID()),
                                                                  ImVec2(0.3f * GUIWidth, 0.07f * GUIHeight));
            ImGui::End();
        }
    }

    void PlayStateGUILayer::showMenuResurrect()
    {
        if(m_resurrectMenuShow)
            return;

        m_resurrectMenuShow = true;

        GameStateHelper::pauseGame();

        m_timeAppearsOnScreen = Beryll::TimeStep::getSecFromStart();
    }

    void PlayStateGUILayer::showMenuKillAllToSpawnBoss()
    {
        if(m_killAllMenuShow)
            return;

        m_killAllMenuShow = true;
        m_killAllToSpawnBoss = true;

        GameStateHelper::pauseGame();

        m_timeAppearsOnScreen = Beryll::TimeStep::getSecFromStart();
    }

    void PlayStateGUILayer::showMenuKillAllToWin()
    {
        if(m_killAllMenuShow)
            return;

        m_killAllMenuShow = true;
        m_killAllToSpawnBoss = false;

        GameStateHelper::pauseGame();

        m_timeAppearsOnScreen = Beryll::TimeStep::getSecFromStart();
    }

    void PlayStateGUILayer::showMenuLose()
    {
        if(m_loseMenuShow)
            return;

        m_loseMenuShow = true;
        Sounds::stopBackgroundMusic();
        GameStateHelper::pauseGame();

        m_timeAppearsOnScreen = Beryll::TimeStep::getSecFromStart();
    }

    void PlayStateGUILayer::showMenuWin()
    {
        if(m_winMenuShow)
            return;

        m_winMenuShow = true;

        Sounds::stopBackgroundMusic();
        GameStateHelper::pauseGame();

        m_timeAppearsOnScreen = Beryll::TimeStep::getSecFromStart();
    }

    void PlayStateGUILayer::showMenuBossTankWithCommander()
    {
        if(m_tankWithCommanderMenuShow)
            return;

        m_tankWithCommanderMenuShow = true;

        GameStateHelper::pauseGame();

        m_timeAppearsOnScreen = Beryll::TimeStep::getSecFromStart();
    }
}
