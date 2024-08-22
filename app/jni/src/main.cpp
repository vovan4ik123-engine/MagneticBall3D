#include "EngineHeaders.h"
#include "GameStateHelper.h"
#include "DataBaseHelper.h"
#include "EnumsAndVariables.h"
#include "Sounds.h"

// For static model in blender 36.6k triangles = 1mb FBX file size.

int main(int argc, char* argv[])
{
    BR_INFO("%s", "main() started.");

    Beryll::GameLoop::create(Beryll::ScreenOrientation::MOBILE_PORTRAIT);

    DataBaseHelper::prepareDatabase();

    Beryll::GameLoop::setFPSLimit(EnumsAndVars::SettingsMenu::FPSLimit);
    //Beryll::GameLoop::setFPSLimit(15.0f);

    //Beryll::Camera::setCameraFov(45.0f);
    Beryll::Camera::setProjectionNearClipPlane(5.0f);
    Beryll::Camera::setProjectionFarClipPlane(1800.0f);
    Beryll::Camera::setObjectsViewDistance(1500.0f);

    //Beryll::Physics::setResolution(1);
    //Beryll::Physics::setMinAcceptableFPS(20.0f);

    Beryll::LoadingScreen::setTextures({"loadingScreen/Map0Screen.jpg",
                                        "loadingScreen/Map3Screen.jpg",
                                        "loadingScreen/Map4Screen.jpg",
                                        "loadingScreen/Map2Screen.jpg",
                                        "loadingScreen/Map1Screen.jpg"});

    Beryll::Renderer::enableFaceCulling();

    MagneticBall3D::Sounds::loadSounds();

    //MagneticBall3D::GameStateHelper::pushPlayState();
    MagneticBall3D::GameStateHelper::pushStartMenuState();

    Beryll::GameLoop::run();

    BR_INFO("%s", "main() finished.");
    return 0;
}
