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
    const std::string PlayStateGUILayer::m_buttonPauseID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_buttonResumeID = std::to_string(BeryllUtils::Common::generateID());
    const std::string PlayStateGUILayer::m_buttonExitID = std::to_string(BeryllUtils::Common::generateID());

    PlayStateGUILayer::PlayStateGUILayer()
    {
        m_ID = Beryll::LayerID::PLAY_GUI;

        const float screenAR = Beryll::MainImGUI::getInstance()->getGUIScreenAspectRation();

//        m_statistics1 = std::make_shared<Beryll::Text>("Frame: 00000  FPS: 00000", EnAndVars::FontsPath::roboto, 0.025f, 0, 0, 0.5f, 0.03f);
//        m_guiObjects.push_back(m_statistics1);
//        m_statistics2 = std::make_shared<Beryll::Text>("Phys: 00000  Logic: 00000  GPU: 00000", EnAndVars::FontsPath::roboto, 0.025f, 0, 0.025f, 0.7f, 0.03f);
//        m_guiObjects.push_back(m_statistics2);
//        m_swipeCount = std::make_shared<Beryll::Text>("Swipe: 0000 Time: 00000", EnAndVars::FontsPath::roboto, 0.02f, 0, 0.0475f, 0.45f, 0.025f);
//        m_guiObjects.push_back(m_swipeCount);

//        sliderAmbient = std::make_shared<Beryll::SliderHorizontal>("ambient", EnAndVars::FontsPath::roboto, 0.02f, 0.02f, 0.07f, 0.4f, 0.02f, 0, 1);
//        m_guiObjects.push_back(sliderAmbient);
//        sliderAmbient->setValue(0.7f);
//
//        sliderSpecularPower = std::make_shared<Beryll::SliderHorizontal>("specular power", EnAndVars::FontsPath::roboto, 0.02f, 0.02f, 0.1f, 0.4f, 0.02f, 0, 2);
//        m_guiObjects.push_back(sliderSpecularPower);
//        sliderSpecularPower->setValue(1.0f);
//
//        sliderSunPower = std::make_shared<Beryll::SliderHorizontal>("sun power", EnAndVars::FontsPath::roboto, 0.02f, 0.02f, 0.13f, 0.4f, 0.02f, 0, 1);
//        m_guiObjects.push_back(sliderSunPower);
//        sliderSunPower->setValue(0.5f);

//        progressBarHP = std::make_shared<Beryll::ProgressBar>(-0.02f, 0.956f, 1.04f, 0.025f);
//        m_guiObjects.push_back(progressBarHP);
//        ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
//        progressBarHP->setProgressColor(0.0f, 1.0f, 0.0f, 1.0f);
//        progressBarHP->setBackgroundColor(1.0f, 0.0f, 0.0f, 1.0f);
//        progressBarHP->setProgress(1.0f);
//
//        progressBarXP = std::make_shared<Beryll::ProgressBar>( -0.02f, 0.976f, 1.04f, 0.03f);
//        m_guiObjects.push_back(progressBarXP);
//        progressBarXP->setFontColor(0.0f, 0.0f, 0.0f, 0.0f);
//        progressBarXP->setProgressColor(0.0f, 0.0f, 1.0f, 1.0f);
//        progressBarXP->setBackgroundColor(0.0f, 0.0f, 0.0f, 1.0f);
//        progressBarXP->setProgress(0.0f);

        m_fontMapPlayTimer = Beryll::MainImGUI::getInstance()->createFont(EnAndVars::FontsPath::roboto, 0.03f);

        m_buttonPauseTexture = Beryll::Renderer::createTexture("GUI/playState/Pause.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_buttonResumeTexture = Beryll::Renderer::createTexture("GUI/playState/Resume.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_buttonExitTexture = Beryll::Renderer::createTexture("GUI/Exit.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);

        // Map0Tutorial.
        textureTutorialSwipe = std::make_shared<Beryll::GUITexture>("GUI/playState/TutorialSwipeToMove.png", 0.093f, 0.525f, 0.8f, 0.39f);
        m_guiObjects.push_back(textureTutorialSwipe);
        textureTutorialSwipe->disable();
        textureTutorialSwipeFaster = std::make_shared<Beryll::GUITexture>("GUI/playState/TutorialSwipeFaster.png", 0.1f, 0.0f, 0.8f, 0.1f);
        m_guiObjects.push_back(textureTutorialSwipeFaster);
        textureTutorialSwipeFaster->disable();
        textureTutorialSwipeOnBuilding = std::make_shared<Beryll::GUITexture>("GUI/playState/TutorialSwipeOnBuilding.png", 0.1f, 0.0f, 0.8f, 0.15f);
        m_guiObjects.push_back(textureTutorialSwipeOnBuilding);
        textureTutorialSwipeOnBuilding->disable();

        // Pause.
        //buttonPause = std::make_shared<Beryll::ButtonWithTexture>("GUI/playState/Pause.jpg", "", 0.0f, 0.0f, 0.1f, 0.045f);
        //buttonPauseResume = std::make_shared<Beryll::ButtonWithText>("Resume", EnAndVars::FontsPath::roboto, 0.05f, 0.25f, 0.35f, 0.5f, 0.1f);


        // Resurrect.
        textureResurrect = std::make_shared<Beryll::GUITexture>("GUI/playState/CanResurrect.jpg", 0.2f, 0.25f, 0.6f, 0.25f);
        m_guiObjects.push_back(textureResurrect);
        textureResurrect->disable();
        buttonResurrectOk = std::make_shared<Beryll::ButtonWithText>("Ok", EnAndVars::FontsPath::roboto, 0.05f, 0.5f, 0.5f, 0.3f, 0.07f);
        m_guiObjects.push_back(buttonResurrectOk);
        buttonResurrectOk->disable();

        // Resurrect no crystals.
        textureResurrectNoCrystals = std::make_shared<Beryll::GUITexture>("GUI/playState/ResurrectNoCrystals.jpg", 0.2f, 0.25f, 0.6f, 0.25f);
        m_guiObjects.push_back(textureResurrectNoCrystals);
        textureResurrectNoCrystals->disable();

        // Lose.
        textureLose = std::make_shared<Beryll::GUITexture>("GUI/playState/Lose.jpg", 0.2f, 0.25f, 0.6f, 0.25f);
        m_guiObjects.push_back(textureLose);
        textureLose->disable();

        // Kill all enemies before boss.
        textureKillAll = std::make_shared<Beryll::GUITexture>("GUI/playState/KillAllEnemiesToSpawnBoss.jpg", 0.2f, 0.25f, 0.6f, 0.25f);
        m_guiObjects.push_back(textureKillAll);
        textureKillAll->disable();
        buttonKillAllOk = std::make_shared<Beryll::ButtonWithText>("Ok", EnAndVars::FontsPath::roboto, 0.05f, 0.35f, 0.5f, 0.3f, 0.07f);
        m_guiObjects.push_back(buttonKillAllOk);
        buttonKillAllOk->disable();

        // Win.
        textureWin = std::make_shared<Beryll::GUITexture>("GUI/playState/Win.jpg", 0.2f, 0.25f, 0.6f, 0.25f);
        m_guiObjects.push_back(textureWin);
        textureWin->disable();
        buttonWinOk = std::make_shared<Beryll::ButtonWithText>("Ok", EnAndVars::FontsPath::roboto, 0.05f, 0.35f, 0.5f, 0.3f, 0.07f);
        m_guiObjects.push_back(buttonWinOk);
        buttonWinOk->disable();

        // Menus before specific bosses.
        // Tank with commander.
        textureTankWithCommander = std::make_shared<Beryll::GUITexture>("GUI/playState/BossTankWithCommander.jpg", 0.2f, 0.25f, 0.6f, 0.25f);
        m_guiObjects.push_back(textureTankWithCommander);
        textureTankWithCommander->disable();
        buttonTankWithCommanderOk = std::make_shared<Beryll::ButtonWithText>("Ok", EnAndVars::FontsPath::roboto, 0.05f, 0.35f, 0.5f, 0.3f, 0.07f);
        m_guiObjects.push_back(buttonTankWithCommanderOk);
        buttonTankWithCommanderOk->disable();
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

        if(Beryll::TimeStep::getMilliSecFromStart() > m_statisticsUpdateTime + 200) // Update every 200 ms.
        {
            std::stringstream stream;
//            stream << std::fixed << std::setprecision(1);
//            stream << "Frame: " << Beryll::GameLoop::getFrameTime() << "  FPS: " << Beryll::GameLoop::getFPS();
//            m_statistics1->text = stream.str();
//
//            stream.str(""); // Way to clear std::stringstream.
//            stream << std::fixed << std::setprecision(1);
//            stream << "Phys: " << Beryll::Physics::getSimulationTime();
//            stream << "  Logic: " << (Beryll::GameLoop::getCPUTime() - Beryll::Physics::getSimulationTime());
//            stream << "  GPU: " << Beryll::GameLoop::getGPUTime();
//            m_statistics2->text = stream.str();
//
//            stream.str("");
//            stream << "Swipe: " << EnAndVars::mapSwipeCount;
//            stream << "  Time: " << int(EnAndVars::mapPlayTimeSec / 60.0f) << ":" << int(std::fmod(EnAndVars::mapPlayTimeSec, 60.0f));
//            m_swipeCount->text = stream.str();
//            stream.str("");

            //BR_INFO("FPS: %f", Beryll::GameLoop::getFPS());

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
                m_buttonExitLeft = 0.35f;
                m_buttonExitEnabled = true;
            }
        }
        else if(m_buttonResumeClicked)
        {
            m_buttonResumeClicked = false;

            GameStateHelper::resumeGame();
            m_buttonResumeEnabled = false;
            m_buttonExitEnabled = false;
        }
        else if(m_buttonExitClicked)
        {
            m_buttonExitClicked = false;

            // Pause game before exit to avoid update scene layer.
            GameStateHelper::pauseGame();

            GameStateHelper::popState();
            GameStateHelper::pushStartMenuState();
            return;
        }
        else if(buttonKillAllOk->getIsPressed())
        {
            GameStateHelper::resumeGame();
            textureKillAll->disable();
            buttonKillAllOk->disable();
        }
        else if(buttonWinOk->getIsPressed())
        {
            // Pause game before exit to avoid update scene layer.
            GameStateHelper::pauseGame();

            GameStateHelper::popState();
            GameStateHelper::pushStartMenuState();
            return;
        }
        else if(buttonTankWithCommanderOk->getIsPressed())
        {
            GameStateHelper::resumeGame();
            textureTankWithCommander->disable();
            buttonTankWithCommanderOk->disable();
        }
        else if(buttonResurrectOk->getIsPressed())
        {
            resurrectButtonPressed = true;

            GameStateHelper::resumeGame();
            textureResurrect->disable();
            buttonResurrectOk->disable();
            m_buttonExitEnabled = false;
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

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 0.0f, 0.0f, 0.0f, 1.0f });
            ImGui::SetNextWindowPos(ImVec2(0.42f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.0f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
            ImGui::Begin(m_mapPlayTimerID.c_str(), nullptr, m_noBackgroundNoFrame);
            ImGui::PushFont(m_fontMapPlayTimer);
            ImGui::Text("%s", m_textMapPlayTimer.c_str()); // ImGUI ignores "%s". Modify void ImFormatStringToTempBufferV( to avoid that.
            ImGui::PopFont();
            ImGui::End();
            ImGui::PopStyleColor(1);
        }

        // HP bar.
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0.0f, 0.0f, 0.0f, 0.0f});
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4{0.0f, 1.0f, 0.0f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{1.0f, 0.0f, 0.0f, 1.0f});
        ImGui::SetNextWindowPos(ImVec2(-0.02f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.956f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
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

        // Button pause.
        ImGui::SetNextWindowPos(ImVec2(0.0f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.0f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.

        ImGui::Begin(m_buttonPauseID.c_str(), nullptr, m_noBackgroundNoFrame);

        m_buttonPauseClicked = ImGui::ImageButton(m_buttonPauseID.c_str(),reinterpret_cast<ImTextureID>(m_buttonPauseTexture->getID()),
                                                 ImVec2(0.1f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.045f * Beryll::MainImGUI::getInstance()->getGUIHeight()));

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
    }

    void PlayStateGUILayer::showMenuResurrect()
    {
        if(textureResurrect->getIsEnabled())
            return;

        GameStateHelper::pauseGame();
        textureResurrect->enable();
        buttonResurrectOk->enable();
        m_buttonExitLeft = 0.2f;
        m_buttonExitEnabled = true;

        m_timeAppearsOnScreen = Beryll::TimeStep::getSecFromStart();
    }

    void PlayStateGUILayer::showMenuResurrectNoCrystals()
    {
        if(textureResurrectNoCrystals->getIsEnabled())
            return;

        GameStateHelper::pauseGame();
        textureResurrectNoCrystals->enable();
        m_buttonExitLeft = 0.35f;
        m_buttonExitEnabled = true;

        m_timeAppearsOnScreen = Beryll::TimeStep::getSecFromStart();
    }

    void PlayStateGUILayer::showMenuKillAllBeforeBoss()
    {
        if(textureKillAll->getIsEnabled())
            return;

        GameStateHelper::pauseGame();
        textureKillAll->enable();
        buttonKillAllOk->enable();

        m_timeAppearsOnScreen = Beryll::TimeStep::getSecFromStart();
    }

    void PlayStateGUILayer::showMenuLose()
    {
        if(textureLose->getIsEnabled())
            return;

        GameStateHelper::pauseGame();
        textureLose->enable();
        m_buttonExitLeft = 0.35f;
        m_buttonExitEnabled = true;

        m_timeAppearsOnScreen = Beryll::TimeStep::getSecFromStart();
    }

    void PlayStateGUILayer::showMenuWin()
    {
        if(textureWin->getIsEnabled())
            return;

        GameStateHelper::pauseGame();
        textureWin->enable();
        buttonWinOk->enable();

        m_timeAppearsOnScreen = Beryll::TimeStep::getSecFromStart();
    }

    void PlayStateGUILayer::showMenuBossTankWithCommander()
    {
        if(textureTankWithCommander->getIsEnabled())
            return;

        GameStateHelper::pauseGame();
        textureTankWithCommander->enable();
        buttonTankWithCommanderOk->enable();

        m_timeAppearsOnScreen = Beryll::TimeStep::getSecFromStart();
    }
}
