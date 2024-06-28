#include "EngineHeaders.h"
#include "GameStateHelper.h"
#include "DataBaseHelper.h"
#include "EnumsAndVariables.h"
#include "Sounds.h"

int main(int argc, char* argv[])
{
    BR_INFO("%s", "main() started.");

    Beryll::GameLoop::create(Beryll::ScreenOrientation::MOBILE_PORTRAIT);

    DataBaseHelper::prepareDatabase();

    Beryll::GameLoop::setFPSLimit(EnAndVars::SettingsMenu::FPSLimit);
    //Beryll::GameLoop::setFPSLimit(15.0f);

    //Beryll::Camera::setCameraFov(45.0f);
    Beryll::Camera::setProjectionNearClipPlane(5.0f);
    Beryll::Camera::setProjectionFarClipPlane(1600.0f);
    Beryll::Camera::setObjectsViewDistance(1000.0f);

    //Beryll::Physics::setResolution(1);
    //Beryll::Physics::setMinAcceptableFPS(20.0f);

    Beryll::LoadingScreen::setTextures({"loadingScreen/Map0Screen.jpg", "loadingScreen/Map1Screen.jpg", "loadingScreen/Map2Screen.jpg"});

    Beryll::Renderer::enableFaceCulling();

    MagneticBall3D::Sounds::loadSounds();

    //MagneticBall3D::GameStateHelper::pushPlayState();
    MagneticBall3D::GameStateHelper::pushStartMenuState();

    Beryll::GameLoop::run();

    BR_INFO("%s", "main() finished.");
    return 0;
}
