#include "StartMenuGUILayer.h"
#include "EnumsAndVariables.h"
#include "GameStateHelper.h"

namespace MagneticBall3D
{
    StartMenuGUILayer::StartMenuGUILayer()
    {
        m_ID = Beryll::LayerID::PLAY_GUI;

        Beryll::Window::getInstance()->setClearColor(0.0666f * 0.7f, 0.7333f * 0.7f, 0.2f * 0.7f, 1.0f);

        map1Texture = std::make_shared<Beryll::GUITexture>("GUI/menus/start/map1.jpg", 15, 10, 70, 50);
        m_guiObjects.push_back(map1Texture);

        buttonPlay = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/start/play.jpg", "", 25, 70, 50, 10);
        m_guiObjects.push_back(buttonPlay);

        buttonShop = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/start/shop.jpg", "", -1.0f, 90, 34.0f, 10);
        m_guiObjects.push_back(buttonShop);

        buttonPlayer = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/start/player.jpg", "", 33.0f, 90, 33.3f, 10);
        m_guiObjects.push_back(buttonPlayer);

        buttonSettings = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/start/settings.jpg", "", 66.3f, 90, 34.0f, 10);
        m_guiObjects.push_back(buttonSettings);
    }

    StartMenuGUILayer::~StartMenuGUILayer()
    {

    }

    void StartMenuGUILayer::updateBeforePhysics()
    {
        for(const std::shared_ptr<Beryll::GUIObject>& go : m_guiObjects)
        {
            if(go->getIsEnabled())
            {
                go->updateBeforePhysics();
            }
        }

        if(buttonPlay->getIsPressed())
        {
            MagneticBall3D::GameStateHelper::pushPlayState();
        }
    }

    void StartMenuGUILayer::updateAfterPhysics()
    {
        // Dont need update.
    }

    void StartMenuGUILayer::draw()
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
