#include "GameStateHelper.h"
#include "EngineHeaders.h"
#include "playState/PlayStateGUILayer.h"
#include "playState/PlayStateSceneLayer.h"
#include "menus/start/StartMenuGUILayer.h"

namespace MagneticBall3D
{
    void GameStateHelper::pushStartMenuState()
    {
        std::shared_ptr<MagneticBall3D::StartMenuGUILayer> GUILayer = std::make_shared<MagneticBall3D::StartMenuGUILayer>();

        std::shared_ptr<Beryll::GameState> startMenuState = std::make_shared<Beryll::GameState>();
        startMenuState->ID = Beryll::GameStateID::START_SCREEN;
        startMenuState->layerStack.pushOverlay(GUILayer);

        Beryll::GameStateMachine::pushState(startMenuState);
    }

    void GameStateHelper::pushPlayState()
    {
        std::shared_ptr<MagneticBall3D::PlayStateGUILayer> GUILayer = std::make_shared<MagneticBall3D::PlayStateGUILayer>();
        std::shared_ptr<MagneticBall3D::PlayStateSceneLayer> sceneLayer = std::make_shared<MagneticBall3D::PlayStateSceneLayer>(GUILayer);

        std::shared_ptr<Beryll::GameState> playState = std::make_shared<Beryll::GameState>();
        playState->ID = Beryll::GameStateID::PLAY;
        playState->layerStack.pushLayer(sceneLayer);
        playState->layerStack.pushOverlay(GUILayer);

        Beryll::GameStateMachine::pushState(playState);
    }

    void GameStateHelper::pushSettingsState()
    {

    }

    void GameStateHelper::pushShopState()
    {

    }

    void GameStateHelper::popState()
    {
        Beryll::GameStateMachine::popState();
    }
}
