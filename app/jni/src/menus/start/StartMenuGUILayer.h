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

        std::shared_ptr<Beryll::Text> textCrystals;

    private:
        std::vector<std::shared_ptr<Beryll::GUIObject>> m_guiObjects;

        std::vector<std::shared_ptr<Beryll::GUITexture>> m_allMapsTextures;
        std::shared_ptr<Beryll::GUITexture> m_map0TutorialTexture;
        std::shared_ptr<Beryll::GUITexture> m_map1Texture;
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonMapSwipeLeft;
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonMapSwipeRight;
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonPlay;
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonShop;
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonPlayerTalents;
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonSettings;
    };
}
