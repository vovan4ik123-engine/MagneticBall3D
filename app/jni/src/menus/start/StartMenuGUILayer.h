#pragma once

#include "EngineHeaders.h"

namespace MagneticBall3D
{
    class StartMenuGUILayer : public Beryll::Layer
    {
    public:
        StartMenuGUILayer();
        ~StartMenuGUILayer() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

    private:
        std::vector<std::shared_ptr<Beryll::GUIObject>> m_guiObjects;

        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonShop;
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonTalents;
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonSettings;
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonMapSwipeLeft;
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonMapSwipeRight;
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonPlay;

        void enableDisableButtons();

        std::vector<std::shared_ptr<Beryll::GUITexture>> m_mapsPreviews;

        std::shared_ptr<Beryll::GUIText> m_crystalsCount;
    };
}
