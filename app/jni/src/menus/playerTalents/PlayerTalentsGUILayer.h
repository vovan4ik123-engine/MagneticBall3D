#pragma once

#include "EngineHeaders.h"

namespace MagneticBall3D
{
    class PlayerTalentsGUILayer : public Beryll::Layer
    {
    public:
        PlayerTalentsGUILayer();
        ~PlayerTalentsGUILayer() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

    private:
        std::vector<std::shared_ptr<Beryll::GUIObject>> m_guiObjects;
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonBack;
    };
}
