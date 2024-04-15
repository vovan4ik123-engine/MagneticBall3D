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

        std::shared_ptr<Beryll::GUITexture> map1Texture;
        std::shared_ptr<Beryll::ButtonWithTexture> buttonPlay;
        std::shared_ptr<Beryll::ButtonWithTexture> buttonShop;
        std::shared_ptr<Beryll::ButtonWithTexture> buttonPlayerTalents;
        std::shared_ptr<Beryll::ButtonWithTexture> buttonSettings;
        std::shared_ptr<Beryll::Text> textCrystals;

    private:
        std::vector<std::shared_ptr<Beryll::GUIObject>> m_guiObjects;
    };
}
