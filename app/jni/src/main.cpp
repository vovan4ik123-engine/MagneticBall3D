#include "EngineHeaders.h"
#include "GameStateHelper.h"
#include "DataBaseHelper.h"
#include "EnumsAndVariables.h"

int main(int argc, char* argv[])
{
    BR_INFO("%s", "main() started.");

    Beryll::GameLoop::create(Beryll::ScreenOrientation::MOBILE_PORTRAIT);

    DataBaseHelper::prepareDatabase();

    //Beryll::GameLoop::setFPSLimit(EnumsAndVariables::SettingsMenu::FPSLimit);
    //Beryll::GameLoop::setFPSLimit(15.0f);

    //Beryll::Camera::setCameraFov(45.0f);
    Beryll::Camera::setProjectionNearClipPlane(5.0f);
    Beryll::Camera::setProjectionFarClipPlane(1800.0f);
    Beryll::Camera::setObjectsViewDistance(1000.0f);

    //Beryll::Physics::setResolution(1);
    //Beryll::Physics::setMinAcceptableFPS(20.0f);

    Beryll::Renderer::enableFaceCulling();

    MagneticBall3D::GameStateHelper::pushPlayState();

    Beryll::GameLoop::run();

    BR_INFO("%s", "main() finished.");
    return 0;
}
