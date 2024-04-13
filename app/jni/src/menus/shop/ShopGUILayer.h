#pragma once

#include "EngineHeaders.h"

namespace MagneticBall3D
{
    class ShopGUILayer : public Beryll::Layer
    {
    public:
        ShopGUILayer();
        ~ShopGUILayer() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;



    private:
        std::vector<std::shared_ptr<Beryll::GUIObject>> m_guiObjects;
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonBack;

        std::shared_ptr<Beryll::ButtonWithText> button70crystals;
        std::shared_ptr<Beryll::ButtonWithText> button200crystals;
        std::shared_ptr<Beryll::ButtonWithText> button420crystals;
        std::shared_ptr<Beryll::ButtonWithText> button900crystals;
        std::shared_ptr<Beryll::ButtonWithText> button2500crystals;
        std::shared_ptr<Beryll::ButtonWithText> button15000crystals;

        std::shared_ptr<Beryll::Text> m_shopText;
    };
}
