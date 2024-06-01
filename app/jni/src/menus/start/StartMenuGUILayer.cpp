#include "StartMenuGUILayer.h"
#include "EnumsAndVariables.h"
#include "GameStateHelper.h"
#include "DataBaseHelper.h"

namespace MagneticBall3D
{
    StartMenuGUILayer::StartMenuGUILayer()
    {
        m_ID = Beryll::LayerID::PLAY_GUI;

        Beryll::Window::getInstance()->setClearColor(0.0666f * 0.7f, 0.7333f * 0.7f, 0.2f * 0.7f, 1.0f);

        m_map0TutorialTexture = std::make_shared<Beryll::GUITexture>("loadingScreen/Map0Screen.jpg", 0.2f, 0.1f, 0.6f, 0.5f);
        m_guiObjects.push_back(m_map0TutorialTexture);
        m_allMapsTextures.push_back(m_map0TutorialTexture);

        m_map1Texture = std::make_shared<Beryll::GUITexture>("loadingScreen/Map1Screen.jpg", 0.2f, 0.1f, 0.6f, 0.5f);
        m_guiObjects.push_back(m_map1Texture);
        m_allMapsTextures.push_back(m_map1Texture);

        // Disable all.
        for(auto& mapTexture : m_allMapsTextures)
            mapTexture->disable();

        BR_ASSERT((EnAndVars::MapsProgress::currentMapIndex < m_allMapsTextures.size()), "%s", "currentMapIndex is wrong.");
        // Enable by index.
        if(EnAndVars::MapsProgress::currentMapIndex < m_allMapsTextures.size())
            m_allMapsTextures[EnAndVars::MapsProgress::currentMapIndex]->enable();

        m_buttonMapSwipeLeft = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/start/MapSwipeLeft.jpg", "", 0.05f, 0.3f, 0.15f, 0.1f);
        m_guiObjects.push_back(m_buttonMapSwipeLeft);
        if(EnAndVars::MapsProgress::currentMapIndex == 0)
            m_buttonMapSwipeLeft->disable();

        m_buttonMapSwipeRight = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/start/MapSwipeRight.jpg", "", 0.8f, 0.3f, 0.15f, 0.1f);
        m_guiObjects.push_back(m_buttonMapSwipeRight);
        if(EnAndVars::MapsProgress::currentMapIndex == EnAndVars::MapsProgress::lastOpenedMapIndex ||
           EnAndVars::MapsProgress::currentMapIndex == EnAndVars::MapsProgress::maxMapIndex)
            m_buttonMapSwipeRight->disable();

        m_buttonPlay = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/start/Play.jpg", "", 0.25f, 0.7f, 0.5f, 0.1f);
        m_guiObjects.push_back(m_buttonPlay);

//        m_buttonShop = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/start/Shop.jpg", "", -0.01f, 0.9f, 0.34f, 0.1f);
//        m_guiObjects.push_back(m_buttonShop);
//
//        m_buttonPlayerTalents = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/start/PlayerTalents.jpg", "", 0.33f, 0.9f, 0.33f, 0.1f);
//        m_guiObjects.push_back(m_buttonPlayerTalents);

        m_buttonSettings = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/start/Settings.jpg", "", 0.66f, 0.9f, 0.34f, 0.1f);
        m_guiObjects.push_back(m_buttonSettings);

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

        if(m_buttonPlay->getIsPressed())
        {
            GameStateHelper::popState();
            GameStateHelper::pushPlayState();
        }
//        else if(m_buttonShop->getIsPressed())
//        {
//            GameStateHelper::pushShopState();
//        }
//        else if(m_buttonPlayerTalents->getIsPressed())
//        {
//            GameStateHelper::pushPlayerTalentsState();
//        }
        else if(m_buttonSettings->getIsPressed())
        {
            GameStateHelper::pushSettingsState();
        }
        else if(m_buttonMapSwipeLeft->getIsPressed() &&
                EnAndVars::MapsProgress::currentMapIndex > 0)
        {
            --EnAndVars::MapsProgress::currentMapIndex;
            BR_INFO("currentMapIndex: %d", EnAndVars::MapsProgress::currentMapIndex);
            DataBaseHelper::storeMapsProgressCurrentMapIndex(EnAndVars::MapsProgress::currentMapIndex);

            if(EnAndVars::MapsProgress::currentMapIndex == 0)
                m_buttonMapSwipeLeft->disable();
            else
                m_buttonMapSwipeLeft->enable();

            if(EnAndVars::MapsProgress::currentMapIndex < EnAndVars::MapsProgress::lastOpenedMapIndex &&
               EnAndVars::MapsProgress::currentMapIndex < EnAndVars::MapsProgress::maxMapIndex)
                m_buttonMapSwipeRight->enable();
            else
                m_buttonMapSwipeRight->disable();

            // Disable all.
            for(auto& mapTexture : m_allMapsTextures)
                mapTexture->disable();

            BR_ASSERT((EnAndVars::MapsProgress::currentMapIndex < m_allMapsTextures.size()), "%s", "currentMapIndex is wrong.");
            // Enable by index.
            if(EnAndVars::MapsProgress::currentMapIndex < m_allMapsTextures.size())
                m_allMapsTextures[EnAndVars::MapsProgress::currentMapIndex]->enable();
        }
        else if(m_buttonMapSwipeRight->getIsPressed() &&
                EnAndVars::MapsProgress::currentMapIndex < EnAndVars::MapsProgress::lastOpenedMapIndex &&
                EnAndVars::MapsProgress::currentMapIndex < EnAndVars::MapsProgress::maxMapIndex)
        {
            ++EnAndVars::MapsProgress::currentMapIndex;
            BR_INFO("currentMapIndex: %d", EnAndVars::MapsProgress::currentMapIndex);
            DataBaseHelper::storeMapsProgressCurrentMapIndex(EnAndVars::MapsProgress::currentMapIndex);

            if(EnAndVars::MapsProgress::currentMapIndex == 0)
                m_buttonMapSwipeLeft->disable();
            else
                m_buttonMapSwipeLeft->enable();

            if(EnAndVars::MapsProgress::currentMapIndex < EnAndVars::MapsProgress::lastOpenedMapIndex &&
               EnAndVars::MapsProgress::currentMapIndex < EnAndVars::MapsProgress::maxMapIndex)
                m_buttonMapSwipeRight->enable();
            else
                m_buttonMapSwipeRight->disable();

            // Disable all.
            for(auto& mapTexture : m_allMapsTextures)
                mapTexture->disable();

            BR_ASSERT((EnAndVars::MapsProgress::currentMapIndex < m_allMapsTextures.size()), "%s", "currentMapIndex is wrong.");
            // Enable by index.
            if(EnAndVars::MapsProgress::currentMapIndex < m_allMapsTextures.size())
                m_allMapsTextures[EnAndVars::MapsProgress::currentMapIndex]->enable();
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
