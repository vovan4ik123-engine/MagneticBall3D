#include "PlayerTalentsGUILayer.h"
#include "EnumsAndVariables.h"
#include "GameStateHelper.h"

namespace MagneticBall3D
{
    PlayerTalentsGUILayer::PlayerTalentsGUILayer()
    {
        m_buttonBack = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/LeftArrow.jpg", "", 0, 0.9f, 0.3f, 0.1f);
        m_guiObjects.push_back(m_buttonBack);
    }

    PlayerTalentsGUILayer::~PlayerTalentsGUILayer()
    {

    }

    void PlayerTalentsGUILayer::updateBeforePhysics()
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

    void PlayerTalentsGUILayer::updateAfterPhysics()
    {

    }

    void PlayerTalentsGUILayer::draw()
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
