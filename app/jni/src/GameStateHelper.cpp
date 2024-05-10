#include "GameStateHelper.h"
#include "EngineHeaders.h"
#include "playState/PlayStateGUILayer.h"
#include "playState/PlayStateSceneLayer.h"
#include "menus/start/StartMenuGUILayer.h"
#include "menus/shop/ShopGUILayer.h"
#include "menus/playerTalents/PlayerTalentsGUILayer.h"
#include "menus/settings/SettingsMenuGUILayer.h"
#include "enemies/Sniper.h"
#include "Sounds.h"

namespace MagneticBall3D
{
    void GameStateHelper::pushStartMenuState()
    {
        std::shared_ptr<StartMenuGUILayer> GUILayer = std::make_shared<StartMenuGUILayer>();

        std::shared_ptr<Beryll::GameState> startMenuState = std::make_shared<Beryll::GameState>();
        startMenuState->ID = Beryll::GameStateID::START_SCREEN;
        startMenuState->layerStack.pushOverlay(GUILayer);

        Beryll::GameStateMachine::pushState(startMenuState);
    }

    void GameStateHelper::pushPlayState()
    {
        std::shared_ptr<PlayStateGUILayer> GUILayer = std::make_shared<PlayStateGUILayer>();
        std::shared_ptr<PlayStateSceneLayer> sceneLayer = std::make_shared<PlayStateSceneLayer>(GUILayer);

        std::shared_ptr<Beryll::GameState> playState = std::make_shared<Beryll::GameState>();
        playState->ID = Beryll::GameStateID::PLAY;
        playState->layerStack.pushLayer(sceneLayer);
        playState->layerStack.pushOverlay(GUILayer);

        Beryll::GameStateMachine::pushState(playState);
    }

    void GameStateHelper::pushShopState()
    {
        std::shared_ptr<ShopGUILayer> GUILayer = std::make_shared<ShopGUILayer>();

        std::shared_ptr<Beryll::GameState> shopMenuState = std::make_shared<Beryll::GameState>();
        shopMenuState->ID = Beryll::GameStateID::SHOP;
        shopMenuState->layerStack.pushOverlay(GUILayer);

        Beryll::GameStateMachine::pushState(shopMenuState);
    }

    void GameStateHelper::pushPlayerTalentsState()
    {
        std::shared_ptr<PlayerTalentsGUILayer> GUILayer = std::make_shared<PlayerTalentsGUILayer>();

        std::shared_ptr<Beryll::GameState> playerTalentsMenuState = std::make_shared<Beryll::GameState>();
        playerTalentsMenuState->ID = Beryll::GameStateID::PLAYER_TALENTS;
        playerTalentsMenuState->layerStack.pushOverlay(GUILayer);

        Beryll::GameStateMachine::pushState(playerTalentsMenuState);
    }

    void GameStateHelper::pushSettingsState()
    {
        std::shared_ptr<SettingsMenuGUILayer> GUILayer = std::make_shared<SettingsMenuGUILayer>();

        std::shared_ptr<Beryll::GameState> settingsMenuState = std::make_shared<Beryll::GameState>();
        settingsMenuState->ID = Beryll::GameStateID::SETTINGS;
        settingsMenuState->layerStack.pushOverlay(GUILayer);

        Beryll::GameStateMachine::pushState(settingsMenuState);
    }

    void GameStateHelper::popState()
    {
        Beryll::ParticleSystem::disableAll(); // Before state exited.

        Beryll::GameStateMachine::popState();
    }

    void GameStateHelper::resetAllVariables()
    {
        Beryll::Physics::hardRemoveAllObjects();
        EnAndVars::resetAllVariables();

        Sniper::sniperPositions = {};
        Sniper::spawnTime = 0.0f;

        Sounds::pistolShotTime = 0.0f;
        Sounds::pistolHitTime = 0.0f;
        Sounds::grenadeLauncherShotTime = 0.0f;
        Sounds::grenadeLauncherHitTime = 0.0f;
        Sounds::tankShotTime = 0.0f;
        Sounds::tankHitTime = 0.0f;
        Sounds::smashTime = 0.0f;
    }

    void GameStateHelper::prepareVariablesForPlay()
    {
        Beryll::Physics::enableSimulation();
        EnAndVars::mapStartTimeSec = Beryll::TimeStep::getSecFromStart();
    }

    void GameStateHelper::pauseGame()
    {
        Beryll::Physics::disableSimulation();
        EnAndVars::gameOnPause = true;
    }

    void GameStateHelper::resumeGame()
    {
        Beryll::Physics::enableSimulation();
        EnAndVars::gameOnPause = false;
    }
}
