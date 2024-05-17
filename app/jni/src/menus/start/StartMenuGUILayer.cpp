#include "StartMenuGUILayer.h"
#include "EnumsAndVariables.h"
#include "GameStateHelper.h"

namespace MagneticBall3D
{
    StartMenuGUILayer::StartMenuGUILayer()
    {
        m_ID = Beryll::LayerID::PLAY_GUI;

        Beryll::Window::getInstance()->setClearColor(0.0666f * 0.7f, 0.7333f * 0.7f, 0.2f * 0.7f, 1.0f);

        map1Texture = std::make_shared<Beryll::GUITexture>("loadingScreen/M1Screen.jpg", 0.15f, 0.1f, 0.7f, 0.5f);
        m_guiObjects.push_back(map1Texture);

        buttonPlay = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/start/Play.jpg", "", 0.25f, 0.7f, 0.5f, 0.1f);
        m_guiObjects.push_back(buttonPlay);

        buttonShop = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/start/Shop.jpg", "", -0.01f, 0.9f, 0.34f, 0.1f);
        m_guiObjects.push_back(buttonShop);

        buttonPlayerTalents = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/start/PlayerTalents.jpg", "", 0.33f, 0.9f, 0.33f, 0.1f);
        m_guiObjects.push_back(buttonPlayerTalents);

        buttonSettings = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/start/Settings.jpg", "", 0.66f, 0.9f, 0.34f, 0.1f);
        m_guiObjects.push_back(buttonSettings);

        textCrystals = std::make_shared<Beryll::Text>("Crystals: 00000", EnAndVars::FontsPath::CREAMY, 0.024f, 0.55f, 0, 0.45f, 0.03f);
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
