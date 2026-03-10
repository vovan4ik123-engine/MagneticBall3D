#include "StartMenuGUILayer.h"
#include "EnumsAndVariables.h"
#include "GameStateHelper.h"
#include "DataBaseHelper.h"
#include "energySystem/EnergySystem.h"

namespace MagneticBall3D
{
    StartMenuGUILayer::StartMenuGUILayer()
    {
        m_ID = Beryll::LayerID::START_SCREEN_GUI;

        auto background = std::make_shared<Beryll::GUITexture>("GUI/menus/start/StartBackground.jpg",
                                                               glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec2{100.0f, 100.0f});
        m_guiObjects.push_back(background);

        // When we push some game state() over this and then pop it, one button appears without texture. Only first button.
        // I dont know why texture disappears. That is fix for this. Invisible button will affected by this bug.
        auto fixBug = std::make_shared<Beryll::ButtonWithTexture>("GUI/FullTransparent.png", "",
                                                                  glm::vec3{20.0f, 90.0f, 0.1f}, glm::vec2{10.0f, 10.0f});
        m_guiObjects.push_back(fixBug);

        m_buttonShop = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/start/Shop.jpg", "",
                                                                   glm::vec3{0.0f, 66.6f, 0.1f}, glm::vec2{15.0f, 33.4f});
        m_guiObjects.push_back(m_buttonShop);
        m_buttonTalents = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/start/PlayerTalents.jpg", "",
                                                                      glm::vec3{0.0f, 33.3f, 0.1f}, glm::vec2{15.0f, 33.3f});
        m_guiObjects.push_back(m_buttonTalents);
        m_buttonSettings = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/start/Settings.jpg", "",
                                                                       glm::vec3{0.0f, 0.0f, 0.1f}, glm::vec2{15.0f, 33.3f});
        m_guiObjects.push_back(m_buttonSettings);

        m_buttonPlay = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/start/Play.png", "",
                                                                   glm::vec3{47.5f, 7.0f, 0.1f}, glm::vec2{20.0f, 20.0f});
        m_guiObjects.push_back(m_buttonPlay);

        m_buttonMapSwipeLeft = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/start/MapSwipeLeft.png", "",
                                                                           glm::vec3{22.5f, 52.0f, 0.1f}, glm::vec2{10.0f, 14.0f});
        m_guiObjects.push_back(m_buttonMapSwipeLeft);
        m_buttonMapSwipeRight = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/start/MapSwipeRight.png", "",
                                                                            glm::vec3{82.5f, 52.0f, 0.1f}, glm::vec2{10.0f, 14.0f});
        m_guiObjects.push_back(m_buttonMapSwipeRight);

        auto preview1 = std::make_shared<Beryll::GUITexture>("GUI/menus/start/Map0Preview.jpg",
                                                                                   glm::vec3{32.5f, 34.0f, 0.1f}, glm::vec2{50.0f, 50.0f});
        m_guiObjects.push_back(preview1);
        m_mapsPreviews.push_back(preview1);
        auto preview2 = std::make_shared<Beryll::GUITexture>("GUI/menus/start/Map3Preview.jpg",
                                                                                   glm::vec3{32.5f, 34.0f, 0.1f}, glm::vec2{50.0f, 50.0f});
        m_guiObjects.push_back(preview2);
        m_mapsPreviews.push_back(preview2);
        auto preview3 = std::make_shared<Beryll::GUITexture>("GUI/menus/start/Map4Preview.jpg",
                                                                                   glm::vec3{32.5f, 34.0f, 0.1f}, glm::vec2{50.0f, 50.0f});
        m_guiObjects.push_back(preview3);
        m_mapsPreviews.push_back(preview3);
        auto preview4 = std::make_shared<Beryll::GUITexture>("GUI/menus/start/Map5Preview.jpg",
                                                                                   glm::vec3{32.5f, 34.0f, 0.1f}, glm::vec2{50.0f, 50.0f});
        m_guiObjects.push_back(preview4);
        m_mapsPreviews.push_back(preview4);
        auto preview5 = std::make_shared<Beryll::GUITexture>("GUI/menus/start/Map2Preview.jpg",
                                                                                   glm::vec3{32.5f, 34.0f, 0.1f}, glm::vec2{50.0f, 50.0f});
        m_guiObjects.push_back(preview5);
        m_mapsPreviews.push_back(preview5);
        auto preview6 = std::make_shared<Beryll::GUITexture>("GUI/menus/start/Map1Preview.jpg",
                                                                                   glm::vec3{32.5f, 34.0f, 0.1f}, glm::vec2{50.0f, 50.0f});
        m_guiObjects.push_back(preview6);
        m_mapsPreviews.push_back(preview6);

        enableDisableButtons();

        m_crystalsCount = Beryll::Renderer::createGUIText("", glm::vec3{0.06f, 0.06f, 0.06f}, glm::vec3{88.0f, 91.0f, 0.01f}, 0.45f);
        m_guiObjects.push_back(m_crystalsCount);

        // Sort to update nearest objects first. But draw should starts from farest object(in reverse order).
        std::sort(m_guiObjects.begin(), m_guiObjects.end(), [](std::shared_ptr<Beryll::GUIObject> o1, std::shared_ptr<Beryll::GUIObject> o2)
        {
            return (o1->getPositionNormalized().z > o2->getPositionNormalized().z);
        });
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

        m_crystalsCount->text = std::to_string(EnumsAndVars::CurrencyBalance::crystals);

        //EnergySystem::getInstance().update();

        if(m_buttonPlay->getIsPressed())
        {
            //if(EnergySystem::getInstance().isEnoughForPlay())
            {
                //EnergySystem::getInstance().handlePlay();

                GameStateHelper::popState();
                GameStateHelper::pushPlayState();
            }
            //else
            //{
                // Show menu for buy energy.
            //}
        }
        else if(m_buttonShop->getIsPressed())
        {
            GameStateHelper::pushShopState();
        }
        else if(m_buttonTalents->getIsPressed())
        {
            GameStateHelper::pushPlayerTalentsState();
        }
        else if(m_buttonSettings->getIsPressed())
        {
            GameStateHelper::pushSettingsState();
        }
        else if(m_buttonMapSwipeLeft->getIsPressed() && EnumsAndVars::MapsProgress::currentMapIndex > 0)
        {
            --EnumsAndVars::MapsProgress::currentMapIndex;
            enableDisableButtons();

            BR_INFO("currentMapIndex: %d", EnumsAndVars::MapsProgress::currentMapIndex);
            DataBaseHelper::storeMapsProgressCurrentMapIndex(EnumsAndVars::MapsProgress::currentMapIndex);
        }
        else if(m_buttonMapSwipeRight->getIsPressed() &&
                EnumsAndVars::MapsProgress::currentMapIndex < EnumsAndVars::MapsProgress::lastOpenedMapIndex &&
                EnumsAndVars::MapsProgress::currentMapIndex < EnumsAndVars::MapsProgress::maxMapIndex)
        {
            ++EnumsAndVars::MapsProgress::currentMapIndex;
            enableDisableButtons();
            BR_INFO("currentMapIndex: %d", EnumsAndVars::MapsProgress::currentMapIndex);
            DataBaseHelper::storeMapsProgressCurrentMapIndex(EnumsAndVars::MapsProgress::currentMapIndex);
        }
    }

    void StartMenuGUILayer::updateAfterPhysics()
    {
        // Dont need update.
    }

    void StartMenuGUILayer::draw()
    {
        for(auto it = m_guiObjects.rbegin(); it != m_guiObjects.rend(); ++it)
        {
            if((*it)->getIsEnabled())
            {
                (*it)->draw();
            }
        }

        //EnergySystem::getInstance().draw();
    }

    void StartMenuGUILayer::enableDisableButtons()
    {
        // Map swipe left enable.
        if(EnumsAndVars::MapsProgress::currentMapIndex > 0)
        {
            m_buttonMapSwipeLeft->enable();
        }
        else
        {
            m_buttonMapSwipeLeft->disable();
        }

        // Map swipe right enable.
        if(EnumsAndVars::MapsProgress::currentMapIndex < EnumsAndVars::MapsProgress::lastOpenedMapIndex &&
           EnumsAndVars::MapsProgress::currentMapIndex < EnumsAndVars::MapsProgress::maxMapIndex)
        {
            m_buttonMapSwipeRight->enable();
        }
        else
        {
            m_buttonMapSwipeRight->disable();
        }

        // Preview.
        for(const std::shared_ptr<Beryll::GUITexture>& prew : m_mapsPreviews)
        {
            prew->disable();
        }

        BR_ASSERT((EnumsAndVars::MapsProgress::currentMapIndex >= 0 && EnumsAndVars::MapsProgress::currentMapIndex < m_mapsPreviews.size()),
                  "%s", "currentMapIndex must < m_mapsPreviews.size()");

        m_mapsPreviews[EnumsAndVars::MapsProgress::currentMapIndex]->enable();
    }
}
