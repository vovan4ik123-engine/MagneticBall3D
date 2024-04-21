#include "PlayStateSceneLayer.h"
#include "maps/Map1.h"

namespace MagneticBall3D
{
    PlayStateSceneLayer::PlayStateSceneLayer(std::shared_ptr<PlayStateGUILayer> gui)
    {
        m_ID = Beryll::LayerID::PLAY_SCENE;

        Beryll::Physics::hardRemoveAllObjects();

        m_map = std::make_shared<Map1>(gui);

        EnumsAndVariables::mapStartTimeSec = Beryll::TimeStep::getSecFromStart();
        EnumsAndVariables::mapPlayTimeSec = 0.0f;
        EnumsAndVariables::mapSwipeCount = 0;

        //BR_INFO(" X:%f Y:%f Z:%f", .x, .y, .z);
        //BR_INFO("%s", "");
        //BR_INFO(" %f", );
        //BR_INFO(" %d", );
    }

    PlayStateSceneLayer::~PlayStateSceneLayer()
    {

    }

    void PlayStateSceneLayer::updateBeforePhysics()
    {
        m_map->updateBeforePhysics();
    }

    void PlayStateSceneLayer::updateAfterPhysics()
    {
        m_map->updateAfterPhysics();
    }

    void PlayStateSceneLayer::draw()
    {
        //BR_INFO("%s", "scene draw call");

        m_map->draw();
    }
}
