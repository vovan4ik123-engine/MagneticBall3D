#include "PlayStateGUILayer.h"
#include "EnumsAndVariables.h"
#include "GameStateHelper.h"
#include "DataBaseHelper.h"

namespace MagneticBall3D
{
    PlayStateGUILayer::PlayStateGUILayer()
    {
        m_ID = Beryll::LayerID::PLAY_GUI;

        const float screenAR = Beryll::MainImGUI::getInstance()->getGUIScreenAspectRation();

        m_statistics1 = std::make_shared<Beryll::Text>("Frame: 00000  FPS: 00000", EnAndVars::FontsPath::ROBOTO, 0.025f, 0, 0, 0.5f, 0.03f);
        m_guiObjects.push_back(m_statistics1);
        m_statistics2 = std::make_shared<Beryll::Text>("Phys: 00000  Logic: 00000  GPU: 00000", EnAndVars::FontsPath::ROBOTO, 0.025f, 0, 0.025f, 0.7f, 0.03f);
        m_guiObjects.push_back(m_statistics2);
        m_swipeCount = std::make_shared<Beryll::Text>("Swipe: 0000 Time: 00000", EnAndVars::FontsPath::ROBOTO, 0.02f, 0, 0.0475f, 0.45f, 0.025f);
        m_guiObjects.push_back(m_swipeCount);

        //m_releaseTimer = std::make_shared<Beryll::Text>("00:00", EnAndVars::FontsPath::ROBOTO, 0.03f, 0.424f, 0.0f, 0.2f, 0.033f);
        //m_guiObjects.push_back(m_releaseTimer);

//        sliderImpulse = std::make_shared<Beryll::SliderHorizontal>("impulse", EnumsAndVariables::FontsPath::ROBOTO, );
//        m_guiObjects.push_back(sliderImpulse);
//        sliderImpulse->setValue(0.15f);
//
//        sliderTorque = std::make_shared<Beryll::SliderHorizontal>("torque", EnumsAndVariables::FontsPath::ROBOTO, );
//        m_guiObjects.push_back(sliderTorque);
//        sliderTorque->setValue(0.1f);

//        sliderAmbient = std::make_shared<Beryll::SliderHorizontal>("ambient", EnAndVars::FontsPath::ROBOTO, 0.02f, 0.02f, 0.07f, 0.4f, 0.02f, 0, 1);
//        m_guiObjects.push_back(sliderAmbient);
//        sliderAmbient->setValue(0.7f);
//
        sliderSpecularPower = std::make_shared<Beryll::SliderHorizontal>("specular power", EnAndVars::FontsPath::ROBOTO, 0.02f, 0.02f, 0.07f, 0.4f, 0.02f, 0, 2);
        m_guiObjects.push_back(sliderSpecularPower);
        sliderSpecularPower->setValue(1.0f);

        progressBarHP = std::make_shared<Beryll::ProgressBar>(-0.02f, 0.956f, 1.04f, 0.025f);
        m_guiObjects.push_back(progressBarHP);
        progressBarHP->setFontColor(0.0f, 0.0f, 0.0f, 0.0f);
        progressBarHP->setProgressColor(0.0f, 1.0f, 0.0f, 1.0f);
        progressBarHP->setBackgroundColor(1.0f, 0.0f, 0.0f, 1.0f);
        progressBarHP->setProgress(1.0f);

        progressBarXP = std::make_shared<Beryll::ProgressBar>( -0.02f, 0.976f, 1.04f, 0.03f);
        m_guiObjects.push_back(progressBarXP);
        progressBarXP->setFontColor(0.0f, 0.0f, 0.0f, 0.0f);
        progressBarXP->setProgressColor(0.0f, 0.0f, 1.0f, 1.0f);
        progressBarXP->setBackgroundColor(0.0f, 0.0f, 0.0f, 1.0f);
        progressBarXP->setProgress(0.0f);

        buttonExit = std::make_shared<Beryll::ButtonWithText>("Exit", EnAndVars::FontsPath::ROBOTO, 0.05f, 0.35f, 0.5f, 0.3f, 0.07f);
        m_guiObjects.push_back(buttonExit);
        buttonExit->disable();

        // Pause.
        buttonPause = std::make_shared<Beryll::ButtonWithTexture>("GUI/playState/PauseButton.jpg", "", 0.0f, 0.0f, 0.11f, 0.045f);
        m_guiObjects.push_back(buttonPause);
        buttonPauseResume = std::make_shared<Beryll::ButtonWithText>("Resume", EnAndVars::FontsPath::ROBOTO, 0.05f, 0.25f, 0.35f, 0.5f, 0.1f);
        m_guiObjects.push_back(buttonPauseResume);
        buttonPauseResume->disable();

        // Resurrect.
        textureResurrect = std::make_shared<Beryll::GUITexture>("GUI/playState/CanResurrect.jpg", 0.2f, 0.25f, 0.6f, 0.25f);
        m_guiObjects.push_back(textureResurrect);
        textureResurrect->disable();
        buttonResurrectOk = std::make_shared<Beryll::ButtonWithText>("Ok", EnAndVars::FontsPath::ROBOTO, 0.05f, 0.5f, 0.5f, 0.3f, 0.07f);
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
        buttonKillAllOk = std::make_shared<Beryll::ButtonWithText>("Ok", EnAndVars::FontsPath::ROBOTO, 0.05f, 0.35f, 0.5f, 0.3f, 0.07f);
        m_guiObjects.push_back(buttonKillAllOk);
        buttonKillAllOk->disable();

        // Win.
        textureWin = std::make_shared<Beryll::GUITexture>("GUI/playState/Win.jpg", 0.2f, 0.25f, 0.6f, 0.25f);
        m_guiObjects.push_back(textureWin);
        textureWin->disable();
        buttonWinOk = std::make_shared<Beryll::ButtonWithText>("Ok", EnAndVars::FontsPath::ROBOTO, 0.05f, 0.35f, 0.5f, 0.3f, 0.07f);
        m_guiObjects.push_back(buttonWinOk);
        buttonWinOk->disable();

        // Menus before specific bosses.
        // Tank with commander.
        textureTankWithCommander = std::make_shared<Beryll::GUITexture>("GUI/playState/BossTankWithCommander.jpg", 0.2f, 0.25f, 0.6f, 0.25f);
        m_guiObjects.push_back(textureTankWithCommander);
        textureTankWithCommander->disable();
        buttonTankWithCommanderOk = std::make_shared<Beryll::ButtonWithText>("Ok", EnAndVars::FontsPath::ROBOTO, 0.05f, 0.35f, 0.5f, 0.3f, 0.07f);
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

            //BR_INFO("FPS: %f", Beryll::GameLoop::getFPS());

//            int min = int(EnAndVars::mapPlayTimeSec / 60.0f);
//            int sec = int(std::fmod(EnAndVars::mapPlayTimeSec, 60.0f));
//            if(min < 10)
//                stream << "0";
//
//            stream << min << ":";
//
//            if(sec < 10)
//                stream << "0";
//
//            stream << sec;
//
//            m_releaseTimer->text = stream.str();

            m_statisticsUpdateTime = Beryll::TimeStep::getMilliSecFromStart();
        }


        // Only for buttons.
        if(m_timeAppearsOnScreen + m_delayBeforeCanBeClicked > Beryll::TimeStep::getSecFromStart())
            return;

        if(buttonPause->getIsPressed() && !EnAndVars::gameOnPause)
        {
            GameStateHelper::pauseGame();
            buttonPauseResume->enable();
            buttonExit->leftPos = 0.35f;
            buttonExit->enable();
        }
        else if(buttonPauseResume->getIsPressed())
        {
            GameStateHelper::resumeGame();
            buttonPauseResume->disable();
            buttonExit->disable();
        }
        else if(buttonExit->getIsPressed())
        {
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
            EnAndVars::CurrencyBalance::crystals += EnAndVars::mapWinCrystalsPrize;
            DataBaseHelper::storeCurrencyBalanceCrystals(EnAndVars::CurrencyBalance::crystals);

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
            buttonExit->disable();
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
    }

    void PlayStateGUILayer::showMenuResurrect()
    {
        if(textureResurrect->getIsEnabled())
            return;

        GameStateHelper::pauseGame();
        textureResurrect->enable();
        buttonResurrectOk->enable();
        buttonExit->leftPos = 0.2f;
        buttonExit->enable();

        m_timeAppearsOnScreen = Beryll::TimeStep::getSecFromStart();
    }

    void PlayStateGUILayer::showMenuResurrectNoCrystals()
    {
        if(textureResurrectNoCrystals->getIsEnabled())
            return;

        GameStateHelper::pauseGame();
        textureResurrectNoCrystals->enable();
        buttonExit->leftPos = 0.35f;
        buttonExit->enable();

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
        buttonExit->leftPos = 0.35f;
        buttonExit->enable();

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
