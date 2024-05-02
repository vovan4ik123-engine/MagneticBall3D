#include "PlayStateGUILayer.h"
#include "EnumsAndVariables.h"
#include "GameStateHelper.h"

namespace MagneticBall3D
{
    PlayStateGUILayer::PlayStateGUILayer()
    {
        m_ID = Beryll::LayerID::PLAY_GUI;

        const float screenAR = Beryll::MainImGUI::getInstance()->getGUIScreenAspectRation();

        m_statistics1 = std::make_shared<Beryll::Text>("Frame: 00000  FPS: 00000", EnumsAndVariables::FontsPath::ROBOTO, 2.5f, 0, 0, 50, 3);
        m_guiObjects.push_back(m_statistics1);
        m_statistics2 = std::make_shared<Beryll::Text>("Phys: 00000  Logic: 00000  GPU: 00000", EnumsAndVariables::FontsPath::ROBOTO, 2.5f, 0, 2.5f, 70, 3);
        m_guiObjects.push_back(m_statistics2);
        m_swipeCount = std::make_shared<Beryll::Text>("Swipe: 0000 Time: 00000", EnumsAndVariables::FontsPath::ROBOTO, 2.0f, 0, 4.75f, 45, 2.5f);
        m_guiObjects.push_back(m_swipeCount);

//        sliderImpulse = std::make_shared<Beryll::SliderHorizontal>("impulse", EnumsAndVariables::FontsPath::ROBOTO, 2, 2, 6, 40, 2, 0, 1);
//        m_guiObjects.push_back(sliderImpulse);
//        sliderImpulse->setValue(0.15f);
//
//        sliderTorque = std::make_shared<Beryll::SliderHorizontal>("torque", EnumsAndVariables::FontsPath::ROBOTO, 2, 2, 9, 40, 2, 0, 1);
//        m_guiObjects.push_back(sliderTorque);
//        sliderTorque->setValue(0.1f);

//        sliderAmbient = std::make_shared<Beryll::SliderHorizontal>("ambient", EnumsAndVariables::FontsPath::ROBOTO, 2, 2, 7, 40, 2, 0, 1);
//        m_guiObjects.push_back(sliderAmbient);
//        sliderAmbient->setValue(0.7f);
//
//        sliderSunPower = std::make_shared<Beryll::SliderHorizontal>("sun", EnumsAndVariables::FontsPath::ROBOTO, 2, 2, 10, 40, 2, 0, 3);
//        m_guiObjects.push_back(sliderSunPower);
//        sliderSunPower->setValue(0.8f);
//
//        sliderSpecularPower = std::make_shared<Beryll::SliderHorizontal>("specular", EnumsAndVariables::FontsPath::ROBOTO, 2, 2, 13, 40, 2, 0, 3);
//        m_guiObjects.push_back(sliderSpecularPower);
//        sliderSpecularPower->setValue(1.0f);
//
//        sliderJumppad = std::make_shared<Beryll::SliderHorizontal>("jumppad", EnumsAndVariables::FontsPath::ROBOTO, 2, 2, 16, 40, 2, 50, 200);
//        m_guiObjects.push_back(sliderJumppad);
//        sliderJumppad->setValue(120.0f);

        progressBarHP = std::make_shared<Beryll::ProgressBar>( 60, 0, 40, 3);
        m_guiObjects.push_back(progressBarHP);
        progressBarHP->setFontColor(0.0f, 0.0f, 0.0f, 0.0f);
        progressBarHP->setProgressColor(0.0f, 1.0f, 0.0f, 1.0f);
        progressBarHP->setBackgroundColor(1.0f, 0.0f, 0.0f, 1.0f);
        progressBarHP->setProgress(1.0f);

        progressBarXP = std::make_shared<Beryll::ProgressBar>( 0, 96, 100, 5.0f);
        m_guiObjects.push_back(progressBarXP);
        progressBarXP->setFontColor(0.0f, 0.0f, 0.0f, 0.0f);
        progressBarXP->setProgressColor(0.0f, 0.2275f, 1.0f, 1.0f);
        progressBarXP->setBackgroundColor(0.64f, 0.89f, 0.93f, 1.0f);
        progressBarXP->setProgress(0.0f);

        buttonExit = std::make_shared<Beryll::ButtonWithText>("Exit", EnumsAndVariables::FontsPath::ROBOTO, 5, 35, 50, 30, 7);
        m_guiObjects.push_back(buttonExit);
        buttonExit->disable();

        // Pause.
        buttonPause = std::make_shared<Beryll::ButtonWithText>("P", EnumsAndVariables::FontsPath::ROBOTO, 3, 100 - 7 * screenAR, 3, 7 * screenAR, 7);
        m_guiObjects.push_back(buttonPause);
        buttonPauseResume = std::make_shared<Beryll::ButtonWithText>("Resume", EnumsAndVariables::FontsPath::ROBOTO, 5, 30, 40, 40, 7);
        m_guiObjects.push_back(buttonPauseResume);
        buttonPauseResume->disable();

        // Resurrect.
        textureResurrect = std::make_shared<Beryll::GUITexture>("GUI/playState/CanResurrect.jpg", 20, 25, 60, 25);
        m_guiObjects.push_back(textureResurrect);
        textureResurrect->disable();
        buttonResurrectOk = std::make_shared<Beryll::ButtonWithText>("Ok", EnumsAndVariables::FontsPath::ROBOTO, 5, 50, 50, 30, 7);
        m_guiObjects.push_back(buttonResurrectOk);
        buttonResurrectOk->disable();

        // Lose.
        textureLose = std::make_shared<Beryll::GUITexture>("GUI/playState/Lose.jpg", 20, 25, 60, 25);
        m_guiObjects.push_back(textureLose);
        textureLose->disable();

        // Resurrect тo crystals.
        textureResurrectNoCrystals = std::make_shared<Beryll::GUITexture>("GUI/playState/ResurrectNoCrystals.jpg", 20, 25, 60, 25);
        m_guiObjects.push_back(textureResurrectNoCrystals);
        textureResurrectNoCrystals->disable();
    }

    PlayStateGUILayer::~PlayStateGUILayer()
    {

    }

    void PlayStateGUILayer::updateBeforePhysics()
    {
        if(EnumsAndVariables::improvementSystemOnScreen)
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
            stream << "Swipe: " << EnumsAndVariables::mapSwipeCount;
            stream << "  Time: " << int(EnumsAndVariables::mapPlayTimeSec / 60.0f) << ":" << int(std::fmod(EnumsAndVariables::mapPlayTimeSec, 60.0f));
            m_swipeCount->text = stream.str();

            m_statisticsUpdateTime = Beryll::TimeStep::getMilliSecFromStart();
        }

        if(buttonPause->getIsPressed())
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
            GameStateHelper::resumeGame();
            GameStateHelper::popState();
            return;
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

    void PlayStateGUILayer::showResurrectMenu()
    {
        GameStateHelper::pauseGame();
        textureResurrect->enable();
        buttonResurrectOk->enable();
        buttonExit->leftPos = 0.2f;
        buttonExit->enable();
    }

    void PlayStateGUILayer::hideResurrectMenu()
    {
        GameStateHelper::resumeGame();
        textureResurrect->disable();
        buttonResurrectOk->disable();
        buttonExit->disable();
    }

    void PlayStateGUILayer::showResurrectNoCrystalsMenu()
    {
        GameStateHelper::pauseGame();
        textureResurrectNoCrystals->enable();
        buttonExit->leftPos = 0.35f;
        buttonExit->enable();
    }

    void PlayStateGUILayer::showLoseMenu()
    {
        GameStateHelper::pauseGame();
        textureLose->enable();
        buttonExit->leftPos = 0.35f;
        buttonExit->enable();
    }
}
