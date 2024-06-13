#include "ShopGUILayer.h"
#include "EnumsAndVariables.h"
#include "GameStateHelper.h"

namespace MagneticBall3D
{
    ShopGUILayer::ShopGUILayer()
    {
        m_buttonBack = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/LeftArrow.jpg", "", 0, 0.9f, 0.3f, 0.1f);
        m_guiObjects.push_back(m_buttonBack);
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
