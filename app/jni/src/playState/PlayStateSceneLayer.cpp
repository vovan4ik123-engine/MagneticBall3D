#include "PlayStateSceneLayer.h"
#include "maps/Map1.h"

namespace MagneticBall3D
{
    PlayStateSceneLayer::PlayStateSceneLayer(std::shared_ptr<PlayStateGUILayer> gui)
    {
        m_ID = Beryll::LayerID::PLAY_SCENE;

        Beryll::Physics::hardRemoveAllObjects();

        m_map = std::make_shared<Map1>(gui);

//        std::vector<glm::ivec2> path = m_pathFinder.findPath({0, 0}, {60, 0});
//        for(const auto& point : path) { BR_INFO("point in path 1 %d %d", point.x, point.y); }

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
