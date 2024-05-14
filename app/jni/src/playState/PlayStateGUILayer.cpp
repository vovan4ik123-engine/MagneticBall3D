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

        m_statistics1 = std::make_shared<Beryll::Text>("Frame: 00000  FPS: 00000", EnAndVars::FontsPath::ROBOTO, 2.5f, 0, 0, 50, 3);
        m_guiObjects.push_back(m_statistics1);
        m_statistics2 = std::make_shared<Beryll::Text>("Phys: 00000  Logic: 00000  GPU: 00000", EnAndVars::FontsPath::ROBOTO, 2.5f, 0, 2.5f, 70, 3);
        m_guiObjects.push_back(m_statistics2);
        m_swipeCount = std::make_shared<Beryll::Text>("Swipe: 0000 Time: 00000", EnAndVars::FontsPath::ROBOTO, 2.0f, 0, 4.75f, 45, 2.5f);
        m_guiObjects.push_back(m_swipeCount);

//        sliderImpulse = std::make_shared<Beryll::SliderHorizontal>("impulse", EnumsAndVariables::FontsPath::ROBOTO, 2, 2, 6, 40, 2, 0, 1);
//        m_guiObjects.push_back(sliderImpulse);
//        sliderImpulse->setValue(0.15f);
//
//        sliderTorque = std::make_shared<Beryll::SliderHorizontal>("torque", EnumsAndVariables::FontsPath::ROBOTO, 2, 2, 9, 40, 2, 0, 1);
//        m_guiObjects.push_back(sliderTorque);
//        sliderTorque->setValue(0.1f);

        sliderAmbient = std::make_shared<Beryll::SliderHorizontal>("ambient", EnAndVars::FontsPath::ROBOTO, 2, 2, 7, 40, 2, 0, 1);
        m_guiObjects.push_back(sliderAmbient);
        sliderAmbient->setValue(0.7f);

//        sliderSunPower = std::make_shared<Beryll::SliderHorizontal>("sun power", EnAndVars::FontsPath::ROBOTO, 2, 2, 10, 40, 2, 0, 1);
//        m_guiObjects.push_back(sliderSunPower);
//        sliderSunPower->setValue(1.0f);
//
//        sliderSpecularPower = std::make_shared<Beryll::SliderHorizontal>("specular power", EnAndVars::FontsPath::ROBOTO, 2, 2, 13, 40, 2, 0, 2);
//        m_guiObjects.push_back(sliderSpecularPower);
//        sliderSpecularPower->setValue(1.0f);

        progressBarHP = std::make_shared<Beryll::ProgressBar>(-1, 95.6f, 102, 2.5f);
        m_guiObjects.push_back(progressBarHP);
        progressBarHP->setFontColor(0.0f, 0.0f, 0.0f, 0.0f);
        progressBarHP->setProgressColor(0.0f, 1.0f, 0.0f, 1.0f);
        progressBarHP->setBackgroundColor(1.0f, 0.0f, 0.0f, 1.0f);
        progressBarHP->setProgress(1.0f);

        progressBarXP = std::make_shared<Beryll::ProgressBar>( -1, 97.6f, 102, 3.0f);
        m_guiObjects.push_back(progressBarXP);
        progressBarXP->setFontColor(0.0f, 0.0f, 0.0f, 0.0f);
        progressBarXP->setProgressColor(0.0f, 0.0f, 1.0f, 1.0f);
        progressBarXP->setBackgroundColor(0.0f, 0.0f, 0.0f, 1.0f);
        progressBarXP->setProgress(0.0f);

        buttonExit = std::make_shared<Beryll::ButtonWithText>("Exit", EnAndVars::FontsPath::ROBOTO, 5, 35, 18, 30, 7);
        m_guiObjects.push_back(buttonExit);
        buttonExit->disable();

        // Pause.
        buttonPause = std::make_shared<Beryll::ButtonWithText>("||", EnAndVars::FontsPath::ROBOTO, 2.5f, 100 - 6 * screenAR, 0, 6 * screenAR, 4.5f);
        m_guiObjects.push_back(buttonPause);
        buttonPauseResume = std::make_shared<Beryll::ButtonWithText>("Resume", EnAndVars::FontsPath::ROBOTO, 5, 25, 40, 50, 10);
        m_guiObjects.push_back(buttonPauseResume);
        buttonPauseResume->disable();

        // Resurrect.
        textureResurrect = std::make_shared<Beryll::GUITexture>("GUI/playState/CanResurrect.jpg", 20, 25, 60, 25);
        m_guiObjects.push_back(textureResurrect);
        textureResurrect->disable();
        buttonResurrectOk = std::make_shared<Beryll::ButtonWithText>("Ok", EnAndVars::FontsPath::ROBOTO, 5, 50, 18, 30, 7);
        m_guiObjects.push_back(buttonResurrectOk);
        buttonResurrectOk->disable();

        // Resurrect no crystals.
        textureResurrectNoCrystals = std::make_shared<Beryll::GUITexture>("GUI/playState/ResurrectNoCrystals.jpg", 20, 25, 60, 25);
        m_guiObjects.push_back(textureResurrectNoCrystals);
        textureResurrectNoCrystals->disable();

        // Lose.
        textureLose = std::make_shared<Beryll::GUITexture>("GUI/playState/Lose.jpg", 20, 25, 60, 25);
        m_guiObjects.push_back(textureLose);
        textureLose->disable();

        // Kill all enemies before boss.
        textureKillAll = std::make_shared<Beryll::GUITexture>("GUI/playState/KillAllEnemiesToSpawnBoss.jpg", 20, 25, 60, 25);
        m_guiObjects.push_back(textureKillAll);
        textureKillAll->disable();
        buttonKillAllOk = std::make_shared<Beryll::ButtonWithText>("Ok", EnAndVars::FontsPath::ROBOTO, 5, 35, 18, 30, 7);
        m_guiObjects.push_back(buttonKillAllOk);
        buttonKillAllOk->disable();

        // Win.
        textureWin = std::make_shared<Beryll::GUITexture>("GUI/playState/Win.jpg", 20, 25, 60, 25);
        m_guiObjects.push_back(textureWin);
        textureWin->disable();
        buttonWinOk = std::make_shared<Beryll::ButtonWithText>("Ok", EnAndVars::FontsPath::ROBOTO, 5, 35, 18, 30, 7);
        m_guiObjects.push_back(buttonWinOk);
        buttonWinOk->disable();

        // Menus before specific bosses.
        // Tank with commander.
        textureTankWithCommander = std::make_shared<Beryll::GUITexture>("GUI/playState/BossTankWithCommander.jpg", 20, 25, 60, 25);
        m_guiObjects.push_back(textureTankWithCommander);
        textureTankWithCommander->disable();
        buttonTankWithCommanderOk = std::make_shared<Beryll::ButtonWithText>("Ok", EnAndVars::FontsPath::ROBOTO, 5, 35, 18, 30, 7);
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

            m_statisticsUpdateTime = Beryll::TimeStep::getMilliSecFromStart();
        }

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
            return;
        }
        else if(buttonTankWithCommanderOk->getIsPressed())
        {
            GameStateHelper::resumeGame();
            textureTankWithCommander->disable();
            buttonTankWithCommanderOk->disable();
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
        GameStateHelper::pauseGame();
        textureResurrect->enable();
        buttonResurrectOk->enable();
        buttonExit->leftPos = 0.2f;
        buttonExit->enable();
    }

    void PlayStateGUILayer::hideMenuResurrect()
    {
        GameStateHelper::resumeGame();
        textureResurrect->disable();
        buttonResurrectOk->disable();
        buttonExit->disable();
    }

    void PlayStateGUILayer::showMenuResurrectNoCrystals()
    {
        GameStateHelper::pauseGame();
        textureResurrectNoCrystals->enable();
        buttonExit->leftPos = 0.35f;
        buttonExit->enable();
    }

    void PlayStateGUILayer::showMenuLose()
    {
        GameStateHelper::pauseGame();
        textureLose->enable();
        buttonExit->leftPos = 0.35f;
        buttonExit->enable();
    }

    void PlayStateGUILayer::showMenuKillAllBeforeBoss()
    {
        GameStateHelper::pauseGame();
        textureKillAll->enable();
        buttonKillAllOk->enable();
    }

    void PlayStateGUILayer::showMenuWin()
    {
        GameStateHelper::pauseGame();
        textureWin->enable();
        buttonWinOk->enable();
    }

    void PlayStateGUILayer::showMenuBossTankWithCommander()
    {
        GameStateHelper::pauseGame();
        textureTankWithCommander->enable();
        buttonTankWithCommanderOk->enable();
    }
}
