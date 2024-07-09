#include "PlayStateGUILayer.h"
#include "EnumsAndVariables.h"
#include "GameStateHelper.h"
#include "DataBaseHelper.h"

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
    const std::string PlayStateGUILayer::m_buttonPauseID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_buttonResumeID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_buttonExitID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_tutorialSwipeID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_tutorialHowToSwipeID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_tutorialSwipeOnWallID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_resurrectTextureID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_resurrectButtonOkID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_resurrectNoCrystalsTextureID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_loseTextureID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_killAllTextureID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_killAllButtonOkID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_winTextureID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_winButtonOkID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_tankWithCommanderTextureID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_tankWithCommanderButtonOkID = std::to_string(BeryllUtils::Common::generateID());


    PlayStateGUILayer::PlayStateGUILayer()
    {
        m_ID = Beryll::LayerID::PLAY_GUI;

        const float screenAR = Beryll::MainImGUI::getInstance()->getGUIScreenAspectRation();

        if(m_showStatistics)
        {
            m_statistics1 = std::make_shared<Beryll::Text>("Frame: 00000  FPS: 00000", EnAndVars::FontsPath::roboto, 0.025f, 0.15f, 0, 0.55f, 0.03f);
            m_guiObjects.push_back(m_statistics1);
            m_statistics2 = std::make_shared<Beryll::Text>("Phys: 00000  Logic: 00000  GPU: 00000", EnAndVars::FontsPath::roboto, 0.025f, 0.15f, 0.025f, 0.75f, 0.03f);
            m_guiObjects.push_back(m_statistics2);
            m_swipeCount = std::make_shared<Beryll::Text>("Swipe: 0000 Time: 00000", EnAndVars::FontsPath::roboto, 0.02f, 0.15f, 0.0475f, 0.5f, 0.025f);
            m_guiObjects.push_back(m_swipeCount);
        }

//        sliderAmbient = std::make_shared<Beryll::SliderHorizontal>("ambient", EnAndVars::FontsPath::roboto, 0.02f, 0.02f, 0.07f, 0.4f, 0.02f, 0, 1);
//        m_guiObjects.push_back(sliderAmbient);
//        sliderAmbient->setValue(0.5f);
//
//        sliderSpecularPower = std::make_shared<Beryll::SliderHorizontal>("specular power", EnAndVars::FontsPath::roboto, 0.02f, 0.02f, 0.1f, 0.4f, 0.02f, 0, 0.7f);
//        m_guiObjects.push_back(sliderSpecularPower);
//        sliderSpecularPower->setValue(0.2f);
//
//        sliderSunPower = std::make_shared<Beryll::SliderHorizontal>("sun power", EnAndVars::FontsPath::roboto, 0.02f, 0.02f, 0.13f, 0.4f, 0.02f, 0, 1);
//        m_guiObjects.push_back(sliderSunPower);
//        sliderSunPower->setValue(0.5f);

        m_fontMapPlayTimer = Beryll::MainImGUI::getInstance()->createFont(EnAndVars::FontsPath::roboto, 0.03f);
        m_fontSmashedCount = Beryll::MainImGUI::getInstance()->createFont(EnAndVars::FontsPath::roboto, 0.02f);
        m_fontSpeed = Beryll::MainImGUI::getInstance()->createFont(EnAndVars::FontsPath::roboto, 0.02f);

        m_smashedCountTexture = Beryll::Renderer::createTexture("GUI/playState/KilledIcon.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_speedTexture = Beryll::Renderer::createTexture("GUI/playState/SpeedIcon.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_buttonPauseTexture = Beryll::Renderer::createTexture("GUI/playState/Pause.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_buttonResumeTexture = Beryll::Renderer::createTexture("GUI/playState/Resume.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_buttonExitTexture = Beryll::Renderer::createTexture("GUI/Exit.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_tutorialSwipeTexture = Beryll::Renderer::createTexture("GUI/playState/TutorialSwipeToMove.png", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_tutorialHowToSwipeTexture = Beryll::Renderer::createTexture("GUI/playState/TutorialHowToSwipe.png", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_tutorialSwipeOnWallTexture = Beryll::Renderer::createTexture("GUI/playState/TutorialSwipeOnWall.png", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_resurrectTexture = Beryll::Renderer::createTexture("GUI/playState/CanResurrect.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_resurrectButtonOkTexture = Beryll::Renderer::createTexture("GUI/Ok.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_resurrectNoCrystalsTexture = Beryll::Renderer::createTexture("GUI/playState/ResurrectNoCrystals.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_loseTexture = Beryll::Renderer::createTexture("GUI/playState/Lose.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_killAllTexture = Beryll::Renderer::createTexture("GUI/playState/KillAllEnemiesToSpawnBoss.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_killAllButtonOkTexture = Beryll::Renderer::createTexture("GUI/Ok.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_winTexture = Beryll::Renderer::createTexture("GUI/playState/Win.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_winButtonOkTexture = Beryll::Renderer::createTexture("GUI/Ok.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_tankWithCommanderTexture = Beryll::Renderer::createTexture("GUI/playState/BossTankWithCommander.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_tankWithCommanderButtonOkTexture = Beryll::Renderer::createTexture("GUI/Ok.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
    }

    PlayStateGUILayer::~PlayStateGUILayer()
    {

    }

    void PlayStateGUILayer::updateBeforePhysics()
    {
        if(EnAndVars::improvementSystemOnScreen)
            return;

        for(const std::shared_ptr<Beryll::GUIObject>& go : m_guiObjects)
        {
            if(go->getIsEnabled())
            {
                go->updateBeforePhysics();
            }
        }

        if(m_showStatistics && Beryll::TimeStep::getMilliSecFromStart() > m_statisticsUpdateTime + 200) // Update every 200 ms.
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
            stream << "Swipe: " << EnAndVars::mapSwipeCount;
            stream << "  Time: " << int(EnAndVars::mapPlayTimeSec / 60.0f) << ":" << int(std::fmod(EnAndVars::mapPlayTimeSec, 60.0f));
            m_swipeCount->text = stream.str();
            stream.str("");

            m_statisticsUpdateTime = Beryll::TimeStep::getMilliSecFromStart();
        }


        // Only for buttons. Make delay to prevent accidental click when user swipe during gameplay.
        if(m_timeAppearsOnScreen + m_delayBeforeCanBeClicked > Beryll::TimeStep::getSecFromStart())
            return;

        if(m_buttonPauseClicked)
        {
            m_buttonPauseClicked = false;

            if(!EnAndVars::gameOnPause)
            {
                GameStateHelper::pauseGame();
                m_buttonResumeEnabled = true;
                m_buttonExitEnabled = true;
                m_buttonExitLeft = 0.35f;
            }
        }
        else if(m_buttonResumeClicked)
        {
            m_buttonResumeClicked = false;
            m_buttonResumeEnabled = false;
            m_buttonExitEnabled = false;

            GameStateHelper::resumeGame();
        }
        else if(m_buttonExitClicked)
        {
            m_buttonExitClicked = false;
            m_buttonExitEnabled = false;

            // Pause game before exit to avoid update scene layer.
            GameStateHelper::pauseGame();

            GameStateHelper::popState();
            GameStateHelper::pushStartMenuState();
            return;
        }
        else if(m_killAllButtonClicked)
        {
            m_killAllButtonClicked = false;
            m_menuKillAllEnabled = false;

            GameStateHelper::resumeGame();
        }
        else if(m_winButtonClicked)
        {
            m_winButtonClicked = false;
            m_menuWinEnabled = false;

            // Pause game before exit to avoid update scene layer.
            GameStateHelper::pauseGame();

            GameStateHelper::popState();
            GameStateHelper::pushStartMenuState();
            return;
        }
        else if(m_tankWithCommanderButtonClicked)
        {
            m_tankWithCommanderButtonClicked = false;
            m_menuTankWithCommanderEnabled = false;

            GameStateHelper::resumeGame();
        }
        else if(m_resurrectButtonClicked)
        {
            m_resurrectButtonClicked = false;
            m_menuResurrectEnabled = false;
            m_buttonExitEnabled = false;

            resurrectPlayer = true; // Will handled in BaseMap.cpp
            GameStateHelper::resumeGame();
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

        // HP bar.
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0.0f, 0.0f, 0.0f, 0.0f});
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4{0.0f, 1.0f, 0.0f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{1.0f, 0.0f, 0.0f, 1.0f});
        ImGui::SetNextWindowPos(ImVec2(-0.02f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.9565f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(1.04f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.025f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::Begin(m_progressBarHPID.c_str(), nullptr, m_noBackgroundNoFrame);
        ImGui::ProgressBar(progressBarHP);
        ImGui::End();
        ImGui::PopStyleColor(3);

        // XP bar.
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0.0f, 0.0f, 0.0f, 0.0f});
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4{0.0f, 0.0f, 1.0f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{0.0f, 0.0f, 0.0f, 1.0f});
        ImGui::SetNextWindowPos(ImVec2(-0.02f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.976f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(1.04f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.03f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::Begin(m_progressBarXPID.c_str(), nullptr, m_noBackgroundNoFrame);
        ImGui::ProgressBar(progressBarXP);
        ImGui::End();
        ImGui::PopStyleColor(3);

        // Text map timer.
        if(m_showMapPlayTimer)
        {
            m_textMapPlayTimer = "";
            int min = int(EnAndVars::mapPlayTimeSec / 60.0f);
            int sec = int(std::fmod(EnAndVars::mapPlayTimeSec, 60.0f));
            if(min < 10)
                m_textMapPlayTimer += "0";

            m_textMapPlayTimer += std::to_string(min);
            m_textMapPlayTimer += ":";

            if(sec < 10)
                m_textMapPlayTimer += "0";

            m_textMapPlayTimer += std::to_string(sec);

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0.0f, 0.0f, 0.0f, 1.0f });
            ImGui::SetNextWindowPos(ImVec2(0.42f * Beryll::MainImGUI::getInstance()->getGUIWidth(), -0.005f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
            ImGui::Begin(m_mapPlayTimerID.c_str(), nullptr, m_noBackgroundNoFrame);
            ImGui::PushFont(m_fontMapPlayTimer);
            ImGui::Text("%s", m_textMapPlayTimer.c_str()); // ImGUI ignores "%s". Modify void ImFormatStringToTempBufferV( to avoid that.
            ImGui::PopFont();
            ImGui::End();
            ImGui::PopStyleColor(1);
        }

        // Smashed count.
        ImGui::SetNextWindowPos(ImVec2(0.77f * Beryll::MainImGUI::getInstance()->getGUIWidth(), -0.005f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.
        ImGui::Begin(m_smashedCountTextureID.c_str(), nullptr, m_noBackgroundNoFrame | ImGuiWindowFlags_NoBringToFrontOnFocus);
        ImGui::Image(reinterpret_cast<ImTextureID>(m_smashedCountTexture->getID()),
                     ImVec2(0.26f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.03f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::End();

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0.0f, 0.0f, 0.0f, 1.0f });
        ImGui::SetNextWindowPos(ImVec2(0.84f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.002f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
        ImGui::Begin(m_smashedCountTextID.c_str(), nullptr, m_noBackgroundNoFrame);
        ImGui::PushFont(m_fontSmashedCount);
        ImGui::Text("%d", EnAndVars::enemiesKilledCount);
        ImGui::PopFont();
        ImGui::End();
        ImGui::PopStyleColor(1);

        // Speed.
        ImGui::SetNextWindowPos(ImVec2(0.77f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.025f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.
        ImGui::Begin(m_speedTextureID.c_str(), nullptr, m_noBackgroundNoFrame | ImGuiWindowFlags_NoBringToFrontOnFocus);
        ImGui::Image(reinterpret_cast<ImTextureID>(m_speedTexture->getID()),
                     ImVec2(0.26f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.03f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::End();

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0.0f, 0.0f, 0.0f, 1.0f });
        ImGui::SetNextWindowPos(ImVec2(0.84f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.032f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
        ImGui::Begin(m_speedTextID.c_str(), nullptr, m_noBackgroundNoFrame);
        ImGui::PushFont(m_fontSpeed);
        ImGui::Text("%d/%d", EnAndVars::playerCurrentSpeed, int(EnAndVars::playerMaxSpeedXZ));
        ImGui::PopFont();
        ImGui::End();
        ImGui::PopStyleColor(1);

        // Button pause.
        ImGui::SetNextWindowPos(ImVec2(-0.011f * Beryll::MainImGUI::getInstance()->getGUIWidth(), -0.005f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.
        ImGui::Begin(m_buttonPauseID.c_str(), nullptr, m_noBackgroundNoFrame);
        m_buttonPauseClicked = ImGui::ImageButton(m_buttonPauseID.c_str(),reinterpret_cast<ImTextureID>(m_buttonPauseTexture->getID()),
                                                 ImVec2(0.13f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.05f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::End();

        // Button resume.
        if(m_buttonResumeEnabled)
        {
            ImGui::SetNextWindowPos(ImVec2(0.25f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.35f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.
            ImGui::Begin(m_buttonResumeID.c_str(), nullptr, m_noBackgroundNoFrame);
            m_buttonResumeClicked = ImGui::ImageButton(m_buttonResumeID.c_str(),reinterpret_cast<ImTextureID>(m_buttonResumeTexture->getID()),
                                                     ImVec2(0.5f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.08f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::End();
        }

        // Button exit.
        if(m_buttonExitEnabled)
        {
            ImGui::SetNextWindowPos(ImVec2(m_buttonExitLeft * Beryll::MainImGUI::getInstance()->getGUIWidth(), m_buttonExitTop * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.
            ImGui::Begin(m_buttonExitID.c_str(), nullptr, m_noBackgroundNoFrame);
            m_buttonExitClicked = ImGui::ImageButton(m_buttonExitID.c_str(),reinterpret_cast<ImTextureID>(m_buttonExitTexture->getID()),
                                                     ImVec2(0.3f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.07f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::End();
        }

        // Map0Tutorial.
        if(tutorialSwipeEnabled)
        {
            ImGui::SetNextWindowPos(ImVec2(0.093f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.48f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.
            ImGui::Begin(m_tutorialSwipeID.c_str(), nullptr, m_noBackgroundNoFrame);
            ImGui::Image(reinterpret_cast<ImTextureID>(m_tutorialSwipeTexture->getID()),
                         ImVec2(0.8f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.435f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::End();
        }

        if(tutorialHowToSwipeEnabled)
        {
            ImGui::SetNextWindowPos(ImVec2(0.12f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.07f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.
            ImGui::Begin(m_tutorialHowToSwipeID.c_str(), nullptr, m_noBackgroundNoFrame);
            ImGui::Image(reinterpret_cast<ImTextureID>(m_tutorialHowToSwipeTexture->getID()),
                         ImVec2(0.76f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.14f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::End();
        }

        if(tutorialSwipeOnWallEnabled)
        {
            ImGui::SetNextWindowPos(ImVec2(0.12f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.07f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.
            ImGui::Begin(m_tutorialSwipeOnWallID.c_str(), nullptr, m_noBackgroundNoFrame);
            ImGui::Image(reinterpret_cast<ImTextureID>(m_tutorialSwipeOnWallTexture->getID()),
                         ImVec2(0.76f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.08f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::End();
        }

        if(m_menuResurrectEnabled)
        {
            ImGui::SetNextWindowPos(ImVec2(0.2f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.25f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
            ImGui::Begin(m_resurrectTextureID.c_str(), nullptr, m_noBackgroundNoFrame);
            ImGui::Image(reinterpret_cast<ImTextureID>(m_resurrectTexture->getID()),
                         ImVec2(0.6f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.25f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::End();

            ImGui::SetNextWindowPos(ImVec2(0.5f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.5f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
            ImGui::Begin(m_resurrectButtonOkID.c_str(), nullptr, m_noBackgroundNoFrame);
            m_resurrectButtonClicked = ImGui::ImageButton(m_resurrectButtonOkID.c_str(),reinterpret_cast<ImTextureID>(m_resurrectButtonOkTexture->getID()),
                                                          ImVec2(0.3f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.07f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::End();
        }

        if(m_menuResurrectNoCrystalsEnabled)
        {
            ImGui::SetNextWindowPos(ImVec2(0.2f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.25f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
            ImGui::Begin(m_resurrectNoCrystalsTextureID.c_str(), nullptr, m_noBackgroundNoFrame);
            ImGui::Image(reinterpret_cast<ImTextureID>(m_resurrectNoCrystalsTexture->getID()),
                         ImVec2(0.6f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.25f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::End();
        }

        if(m_menuLoseEnabled)
        {
            ImGui::SetNextWindowPos(ImVec2(0.2f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.25f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
            ImGui::Begin(m_loseTextureID.c_str(), nullptr, m_noBackgroundNoFrame);
            ImGui::Image(reinterpret_cast<ImTextureID>(m_loseTexture->getID()),
                         ImVec2(0.6f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.25f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::End();
        }

        if(m_menuKillAllEnabled)
        {
            ImGui::SetNextWindowPos(ImVec2(0.2f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.25f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
            ImGui::Begin(m_killAllTextureID.c_str(), nullptr, m_noBackgroundNoFrame);
            ImGui::Image(reinterpret_cast<ImTextureID>(m_killAllTexture->getID()),
                         ImVec2(0.6f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.25f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::End();

            ImGui::SetNextWindowPos(ImVec2(0.35f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.5f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
            ImGui::Begin(m_killAllButtonOkID.c_str(), nullptr, m_noBackgroundNoFrame);
            m_killAllButtonClicked = ImGui::ImageButton(m_killAllButtonOkID.c_str(),reinterpret_cast<ImTextureID>(m_killAllButtonOkTexture->getID()),
                                                          ImVec2(0.3f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.07f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::End();
        }

        if(m_menuWinEnabled)
        {
            ImGui::SetNextWindowPos(ImVec2(0.2f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.25f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
            ImGui::Begin(m_winTextureID.c_str(), nullptr, m_noBackgroundNoFrame);
            ImGui::Image(reinterpret_cast<ImTextureID>(m_winTexture->getID()),
                         ImVec2(0.6f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.25f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::End();

            ImGui::SetNextWindowPos(ImVec2(0.35f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.5f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
            ImGui::Begin(m_winButtonOkID.c_str(), nullptr, m_noBackgroundNoFrame);
            m_winButtonClicked = ImGui::ImageButton(m_winButtonOkID.c_str(),reinterpret_cast<ImTextureID>(m_winButtonOkTexture->getID()),
                                                    ImVec2(0.3f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.07f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::End();
        }

        if(m_menuTankWithCommanderEnabled)
        {
            ImGui::SetNextWindowPos(ImVec2(0.2f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.25f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
            ImGui::Begin(m_tankWithCommanderTextureID.c_str(), nullptr, m_noBackgroundNoFrame);
            ImGui::Image(reinterpret_cast<ImTextureID>(m_tankWithCommanderTexture->getID()),
                         ImVec2(0.6f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.25f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::End();

            ImGui::SetNextWindowPos(ImVec2(0.35f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.5f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
            ImGui::Begin(m_tankWithCommanderButtonOkID.c_str(), nullptr, m_noBackgroundNoFrame);
            m_tankWithCommanderButtonClicked = ImGui::ImageButton(m_tankWithCommanderButtonOkID.c_str(),reinterpret_cast<ImTextureID>(m_tankWithCommanderButtonOkTexture->getID()),
                                                                  ImVec2(0.3f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.07f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::End();
        }
    }

    void PlayStateGUILayer::showMenuResurrect()
    {
        if(m_menuResurrectEnabled)
            return;

        m_menuResurrectEnabled = true;
        m_buttonExitEnabled = true;
        m_buttonExitLeft = 0.2f;

        GameStateHelper::pauseGame();

        m_timeAppearsOnScreen = Beryll::TimeStep::getSecFromStart();
    }

    void PlayStateGUILayer::showMenuResurrectNoCrystals()
    {
        if(m_menuResurrectNoCrystalsEnabled)
            return;

        m_menuResurrectNoCrystalsEnabled = true;
        m_buttonExitEnabled = true;
        m_buttonExitLeft = 0.35f;

        GameStateHelper::pauseGame();

        m_timeAppearsOnScreen = Beryll::TimeStep::getSecFromStart();
    }

    void PlayStateGUILayer::showMenuKillAllBeforeBoss()
    {
        if(m_menuKillAllEnabled)
            return;

        m_menuKillAllEnabled = true;

        GameStateHelper::pauseGame();

        m_timeAppearsOnScreen = Beryll::TimeStep::getSecFromStart();
    }

    void PlayStateGUILayer::showMenuLose()
    {
        if(m_menuLoseEnabled)
            return;

        m_menuLoseEnabled = true;
        m_buttonExitEnabled = true;
        m_buttonExitLeft = 0.35f;

        GameStateHelper::pauseGame();

        m_timeAppearsOnScreen = Beryll::TimeStep::getSecFromStart();
    }

    void PlayStateGUILayer::showMenuWin()
    {
        if(m_menuWinEnabled)
            return;

        m_menuWinEnabled = true;

        GameStateHelper::pauseGame();

        m_timeAppearsOnScreen = Beryll::TimeStep::getSecFromStart();
    }

    void PlayStateGUILayer::showMenuBossTankWithCommander()
    {
        if(m_menuTankWithCommanderEnabled)
            return;

        m_menuTankWithCommanderEnabled = true;

        GameStateHelper::pauseGame();

        m_timeAppearsOnScreen = Beryll::TimeStep::getSecFromStart();
    }
}
