#include "Map0Tutorial.h"
#include "enemies/StaticEnemy.h"
#include "Sounds.h"

namespace MagneticBall3D
{
    Map0Tutorial::Map0Tutorial(std::shared_ptr<PlayStateGUILayer> gui) : BaseMap(std::move(gui))
    {
        Beryll::LoadingScreen::showProgress(10.0f);

        // Specific for this map only.
        loadPlayer();
        m_player->getObj()->setOrigin(glm::vec3(0.0f, 2.0f,0.0f));
        Beryll::LoadingScreen::showProgress(20.0f);
        loadEnv();
        Beryll::LoadingScreen::showProgress(40.0f);
        loadGarbage();
        Beryll::LoadingScreen::showProgress(60.0f);
        loadEnemies();
        Beryll::LoadingScreen::showProgress(80.0f);

        loadShaders();
        handleCamera();

        m_dirToSun = glm::normalize(glm::vec3(1.0f, 3.5f, 0.0f));
        m_sunLightDir = -m_dirToSun;
        m_sunDistance = 500.0f;

        m_improvements = Improvements(m_player, {});
        m_skyBox = Beryll::Renderer::createSkyBox("skyboxes/map1");

        SendStatisticsHelper::sendMapStart();

        Beryll::LoadingScreen::showProgress(100.0f);
    }

    Map0Tutorial::~Map0Tutorial()
    {
        Sounds::stopBackgroundMusic();
    }

    void Map0Tutorial::updateBeforePhysics()
    {
        if(EnAndVars::gameOnPause)
            return;

        EnAndVars::mapPlayTimeSec += Beryll::TimeStep::getTimeStepSec();

        Sounds::update();

        handleScreenSwipe();
    }

    void Map0Tutorial::updateAfterPhysics()
    {
        if(EnAndVars::gameOnPause)
            return;

        for(const std::shared_ptr<Beryll::SceneObject>& so : m_animatedOrDynamicObjects)
        {
            if(so->getIsEnabledUpdate())
            {
                so->updateAfterPhysics();
            }
        }

        m_player->update();
        updateEnemiesAndTheirsAttacks();
        updateAndMagnetizeGarbage();
        killEnemies();
        handleCamera();
        updateGUI();
    }

    void Map0Tutorial::draw()
    {
        //BR_INFO("%s", "scene draw call");
        // 1. Draw into shadow map.
        glm::vec3 sunPos = m_player->getObj()->getOrigin() + (Beryll::Camera::getCameraFrontDirectionXZ() * 250.0f) + (m_dirToSun * m_sunDistance);
        updateSunPosition(sunPos, 700, 700, m_sunDistance * 2.0f);
        Beryll::Renderer::disableFaceCulling();
        m_shadowMap->drawIntoShadowMap(m_simpleObjForShadowMap, {}, m_sunLightVPMatrix);
        Beryll::Renderer::enableFaceCulling();

        // 2. Draw scene.
        glm::mat4 modelMatrix{1.0f};

        if(EnAndVars::gameOnPause || EnAndVars::improvementSystemOnScreen)
            m_ambientLight = 0.25f;
        else
            m_ambientLight = 0.7f;

        m_simpleObjSunLightShadows->bind();
        m_simpleObjSunLightShadows->set3Float("sunLightDir", m_sunLightDir);
        m_simpleObjSunLightShadows->set3Float("cameraPos", Beryll::Camera::getCameraPos());
        m_simpleObjSunLightShadows->set1Float("ambientLight", m_ambientLight);
        m_simpleObjSunLightShadows->set1Float("specularLightStrength", 1.0f);
        m_simpleObjSunLightShadows->set1Float("alphaTransparency", 1.0f);

        modelMatrix = m_player->getObj()->getModelMatrix();
        m_simpleObjSunLightShadows->setMatrix4x4Float("MVPLightMatrix", m_sunLightVPMatrix * modelMatrix);
        m_simpleObjSunLightShadows->setMatrix4x4Float("modelMatrix", modelMatrix);
        m_simpleObjSunLightShadows->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
        Beryll::Renderer::drawObject(m_player->getObj(), modelMatrix, m_simpleObjSunLightShadows);

        m_simpleObjSunLightShadowsNormals->bind();
        m_simpleObjSunLightShadowsNormals->set3Float("sunLightDir", m_sunLightDir);
        m_simpleObjSunLightShadowsNormals->set3Float("cameraPos", Beryll::Camera::getCameraPos());
        m_simpleObjSunLightShadowsNormals->set1Float("ambientLight", m_ambientLight);
        m_simpleObjSunLightShadowsNormals->set1Float("specularLightStrength", m_gui->sliderSpecularPower->getValue());

        modelMatrix = m_ground->getModelMatrix();
        m_simpleObjSunLightShadowsNormals->setMatrix4x4Float("MVPLightMatrix", m_sunLightVPMatrix * modelMatrix);
        m_simpleObjSunLightShadowsNormals->setMatrix4x4Float("modelMatrix", modelMatrix);
        m_simpleObjSunLightShadowsNormals->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
        Beryll::Renderer::drawObject(m_ground, modelMatrix, m_simpleObjSunLightShadowsNormals);

        m_skyBox->draw();
        Beryll::ParticleSystem::draw();
    }

    void Map0Tutorial::loadEnv()
    {
        m_ground = std::make_shared<Beryll::SimpleCollidingObject>("models3D/map0Tutorial/Ground.fbx",
                                                                            0.0f,
                                                                            false,
                                                                            Beryll::CollisionFlags::STATIC,
                                                                            Beryll::CollisionGroups::GROUND,
                                                                            Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE,
                                                                            Beryll::SceneObjectGroups::GROUND);

        m_ground->setFriction(EnAndVars::staticEnvFriction);
    }

    void Map0Tutorial::loadGarbage()
    {

    }

    void Map0Tutorial::loadEnemies()
    {

    }
}
