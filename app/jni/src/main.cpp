#include "EngineHeaders.h"
#include "GameStateHelper.h"

int main(int argc, char* argv[])
{
    BR_INFO("%s", "main() started.");

    Beryll::GameLoop::create(Beryll::ScreenOrientation::MOBILE_PORTRAIT);

    //Beryll::GameLoop::setFPSLimit(15.0f);

    //Beryll::Camera::setCameraFov(45.0f);
    Beryll::Camera::setProjectionNearClipPlane(1.0f);
    Beryll::Camera::setProjectionFarClipPlane(1500.0f);
    Beryll::Camera::setObjectsViewDistance(500.0f);

    //Beryll::Physics::setResolution(1);
    //Beryll::Physics::setMinAcceptableFPS(20.0f);

    Beryll::Renderer::enableFaceCulling();

    MagneticBall3D::GameStateHelper::pushStartMenuState();
        
    Beryll::GameLoop::run();

    BR_INFO("%s", "main() finished.");
    return 0;
}
