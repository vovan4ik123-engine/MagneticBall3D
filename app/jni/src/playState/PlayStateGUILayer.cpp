#include "PlayStateGUILayer.h"
#include "EnumsAndVariables.h"
#include "GameStateHelper.h"
#include "DataBaseHelper.h"
#include "Sounds.h"

namespace MagneticBall3D
{
    // All there IDs as strings required by ImGUI.
    const std::string PlayStateGUILayer::m_progressBarHPID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_progressBarXPID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_mapPlayTimerID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_smashedCountTextureID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_smashedCountTextID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_speedTextureID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_speedTextID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_pauseButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_resumeButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_exitButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_tutorialSwipeID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_tutorialSwipeOnWallID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_resurrectMenuID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_resurrectByCrystalsButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_resurrectByAdButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_loseTextureID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_killAllTextureID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_killAllButtonOkID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_winMenuID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_winPrize1ButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_winPrize2ButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_tankWithCommanderTextureID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_tankWithCommanderButtonOkID = std::to_string(BeryllUtils::Common::generateID());

    const std::string PlayStateGUILayer::m_noCrystalsMenuID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_noCrystalsButtonOkID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_adLoadingMenuID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_adErrorMenuID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_adErrorButtonOkID = std::to_string(BeryllUtils::Common::generateID());

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
        m_smashedCountFont = Beryll::MainImGUI::getInstance()->createFont(EnumsAndVars::FontsPath::roboto, 0.02f);
        m_speedFont = Beryll::MainImGUI::getInstance()->createFont(EnumsAndVars::FontsPath::roboto, 0.02f);

        m_smashedCountTexture = Beryll::Renderer::createTexture("GUI/playState/KilledIcon.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_speedTexture = Beryll::Renderer::createTexture("GUI/playState/SpeedIcon.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
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
                m_resumeButtonEnabled = true;
                m_exitButtonEnabled = true;
                m_exitButtonTop = 0.65f;
                m_exitButtonLeft = 0.35f;

                GameStateHelper::pauseGame();
            }
        }
        else if(m_resumeButtonClicked)
        {
            m_resumeButtonClicked = false;
            m_resumeButtonEnabled = false;
            m_exitButtonEnabled = false;

            GameStateHelper::resumeGame();
        }
        else if(m_exitButtonClicked)
        {
            m_exitButtonClicked = false;
            m_adLoadingMenuShow = true;

            Beryll::Ads::getInstance()->showInterstitialAd(m_exitAdSuccessCallback, m_commonAdErrorCallback);
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
            m_adLoadingMenuShow = true;

            Beryll::Ads::getInstance()->showInterstitialAd(m_winPrize1AdSuccessCallback, m_commonAdErrorCallback);
        }
        else if(m_winPrize2ButtonClicked)
        {
            BR_INFO("%s", "m_winPrize2ButtonClicked");
            m_winPrize2ButtonClicked = false;
            m_adLoadingMenuShow = true;

            Beryll::Ads::getInstance()->showRewardedAd(m_winPrize2AdSuccessCallback, m_commonAdErrorCallback, true);
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
            m_adLoadingMenuShow = true;
            Sounds::pauseBackgroundMusic();
            Beryll::Ads::getInstance()->showRewardedAd(m_resurrectAdSuccessCallback, m_commonAdErrorCallback, true);
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

            EnumsAndVars::CurrencyBalance::crystals += 50;
            DataBaseHelper::storeCurrencyBalanceCrystals(EnumsAndVars::CurrencyBalance::crystals);

            GameStateHelper::popState();
            GameStateHelper::pushStartMenuState();
            return;
        }

        if(PlayStateGUILayer::m_exitAdSuccess)
        {
            PlayStateGUILayer::m_exitAdSuccess = false;
            m_exitButtonEnabled = false;

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
        // HP bar.
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0.0f, 0.0f, 0.0f, 0.0f});
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4{0.0f, 1.0f, 0.0f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{1.0f, 0.0f, 0.0f, 1.0f});
        ImGui::SetNextWindowPos(ImVec2(-0.02f * GUIWidth, 0.9565f * GUIHeight));
        ImGui::SetNextWindowSize(ImVec2(1.04f * GUIWidth, 0.025f * GUIHeight));
        ImGui::Begin(m_progressBarHPID.c_str(), nullptr, m_noBackgroundNoFrame);
        ImGui::ProgressBar(progressBarHP);
        ImGui::End();
        ImGui::PopStyleColor(3);

        // XP bar.
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0.0f, 0.0f, 0.0f, 0.0f});
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4{0.0f, 0.0f, 1.0f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{0.0f, 0.0f, 0.0f, 1.0f});
        ImGui::SetNextWindowPos(ImVec2(-0.02f * GUIWidth, 0.976f * GUIHeight));
        ImGui::SetNextWindowSize(ImVec2(1.04f * GUIWidth, 0.03f * GUIHeight));
        ImGui::Begin(m_progressBarXPID.c_str(), nullptr, m_noBackgroundNoFrame);
        ImGui::ProgressBar(progressBarXP);
        ImGui::End();
        ImGui::PopStyleColor(3);

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
            ImGui::Begin(m_mapPlayTimerID.c_str(), nullptr, m_noBackgroundNoFrame);
            ImGui::PushFont(m_mapPlayTimerFont);
            ImGui::Text("%s", m_mapPlayTimerText.c_str()); // ImGUI ignores "%s". Modify void ImFormatStringToTempBufferV( to avoid that.
            ImGui::PopFont();
            ImGui::End();
            ImGui::PopStyleColor(1);
        }

        // Smashed count.
        ImGui::SetNextWindowPos(ImVec2(0.77f * GUIWidth, -0.005f * GUIHeight));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.
        ImGui::Begin(m_smashedCountTextureID.c_str(), nullptr, m_noBackgroundNoFrame | ImGuiWindowFlags_NoBringToFrontOnFocus);
        ImGui::Image(reinterpret_cast<ImTextureID>(m_smashedCountTexture->getID()),
                     ImVec2(0.26f * GUIWidth, 0.03f * GUIHeight));
        ImGui::End();

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0.0f, 0.0f, 0.0f, 1.0f });
        ImGui::SetNextWindowPos(ImVec2(0.84f * GUIWidth, 0.002f * GUIHeight));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
        ImGui::Begin(m_smashedCountTextID.c_str(), nullptr, m_noBackgroundNoFrame);
        ImGui::PushFont(m_smashedCountFont);
        ImGui::Text("%d", EnumsAndVars::enemiesKilledCount);
        ImGui::PopFont();
        ImGui::End();
        ImGui::PopStyleColor(1);

        // Speed.
        ImGui::SetNextWindowPos(ImVec2(0.77f * GUIWidth, 0.025f * GUIHeight));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.
        ImGui::Begin(m_speedTextureID.c_str(), nullptr, m_noBackgroundNoFrame | ImGuiWindowFlags_NoBringToFrontOnFocus);
        ImGui::Image(reinterpret_cast<ImTextureID>(m_speedTexture->getID()),
                     ImVec2(0.26f * GUIWidth, 0.03f * GUIHeight));
        ImGui::End();

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0.0f, 0.0f, 0.0f, 1.0f });
        ImGui::SetNextWindowPos(ImVec2(0.84f * GUIWidth, 0.032f * GUIHeight));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
        ImGui::Begin(m_speedTextID.c_str(), nullptr, m_noBackgroundNoFrame);
        ImGui::PushFont(m_speedFont);
        ImGui::Text("%d/%d", int(std::roundf(EnumsAndVars::playerCurrentSpeed)), int(std::roundf(EnumsAndVars::playerMaxSpeedXZ)));
        ImGui::PopFont();
        ImGui::End();
        ImGui::PopStyleColor(1);

        // Button pause.
        ImGui::SetNextWindowPos(ImVec2(-0.011f * GUIWidth, -0.005f * GUIHeight));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.
        ImGui::Begin(m_pauseButtonID.c_str(), nullptr, m_noBackgroundNoFrame);
        m_pauseButtonClicked = ImGui::ImageButton(m_pauseButtonID.c_str(), reinterpret_cast<ImTextureID>(m_pauseButtonTexture->getID()),
                                                  ImVec2(0.13f * GUIWidth, 0.05f * GUIHeight));
        ImGui::End();

        // Button resume.
        if(m_resumeButtonEnabled)
        {
            ImGui::SetNextWindowPos(ImVec2(0.25f * GUIWidth, 0.42f * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.
            ImGui::Begin(m_resumeButtonID.c_str(), nullptr, m_noBackgroundNoFrame);
            m_resumeButtonClicked = ImGui::ImageButton(m_resumeButtonID.c_str(), reinterpret_cast<ImTextureID>(m_resumeButtonTexture->getID()),
                                                       ImVec2(0.5f * GUIWidth, 0.08f * GUIHeight));
            ImGui::End();
        }

        // Button exit.
        if(m_exitButtonEnabled)
        {
            ImGui::SetNextWindowPos(ImVec2(m_exitButtonLeft * GUIWidth, m_exitButtonTop * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.
            ImGui::Begin(m_exitButtonID.c_str(), nullptr, m_noBackgroundNoFrame);
            m_exitButtonClicked = ImGui::ImageButton(m_exitButtonID.c_str(), reinterpret_cast<ImTextureID>(m_exitButtonTexture->getID()),
                                                     ImVec2(0.3f * GUIWidth, 0.07f * GUIHeight));
            ImGui::End();
        }

        // Map0Tutorial.
        if(tutorialSwipeEnabled)
        {
            ImGui::SetNextWindowPos(ImVec2(0.093f * GUIWidth, 0.547f * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.
            ImGui::Begin(m_tutorialSwipeID.c_str(), nullptr, m_noBackgroundNoFrame);
            ImGui::Image(reinterpret_cast<ImTextureID>(m_tutorialSwipeTexture->getID()),
                         ImVec2(0.8f * GUIWidth, 0.367f * GUIHeight));
            ImGui::End();
        }

        if(tutorialSwipeOnWallEnabled)
        {
            ImGui::SetNextWindowPos(ImVec2(0.12f * GUIWidth, 0.07f * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.
            ImGui::Begin(m_tutorialSwipeOnWallID.c_str(), nullptr, m_noBackgroundNoFrame);
            ImGui::Image(reinterpret_cast<ImTextureID>(m_tutorialSwipeOnWallTexture->getID()),
                         ImVec2(0.76f * GUIWidth, 0.08f * GUIHeight));
            ImGui::End();
        }

        if(m_resurrectMenuShow)
        {
            ImGui::SetNextWindowPos(ImVec2(0.2f * GUIWidth, 0.25f * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
            ImGui::Begin(m_resurrectMenuID.c_str(), nullptr, m_noBackgroundNoFrame);
            ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
            ImGui::Image(reinterpret_cast<ImTextureID>(m_resurrectTexture->getID()),
                         ImVec2(0.6f * GUIWidth, 0.25f * GUIHeight));

            ImGui::SetCursorPos(ImVec2(0.0f * GUIWidth, 0.255f * GUIHeight));
            m_resurrectByAdButtonClicked = ImGui::ImageButton(m_resurrectByAdButtonID.c_str(),reinterpret_cast<ImTextureID>(m_resurrectByAdButtonTexture->getID()),
                                                              ImVec2(0.295f * GUIWidth, 0.07f * GUIHeight));

            ImGui::SetCursorPos(ImVec2(0.31f * GUIWidth, 0.255f * GUIHeight));
            m_resurrectByCrystalsButtonClicked = ImGui::ImageButton(m_resurrectByCrystalsButtonID.c_str(), reinterpret_cast<ImTextureID>(m_resurrectByCrystalsButtonTexture->getID()),
                                                                    ImVec2(0.295f * GUIWidth, 0.07f * GUIHeight));

            ImGui::SetCursorPos(ImVec2(0.15f * GUIWidth, 0.4f * GUIHeight));
            m_exitButtonClicked = ImGui::ImageButton(m_exitButtonID.c_str(), reinterpret_cast<ImTextureID>(m_exitButtonTexture->getID()),
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

        // Menu kill all can be before boss or without boss.
        if(m_killAllMenuShow)
        {
            ImGui::SetNextWindowPos(ImVec2(0.2f * GUIWidth, 0.25f * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
            ImGui::Begin(m_killAllTextureID.c_str(), nullptr, m_noBackgroundNoFrame);
            if(m_killAllToSpawnBoss) // Show message to spawn boss.
            {
                ImGui::Image(reinterpret_cast<ImTextureID>(m_killAllToSpawnBossTexture->getID()),
                             ImVec2(0.6f * GUIWidth, 0.25f * GUIHeight));
            }
            else // Show message to win without boss.
            {
                ImGui::Image(reinterpret_cast<ImTextureID>(m_killAllToWinTexture->getID()),
                             ImVec2(0.6f * GUIWidth, 0.25f * GUIHeight));
            }
            ImGui::End();

            ImGui::SetNextWindowPos(ImVec2(0.35f * GUIWidth, 0.505f * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
            ImGui::Begin(m_killAllButtonOkID.c_str(), nullptr, m_noBackgroundNoFrame);
            m_killAllButtonClicked = ImGui::ImageButton(m_killAllButtonOkID.c_str(),reinterpret_cast<ImTextureID>(m_killAllButtonOkTexture->getID()),
                                                          ImVec2(0.3f * GUIWidth, 0.07f * GUIHeight));
            ImGui::End();
        }

        if(m_winMenuShow)
        {
            ImGui::SetNextWindowPos(ImVec2(0.2f * GUIWidth, 0.25f * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
            ImGui::Begin(m_winMenuID.c_str(), nullptr, m_noBackgroundNoFrame);
            ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
            ImGui::Image(reinterpret_cast<ImTextureID>(m_winTexture->getID()),
                         ImVec2(0.6f * GUIWidth, 0.25f * GUIHeight));

            ImGui::SetCursorPos(ImVec2(0.0f * GUIWidth, 0.255f * GUIHeight));
            m_winPrize1ButtonClicked = ImGui::ImageButton(m_winPrize1ButtonID.c_str(),reinterpret_cast<ImTextureID>(m_winPrize1ButtonTexture->getID()),
                                                              ImVec2(0.295f * GUIWidth, 0.07f * GUIHeight));

            ImGui::SetCursorPos(ImVec2(0.31f * GUIWidth, 0.255f * GUIHeight));
            m_winPrize2ButtonClicked = ImGui::ImageButton(m_winPrize2ButtonID.c_str(), reinterpret_cast<ImTextureID>(m_winPrize2ButtonTexture->getID()),
                                                                    ImVec2(0.295f * GUIWidth, 0.07f * GUIHeight));

            ImGui::End();
        }

        if(m_loseMenuShow)
        {
            ImGui::SetNextWindowPos(ImVec2(0.2f * GUIWidth, 0.25f * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
            ImGui::Begin(m_loseTextureID.c_str(), nullptr, m_noBackgroundNoFrame);
            ImGui::Image(reinterpret_cast<ImTextureID>(m_loseTexture->getID()),
                         ImVec2(0.6f * GUIWidth, 0.25f * GUIHeight));
            ImGui::End();
        }

        if(m_tankWithCommanderMenuShow)
        {
            ImGui::SetNextWindowPos(ImVec2(0.2f * GUIWidth, 0.25f * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
            ImGui::Begin(m_tankWithCommanderTextureID.c_str(), nullptr, m_noBackgroundNoFrame);
            ImGui::Image(reinterpret_cast<ImTextureID>(m_tankWithCommanderTexture->getID()),
                         ImVec2(0.6f * GUIWidth, 0.25f * GUIHeight));
            ImGui::End();

            ImGui::SetNextWindowPos(ImVec2(0.35f * GUIWidth, 0.505f * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
            ImGui::Begin(m_tankWithCommanderButtonOkID.c_str(), nullptr, m_noBackgroundNoFrame);
            m_tankWithCommanderButtonClicked = ImGui::ImageButton(m_tankWithCommanderButtonOkID.c_str(),reinterpret_cast<ImTextureID>(m_tankWithCommanderButtonOkTexture->getID()),
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
        m_exitButtonEnabled = true;
        m_exitButtonTop = 0.65f;
        m_exitButtonLeft = 0.35f;

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
