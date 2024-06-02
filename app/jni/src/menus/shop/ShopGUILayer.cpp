#include "ShopGUILayer.h"
#include "EnumsAndVariables.h"
#include "GameStateHelper.h"

namespace MagneticBall3D
{
    ShopGUILayer::ShopGUILayer()
    {
        const float screenAR = Beryll::MainImGUI::getInstance()->getGUIScreenAspectRation();

        m_buttonBack = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/LeftArrow.jpg", "", 0, 0.9f, 0.3f, 0.1f);
        m_guiObjects.push_back(m_buttonBack);

        m_shopText = std::make_shared<Beryll::Text>("Shop", EnAndVars::FontsPath::roboto, 0.05f, 0.38f, 0, 0.25f, 0.055f);
        m_guiObjects.push_back(m_shopText);

//        button70crystals = std::make_shared<Beryll::ButtonWithText>("      1$\n70 crystals", EnAndVars::FontsPath::roboto, 0.03f, 0.05f, 0.1f, 0.4f, 0.1f);
//        m_guiObjects.push_back(button70crystals);
//
//        button200crystals = std::make_shared<Beryll::ButtonWithText>("      2.5$\n200 crystals", EnAndVars::FontsPath::roboto, 0.03f, 0.55f, 0.1f, 0.4f, 0.1f);
//        m_guiObjects.push_back(button200crystals);
//
//        button420crystals = std::make_shared<Beryll::ButtonWithText>("       5$\n420 crystals", EnAndVars::FontsPath::roboto, 0.03f, 0.05f, 0.25f, 0.4f, 0.1f);
//        m_guiObjects.push_back(button420crystals);
//
//        button900crystals = std::make_shared<Beryll::ButtonWithText>("       10$\n900 crystals", EnAndVars::FontsPath::roboto, 0.03f, 0.55f, 0.25f, 0.4f, 0.1f);
//        m_guiObjects.push_back(button900crystals);
//
//        button2500crystals = std::make_shared<Beryll::ButtonWithText>("       25$\n2500 crystals", EnAndVars::FontsPath::roboto, 0.03f, 0.05f, 0.4f, 0.4f, 0.1f);
//        m_guiObjects.push_back(button2500crystals);
//
//        button15000crystals = std::make_shared<Beryll::ButtonWithText>("      100$\n15000 crystals", EnAndVars::FontsPath::roboto, 0.03f, 0.55f, 0.4f, 0.4f, 0.1f);
//        m_guiObjects.push_back(button15000crystals);
    }

    ShopGUILayer::~ShopGUILayer()
    {

    }

    void ShopGUILayer::updateBeforePhysics()
    {
        for(const std::shared_ptr<Beryll::GUIObject>& go : m_guiObjects)
        {
            if(go->getIsEnabled())
            {
                go->updateBeforePhysics();
            }
        }

        if(m_buttonBack->getIsPressed())
        {
            GameStateHelper::popState();
            return;
        }

//        if(button70crystals->getIsPressed())
//        {
//
//        }
//        else if(button200crystals->getIsPressed())
//        {
//
//        }
//        else if(button420crystals->getIsPressed())
//        {
//
//        }
//        else if(button900crystals->getIsPressed())
//        {
//
//        }
//        else if(button2500crystals->getIsPressed())
//        {
//
//        }
//        else if(button15000crystals->getIsPressed())
//        {
//
//        }
    }

    void ShopGUILayer::updateAfterPhysics()
    {

    }

    void ShopGUILayer::draw()
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
