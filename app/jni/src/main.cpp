#include "EngineHeaders.h"
#include "playState/PlayStateGUILayer.h"
#include "playState/PlayStateSceneLayer.h"

int main(int argc, char* argv[])
{
    BR_INFO("%s", "main() started.");

    Beryll::GameLoop::create(Beryll::ScreenOrientation::MOBILE_PORTRAIT);

    //Beryll::GameLoop::setFPSLimit(10.0f);

    //Beryll::Camera::setCameraFov(45.0f);
    Beryll::Camera::setProjectionNearClipPlane(1.0f);
    Beryll::Camera::setProjectionFarClipPlane(1000.0f);

    Beryll::Physics::setResolution(2);

    std::shared_ptr<MagneticBall3D::PlayStateGUILayer> GUILayer = std::make_shared<MagneticBall3D::PlayStateGUILayer>();
    std::shared_ptr<MagneticBall3D::PlayStateSceneLayer> sceneLayer = std::make_shared<MagneticBall3D::PlayStateSceneLayer>(GUILayer);

    std::shared_ptr<Beryll::GameState> playState = std::make_shared<Beryll::GameState>();
    playState->ID = Beryll::GameStateID::PLAY;
    playState->layerStack.pushLayer(sceneLayer);
    playState->layerStack.pushOverlay(GUILayer);

    Beryll::GameStateMachine::pushState(playState);
        
    Beryll::GameLoop::run();

    BR_INFO("%s", "main() finished.");
    return 0;
}
