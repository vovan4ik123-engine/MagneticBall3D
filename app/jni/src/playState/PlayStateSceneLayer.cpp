#include "PlayStateSceneLayer.h"
#include "EnumsAndVariables.h"

namespace MagneticBall3D
{
    PlayStateSceneLayer::PlayStateSceneLayer(std::shared_ptr<PlayStateGUILayer> gui)
    {
        m_ID = Beryll::LayerID::PLAY_SCENE;
        
        m_gui = std::move(gui);

        Beryll::Physics::hardRemoveAllObjects();

        loadPlayerAndStaticEnv();
        loadDynamicEnv();
        loadShaders();
        loadSunPosition(glm::vec3(200.0f, 200.0f, -200.0f), 500.0f, 500.0f, 600.0f);

        m_cameraFront = m_player->getOrigin();
        m_cameraFront.y += 15.0f;
        m_cameraOffset = glm::normalize(m_cameraOffset);
        Beryll::Camera::setCameraPos(m_cameraFront + m_cameraOffset * m_cameraDistance);
        Beryll::Camera::setCameraFrontPos(m_cameraFront);

        //BR_INFO(" X:%f Y:%f Z:%f", .x, .y, .z);
        //BR_INFO("%s", "");
        //BR_INFO(" %f", );
    }

    PlayStateSceneLayer::~PlayStateSceneLayer()
    {

    }

    void PlayStateSceneLayer::updateBeforePhysics()
    {
        controlPlayer();

        if(m_gui->sliderFPS->getIsValueChanging())
            Beryll::GameLoop::setFPSLimit(m_gui->sliderFPS->getValue());

    }

    void PlayStateSceneLayer::updateAfterPhysics()
    {
        for(const std::shared_ptr<Beryll::SceneObject>& so : m_allSceneObjects)
        {
            if(so->getIsEnabledUpdate())
            {
                so->updateAfterPhysics();
            }
        }

        rotateCamera();

        m_cameraFront = m_player->getOrigin();
        m_cameraFront.y += 15.0f;
        Beryll::Camera::setCameraPos(m_cameraFront + m_cameraOffset * m_cameraDistance);
        Beryll::Camera::setCameraFrontPos(m_cameraFront);
    }

    void PlayStateSceneLayer::draw()
    {
        //BR_INFO("%s", "scene draw call");
        // 1. Draw into shadow map.
        Beryll::Renderer::disableFaceCulling();
        m_shadowMap->drawIntoShadowMap(m_simpleObjForShadowMap, {}, m_sunLightVPMatrix);
        Beryll::Renderer::enableFaceCulling();

        // 2. Draw scene.
        glm::mat4 modelMatrix{1.0f};

        m_simpleObjSunLightShadows->bind();
        m_simpleObjSunLightShadows->set3Float("sunLightDir", m_sunLightDir);
        m_simpleObjSunLightShadows->set3Float("cameraPos", Beryll::Camera::getCameraPos());
        m_simpleObjSunLightShadows->set1Float("ambientLight", 0.5f);
        m_simpleObjSunLightShadows->set1Float("specularLightStrength", 1.0f);


        modelMatrix = m_player->getModelMatrix();
        m_simpleObjSunLightShadows->setMatrix4x4Float("MVPLightMatrix", m_sunLightVPMatrix * modelMatrix);
        m_simpleObjSunLightShadows->setMatrix4x4Float("modelMatrix", modelMatrix);
        m_simpleObjSunLightShadows->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
        Beryll::Renderer::drawObject(m_player, modelMatrix, m_simpleObjSunLightShadows);

        for(const auto& obj : m_allGarbage)
        {
            modelMatrix = obj->getModelMatrix();
            m_simpleObjSunLightShadows->setMatrix4x4Float("MVPLightMatrix", m_sunLightVPMatrix * modelMatrix);
            m_simpleObjSunLightShadows->setMatrix4x4Float("modelMatrix", modelMatrix);
            m_simpleObjSunLightShadows->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
            Beryll::Renderer::drawObject(obj, modelMatrix, m_simpleObjSunLightShadows);
        }

        m_simpleObjSunLightShadowsNormals->bind();
        m_simpleObjSunLightShadowsNormals->set3Float("sunLightDir", m_sunLightDir);
        m_simpleObjSunLightShadowsNormals->set3Float("cameraPos", Beryll::Camera::getCameraPos());
        m_simpleObjSunLightShadowsNormals->set1Float("ambientLight", 1.0f);
        m_simpleObjSunLightShadowsNormals->set1Float("specularLightStrength", 1.0f);

        modelMatrix = m_ground->getModelMatrix();
        m_simpleObjSunLightShadowsNormals->setMatrix4x4Float("MVPLightMatrix", m_sunLightVPMatrix * modelMatrix);
        m_simpleObjSunLightShadowsNormals->setMatrix4x4Float("modelMatrix", modelMatrix);
        m_simpleObjSunLightShadowsNormals->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
        Beryll::Renderer::drawObject(m_ground, modelMatrix, m_simpleObjSunLightShadowsNormals);
    }

    void PlayStateSceneLayer::loadPlayerAndStaticEnv()
    {
        m_player = std::make_shared<Beryll::SimpleCollidingObject>("models3D/player/Player.fbx",
                                                                      10.0f,
                                                                      true,
                                                                      Beryll::CollisionFlags::DYNAMIC,
                                                                      Beryll::CollisionGroups::PLAYER,
                                                                      Beryll::CollisionGroups::STATIC_ENVIRONMENT | Beryll::CollisionGroups::DYNAMIC_ENVIRONMENT,
                                                                      Beryll::SceneObjectGroups::PLAYER);

        m_allSceneObjects.push_back(m_player);
        m_simpleObjForShadowMap.push_back(m_player);

        //m_player->setOrigin(glm::vec3(-140.0f, 5.0f,-140.0f));
        m_player->setGravity(glm::vec3(0.0f, -70.0f, 0.0f));
        m_player->setFriction(EnumsAndVariables::playerFriction);
        m_player->setDamping(EnumsAndVariables::playerDamping, EnumsAndVariables::playerDamping);

        // Static env.
        m_ground = std::make_shared<Beryll::SimpleCollidingObject>("models3D/Ground.fbx",
                                                                         0.0f,
                                                                         false,
                                                                         Beryll::CollisionFlags::STATIC,
                                                                         Beryll::CollisionGroups::STATIC_ENVIRONMENT,
                                                                         Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::DYNAMIC_ENVIRONMENT,
                                                                         Beryll::SceneObjectGroups::STATIC_ENVIRONMENT);

        m_allSceneObjects.push_back(m_ground);
        m_simpleObjForShadowMap.push_back(m_ground);

        m_ground->setFriction(20.0f);
    }

    void PlayStateSceneLayer::loadDynamicEnv()
    {
        const auto objects1 = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/Garbage1.fbx",
                                                                                       1.0f,
                                                                                       false,
                                                                                       Beryll::CollisionFlags::DYNAMIC,
                                                                                       Beryll::CollisionGroups::DYNAMIC_ENVIRONMENT,
                                                                                       Beryll::CollisionGroups::STATIC_ENVIRONMENT | Beryll::CollisionGroups::PLAYER |
                                                                                               Beryll::CollisionGroups::DYNAMIC_ENVIRONMENT,
                                                                                       Beryll::SceneObjectGroups::DYNAMIC_ENVIRONMENT);

        for(const auto& obj : objects1)
        {
            m_allSceneObjects.push_back(obj);
            m_allGarbage.push_back(obj);
            m_simpleObjForShadowMap.push_back(obj);
        }

        const auto objects2 = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/Garbage2.fbx",
                                                                                       1.0f,
                                                                                       false,
                                                                                       Beryll::CollisionFlags::DYNAMIC,
                                                                                       Beryll::CollisionGroups::DYNAMIC_ENVIRONMENT,
                                                                                       Beryll::CollisionGroups::STATIC_ENVIRONMENT | Beryll::CollisionGroups::PLAYER |
                                                                                       Beryll::CollisionGroups::DYNAMIC_ENVIRONMENT,
                                                                                       Beryll::SceneObjectGroups::DYNAMIC_ENVIRONMENT);

        for(const auto& obj : objects2)
        {
            m_allSceneObjects.push_back(obj);
            m_allGarbage.push_back(obj);
            m_simpleObjForShadowMap.push_back(obj);
        }

        const auto objects3 = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/Garbage3.fbx",
                                                                                       1.0f,
                                                                                       false,
                                                                                       Beryll::CollisionFlags::DYNAMIC,
                                                                                       Beryll::CollisionGroups::DYNAMIC_ENVIRONMENT,
                                                                                       Beryll::CollisionGroups::STATIC_ENVIRONMENT | Beryll::CollisionGroups::PLAYER |
                                                                                       Beryll::CollisionGroups::DYNAMIC_ENVIRONMENT,
                                                                                       Beryll::SceneObjectGroups::DYNAMIC_ENVIRONMENT);

        for(const auto& obj : objects3)
        {
            m_allSceneObjects.push_back(obj);
            m_allGarbage.push_back(obj);
            m_simpleObjForShadowMap.push_back(obj);
        }
    }

    void PlayStateSceneLayer::loadShaders()
    {
        m_simpleObjSunLightShadows = Beryll::Renderer::createShader("shaders/GLES/SimpleObjectSunLightShadows.vert",
                                                                    "shaders/GLES/SimpleObjectSunLightShadows.frag");
        m_simpleObjSunLightShadows->bind();
        m_simpleObjSunLightShadows->activateDiffuseTextureMat1();
        m_simpleObjSunLightShadows->activateShadowMapTexture();

        m_simpleObjSunLightShadowsNormals = Beryll::Renderer::createShader("shaders/GLES/SimpleObjectSunLightShadowsNormals.vert",
                                                                           "shaders/GLES/SimpleObjectSunLightShadowsNormals.frag");
        m_simpleObjSunLightShadowsNormals->bind();
        m_simpleObjSunLightShadowsNormals->activateDiffuseTextureMat1();
        m_simpleObjSunLightShadowsNormals->activateNormalMapTextureMat1();
        m_simpleObjSunLightShadowsNormals->activateShadowMapTexture();

        m_shadowMap = Beryll::Renderer::createShadowMap((1024 * 4) - 1, (1024 * 4) - 1);
    }

    void PlayStateSceneLayer::loadSunPosition(const glm::vec3& pos, float clipCubeWidth, float clipCubeHeight, float clipCubeDepth)
    {
        m_sunLightDir = glm::normalize(-pos);

        glm::mat4 lightProjection = glm::ortho(-clipCubeWidth * 0.5f, clipCubeWidth * 0.5f, -clipCubeHeight * 0.5f, clipCubeHeight * 0.5f, 1.0f, clipCubeDepth);
        glm::mat4 lightView = glm::lookAt(pos, pos + m_sunLightDir, BeryllConstants::worldUp);

        m_sunLightVPMatrix = lightProjection * lightView;
    }

    void PlayStateSceneLayer::controlPlayer()
    {
        m_playerMoveDir = glm::normalize(m_player->getLinearVelocity());
        //BR_INFO("moveDir X:%f Y:%f Z:%f", m_playerMoveDir.x, m_playerMoveDir.y, m_playerMoveDir.z);

        m_playerMoveSpeed = glm::length(m_player->getLinearVelocity());
        if(glm::isnan(m_playerMoveSpeed))
        {
            m_playerMoveSpeed = 0.0f;
        }

        //if(m_playerMoveSpeed > EnumsAndVariables::playerMaxSpeed)
        //    return;

        const std::vector<Beryll::Finger>& fingers = Beryll::EventHandler::getFingers();
        bool fingerStillOnScreen = false;
        for(const Beryll::Finger& f : fingers)
        {
            if(f.handled)
                continue;

            if(f.downEvent && m_fingerDownID == -1)
            {
                m_fingerDownID = f.ID;
                m_fingerDownPos = f.SDL2ScreenPos;
            }

            if(f.ID == m_fingerDownID)
            {
                fingerStillOnScreen = true;
                //BR_INFO("%s", "fingerStillOnScreen = true");
                m_fingerUpPos = f.SDL2ScreenPos;
                break;
            }
        }

        if(!fingerStillOnScreen && m_fingerDownID != -1)
        {
            m_fingerDownID = -1;

            if(glm::distance(m_fingerUpPos, m_fingerDownPos) < 0.01f)
                return;

            glm::vec2 screenSwipe = (m_fingerUpPos - m_fingerDownPos);
            //BR_INFO("Swipe dir X:%f Z:%f", -swipeDir.y, swipeDir.x);
            glm::vec3 impulseScreenDir{-screenSwipe.y, 0.0f, screenSwipe.x};
            float impulseLength = glm::length(impulseScreenDir);

            glm::qua cameraRotFromStartDir = glm::normalize(glm::rotation(m_startDir, Beryll::Camera::getCameraFrontDirectionXZ()));
            impulseScreenDir = cameraRotFromStartDir * glm::normalize(impulseScreenDir);
            impulseScreenDir *= impulseLength;

            m_player->applyCentralImpulse(impulseScreenDir * EnumsAndVariables::playerImpulseFactor);

            // Torque applied along right/left vector from impulse.
            glm::vec3 impulseLeft = glm::cross(BeryllConstants::worldUp, glm::normalize(impulseScreenDir));
            impulseLeft = glm::normalize(impulseLeft) * impulseLength;
            m_player->applyTorqueImpulse(impulseLeft * EnumsAndVariables::playerTorqueFactor);
        }
    }

    void PlayStateSceneLayer::rotateCamera()
    {
        BR_INFO("m_playerMoveSpeed %f", m_playerMoveSpeed);

        if(glm::isnan(m_playerMoveSpeed) || m_playerMoveSpeed < EnumsAndVariables::minPlayerSpeedToCameraFollow)
            return;

        const glm::vec3 playerBackXZ = -glm::normalize(glm::vec3(m_playerMoveDir.x, 0.0f, m_playerMoveDir.z));
        if(glm::any(glm::isnan(playerBackXZ)))
            return;
        //BR_INFO("playerMoveBackXZ X:%f Y:%f Z:%f", playerBackXZ.x, playerBackXZ.y, playerBackXZ.z);

        const glm::vec3 cameraBackXZ = Beryll::Camera::getCameraBackDirectionXZ();
        if(glm::any(glm::isnan(cameraBackXZ)))
            return;
        //BR_INFO("cameraBackXZ X:%f Y:%f Z:%f", cameraBackXZ.x, cameraBackXZ.y, cameraBackXZ.z);

        const glm::quat rotation = glm::rotation(cameraBackXZ, playerBackXZ);

        const float angleDifference = glm::angle(rotation);
        if(angleDifference < 0.035f) // Less than 2 degrees.
            return;

        const glm::vec3 axis = glm::normalize(glm::axis(rotation));

        float maxAngleToRotate = EnumsAndVariables::cameraRotationMaxSpeed * Beryll::TimeStep::getTimeStepSec();
        if(angleDifference < maxAngleToRotate)
            maxAngleToRotate = angleDifference;
        const glm::mat4 rotateMatr = glm::rotate(glm::mat4{1.0f}, maxAngleToRotate, axis);

        m_cameraOffset = rotateMatr * glm::vec4(cameraBackXZ, 1.0f);
        m_cameraOffset.y = 0.0f;
        m_cameraOffset = glm::normalize(m_cameraOffset);
        m_cameraOffset.y = 0.2f;
        m_cameraOffset = glm::normalize(m_cameraOffset);
    }
}
