#include "PlayStateGUILayer.h"
#include "EnumsAndVariables.h"

namespace MagneticBall3D
{
    PlayStateGUILayer::PlayStateGUILayer()
    {
        m_ID = Beryll::LayerID::PLAY_GUI;

        const float screenAR = Beryll::MainImGUI::getInstance()->getGUIScreenAspectRation();

        m_statistics1 = std::make_shared<Beryll::Text>("Frame: 00000  FPS: 00000", EnumsAndVariables::FontsPath::ROBOTO, 2.5f, 0, 0);
        m_guiObjects.push_back(m_statistics1);
        m_statistics2 = std::make_shared<Beryll::Text>("Phys: 00000  Logic: 00000  GPU: 00000", EnumsAndVariables::FontsPath::ROBOTO, 2.5f, 0, 2.5f);
        m_guiObjects.push_back(m_statistics2);

//        sliderImpulse = std::make_shared<Beryll::SliderHorizontal>("impulse", EnumsAndVariables::FontsPath::ROBOTO, 2, 2, 6, 40, 2, 0, 1);
//        m_guiObjects.push_back(sliderImpulse);
//        sliderImpulse->setValue(0.15f);
//
//        sliderTorque = std::make_shared<Beryll::SliderHorizontal>("torque", EnumsAndVariables::FontsPath::ROBOTO, 2, 2, 9, 40, 2, 0, 1);
//        m_guiObjects.push_back(sliderTorque);
//        sliderTorque->setValue(0.1f);

        swipeCount = std::make_shared<Beryll::Text>("0000", EnumsAndVariables::FontsPath::ROBOTO, 2.5f, 0, 4.75f);
        m_guiObjects.push_back(swipeCount);

        sliderEnemy = std::make_shared<Beryll::SliderHorizontal>("enemies", EnumsAndVariables::FontsPath::ROBOTO, 2, 2, 7, 40, 2, 0, 600);
        m_guiObjects.push_back(sliderEnemy);
        sliderEnemy->setValue(25.0f);

        sliderAmbient = std::make_shared<Beryll::SliderHorizontal>("ambient", EnumsAndVariables::FontsPath::ROBOTO, 2, 2, 10, 40, 2, 0, 1);
        m_guiObjects.push_back(sliderAmbient);
        sliderAmbient->setValue(0.7f);

        sliderSunPower = std::make_shared<Beryll::SliderHorizontal>("sun", EnumsAndVariables::FontsPath::ROBOTO, 2, 2, 13, 40, 2, 0, 3);
        m_guiObjects.push_back(sliderSunPower);
        sliderSunPower->setValue(0.8f);

        sliderSpecularPower = std::make_shared<Beryll::SliderHorizontal>("specular", EnumsAndVariables::FontsPath::ROBOTO, 2, 2, 16, 40, 2, 0, 3);
        m_guiObjects.push_back(sliderSpecularPower);
        sliderSpecularPower->setValue(1.0f);

        progressBarHP = std::make_shared<Beryll::ProgressBar>( 60, 0, 40, 3);
        m_guiObjects.push_back(progressBarHP);
        progressBarHP->setFontColor(0.0f, 0.0f, 0.0f, 0.0f);
        progressBarHP->setProgressColor(0.0f, 1.0f, 0.0f, 1.0f);
        progressBarHP->setBackgroundColor(1.0f, 0.0f, 0.0f, 1.0f);
        progressBarHP->setProgress(1.0f);

        progressBarXP = std::make_shared<Beryll::ProgressBar>( 0, 96, 80, 4);
        m_guiObjects.push_back(progressBarXP);
        progressBarXP->setFontColor(0.0f, 0.0f, 0.0f, 0.0f);
        progressBarXP->setProgressColor(0.0f, 0.2275f, 1.0f, 1.0f);
        progressBarXP->setBackgroundColor(0.64f, 0.89f, 0.93f, 1.0f);
        progressBarXP->setProgress(0.0f);

        buttonA = std::make_shared<Beryll::ButtonWithText>("A", EnumsAndVariables::FontsPath::ROBOTO, 5, 80, 90, 10 * screenAR, 10);
        m_guiObjects.push_back(buttonA);

        buttonPause = std::make_shared<Beryll::ButtonWithText>("P", EnumsAndVariables::FontsPath::ROBOTO, 3, 100 - 7 * screenAR, 3, 7 * screenAR, 7);
        m_guiObjects.push_back(buttonPause);

        buttonResume = std::make_shared<Beryll::ButtonWithText>("Resume", EnumsAndVariables::FontsPath::ROBOTO, 5, 30, 45, 40, 10);
        m_guiObjects.push_back(buttonResume);
        buttonResume->disable();
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
            stream << "Frame: " << Beryll::GameLoop::getFrameTime() << "  FPS: " << Beryll::GameLoop::getFPS() << "      ";
            m_statistics1->text = stream.str();

            stream.str(""); // Way to clear std::stringstream.
            stream << std::fixed << std::setprecision(1);
            stream << "Phys: " << Beryll::Physics::getSimulationTime();
            stream << "  Logic: " << (Beryll::GameLoop::getCPUTime() - Beryll::Physics::getSimulationTime());
            stream << "  GPU: " << Beryll::GameLoop::getGPUTime() << "      ";
            m_statistics2->text = stream.str();

            m_statisticsUpdateTime = Beryll::TimeStep::getMilliSecFromStart();
        }

        if(buttonPause->getIsPressed())
        {
            Beryll::Physics::disableSimulation();
            EnumsAndVariables::gameOnPause = true;
            buttonResume->enable();
        }
        else if(buttonResume->getIsPressed())
        {
            Beryll::Physics::enableSimulation();
            EnumsAndVariables::gameOnPause = false;
            buttonResume->disable();
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
}
