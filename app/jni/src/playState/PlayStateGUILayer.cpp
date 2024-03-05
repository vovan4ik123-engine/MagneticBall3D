#include "PlayStateGUILayer.h"
#include "EnumsAndVariables.h"

namespace MagneticBall3D
{
    PlayStateGUILayer::PlayStateGUILayer()
    {
        m_ID = Beryll::LayerID::PLAY_GUI;

        m_statistics1 = std::make_shared<Beryll::Text>("Frame: 00000  FPS: 00000", EnumsAndVariables::FontsPath::ROBOTO, 2.5f, 0, 0);
        m_guiObjects.push_back(m_statistics1);
        m_statistics2 = std::make_shared<Beryll::Text>("Phys: 00000  Logic: 00000  GPU: 00000", EnumsAndVariables::FontsPath::ROBOTO, 2.5f, 0, 3);
        m_guiObjects.push_back(m_statistics2);

        sliderFPS = std::make_shared<Beryll::SliderHorizontal>("FPS", EnumsAndVariables::FontsPath::ROBOTO, 2, 2, 6, 40, 2, 10, 200);
        m_guiObjects.push_back(sliderFPS);
        sliderFPS->setValue(200.0f);

//        sliderFriction = std::make_shared<Beryll::SliderHorizontal>("Friction", EnumsAndVariables::FontsPath::ROBOTO, 2, 2, 9, 40, 2, 0, 100);
//        m_guiObjects.push_back(sliderFriction);
//        sliderFriction->setValue(100.0f);

//        sliderDamping = std::make_shared<Beryll::SliderHorizontal>("Damping", EnumsAndVariables::FontsPath::ROBOTO, 2, 2, 12, 40, 2, 0, 1);
//        m_guiObjects.push_back(sliderDamping);
//        sliderDamping->setValue(0.0f);
//
//        sliderImpulse = std::make_shared<Beryll::SliderHorizontal>("Impulse", EnumsAndVariables::FontsPath::ROBOTO, 2, 2, 15, 40, 2, 0, 1);
//        m_guiObjects.push_back(sliderImpulse);
//        sliderImpulse->setValue(0.1f);
//
//        sliderTorque = std::make_shared<Beryll::SliderHorizontal>("Torque", EnumsAndVariables::FontsPath::ROBOTO, 2, 2, 18, 40, 2, 0, 1);
//        m_guiObjects.push_back(sliderTorque);
//        sliderTorque->setValue(0.6f);
    }

    PlayStateGUILayer::~PlayStateGUILayer()
    {

    }

    void PlayStateGUILayer::updateBeforePhysics()
    {
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
