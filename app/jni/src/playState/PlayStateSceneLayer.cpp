#include "PlayStateSceneLayer.h"
#include "maps/Map0Tutorial.h"
#include "maps/Map1.h"
#include "maps/Map2.h"
#include "GameStateHelper.h"

namespace MagneticBall3D
{
    PlayStateSceneLayer::PlayStateSceneLayer(std::shared_ptr<PlayStateGUILayer> gui)
    {
        m_ID = Beryll::LayerID::PLAY_SCENE;

        GameStateHelper::resetAllVariables();

        Beryll::LoadingScreen::setTextureByIndex(EnumsAndVars::MapsProgress::currentMapIndex);

        if(EnumsAndVars::MapsProgress::currentMapIndex == 0)
            m_map = std::make_shared<Map0Tutorial>(gui);
        else if(EnumsAndVars::MapsProgress::currentMapIndex == 1)
            m_map = std::make_shared<Map1>(gui);
        else if(EnumsAndVars::MapsProgress::currentMapIndex == 2)
            m_map = std::make_shared<Map2>(gui);
        else
        {
            BR_ASSERT(false, "Map index does not handled: %d", EnumsAndVars::MapsProgress::currentMapIndex);
        }

        GameStateHelper::prepareVariablesForPlay();

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
