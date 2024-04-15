#include "PlayerTalentsGUILayer.h"
#include "EnumsAndVariables.h"
#include "GameStateHelper.h"

namespace MagneticBall3D
{
    PlayerTalentsGUILayer::PlayerTalentsGUILayer()
    {
        m_buttonBack = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/LeftArrow.jpg", "", 0, 90, 30, 10);
        m_guiObjects.push_back(m_buttonBack);

        m_talentsText = std::make_shared<Beryll::Text>("Talents", EnumsAndVariables::FontsPath::ROBOTO, 5, 33, 0);
        m_guiObjects.push_back(m_talentsText);
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
