#include "PlayStateSceneLayer.h"

namespace MagneticBall3D
{
    PlayStateSceneLayer::PlayStateSceneLayer(std::shared_ptr<PlayStateGUILayer> gui)
    {
        m_ID = Beryll::LayerID::PLAY_SCENE;
        
        m_gui = std::move(gui);

        Beryll::Physics::hardRemoveAllObjects();
        

        //BR_INFO(" X:%f Y:%f Z:%f", .x, .y, .z);
        //BR_INFO("%s", "");
        //BR_INFO(" %f", );
    }

    PlayStateSceneLayer::~PlayStateSceneLayer()
    {

    }

    void PlayStateSceneLayer::updateBeforePhysics()
    {
        
    }

    void PlayStateSceneLayer::updateAfterPhysics()
    {
        
    }

    void PlayStateSceneLayer::draw()
    {
        //BR_INFO("%s", "scene draw call");
        
    }
}
