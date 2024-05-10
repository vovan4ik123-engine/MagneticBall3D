#include "StartMenuGUILayer.h"
#include "EnumsAndVariables.h"
#include "GameStateHelper.h"

namespace MagneticBall3D
{
    StartMenuGUILayer::StartMenuGUILayer()
    {
        m_ID = Beryll::LayerID::PLAY_GUI;

        Beryll::Window::getInstance()->setClearColor(0.0666f * 0.7f, 0.7333f * 0.7f, 0.2f * 0.7f, 1.0f);

        map1Texture = std::make_shared<Beryll::GUITexture>("loadingScreen/M1Screen.jpg", 15, 10, 70, 50);
        m_guiObjects.push_back(map1Texture);

        buttonPlay = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/start/Play.jpg", "", 25, 70, 50, 10);
        m_guiObjects.push_back(buttonPlay);

        buttonShop = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/start/Shop.jpg", "", -1.0f, 90, 34.0f, 10);
        m_guiObjects.push_back(buttonShop);

        buttonPlayerTalents = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/start/PlayerTalents.jpg", "", 33.0f, 90, 33.3f, 10);
        m_guiObjects.push_back(buttonPlayerTalents);

        buttonSettings = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/start/Settings.jpg", "", 66.3f, 90, 34.0f, 10);
        m_guiObjects.push_back(buttonSettings);

        textCrystals = std::make_shared<Beryll::Text>("Crystals: 00000", EnAndVars::FontsPath::CREAMY, 2.4f, 55, 0, 45, 3);
        m_guiObjects.push_back(textCrystals);
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

        textCrystals->text = "Crystals: " + std::to_string(EnAndVars::CurrencyBalance::crystals);

        if(buttonPlay->getIsPressed())
        {
            GameStateHelper::pushPlayState();
        }
        else if(buttonShop->getIsPressed())
        {
            GameStateHelper::pushShopState();
        }
        else if(buttonPlayerTalents->getIsPressed())
        {
            GameStateHelper::pushPlayerTalentsState();
        }
        else if(buttonSettings->getIsPressed())
        {
            GameStateHelper::pushSettingsState();
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
