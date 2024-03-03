#pragma once

#include "EngineHeaders.h"
#include "PlayStateGUILayer.h"

namespace MagneticBall3D
{
    class PlayStateSceneLayer : public Beryll::Layer
    {
    public:
        PlayStateSceneLayer(std::shared_ptr<PlayStateGUILayer> gui);
        ~PlayStateSceneLayer() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

    private:
        std::shared_ptr<PlayStateGUILayer> m_gui;
    };
}
