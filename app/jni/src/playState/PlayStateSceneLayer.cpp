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
        loadAnimatedModels();
        loadShaders();
        loadSunPosition(glm::vec3(200.0f, 200.0f, 20.0f), 500.0f, 500.0f, 500.0f);

        handleCamera();

        m_updateAfterPhysics = [](std::vector<std::shared_ptr<Beryll::SceneObject>>& v, int begin, int end) -> void // -> void = return type.
        {
            for(int i = begin; i < end; ++i)
            {
                if(v[i]->getIsEnabledUpdate())
                {
                    v[i]->updateAfterPhysics();

                    if(v[i]->getOrigin().y < -500.0f)
                        v[i]->disableForEver();
                }
            }
        };

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
        updateGarbageGravity();

//        if(m_gui->sliderFPS->getIsValueChanging())
//            Beryll::GameLoop::setFPSLimit(m_gui->sliderFPS->getValue());

        if(m_gui->sliderEnemy->getIsValueChanging())
        {
            for(int i = 0; i < m_allAnimated.size(); ++i)
            {
                if(i < int(m_gui->sliderEnemy->getValue()))
                {
                    m_allAnimated[i]->enableDraw();
                    m_allAnimated[i]->enableUpdate();
                    m_allAnimated[i]->enableCollisionMesh();
                }
                else
                {
                    m_allAnimated[i]->disableDraw();
                    m_allAnimated[i]->disableUpdate();
                    m_allAnimated[i]->disableCollisionMesh();
                }
            }
        }
    }

    void PlayStateSceneLayer::updateAfterPhysics()
    {
        for(const std::shared_ptr<Beryll::SceneObject>& so : m_allSceneObjects)
        {
            if(so->getIsEnabledUpdate())
            {
                so->updateAfterPhysics();

                if(so->getOrigin().y < -500.0f)
                    so->disableForEver();
            }
        }

        //Beryll::AsyncRun::Run(m_allSceneObjects, m_updateAfterPhysics);

        updatePlayerGravity();
        checkPlayerSpeed();
        // Last call before draw.
        handleCamera();
    }

    void PlayStateSceneLayer::draw()
    {
        //BR_INFO("%s", "scene draw call");
        // 1. Draw into shadow map.
        //Beryll::Renderer::disableFaceCulling();
        m_shadowMap->drawIntoShadowMap(m_simpleObjForShadowMap, m_animatedObjForShadowMap, m_sunLightVPMatrix);
        //Beryll::Renderer::enableFaceCulling();

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
            if(obj->getIsEnabledDraw())
            {
                modelMatrix = obj->getModelMatrix();
                m_simpleObjSunLightShadows->setMatrix4x4Float("MVPLightMatrix", m_sunLightVPMatrix * modelMatrix);
                m_simpleObjSunLightShadows->setMatrix4x4Float("modelMatrix", modelMatrix);
                m_simpleObjSunLightShadows->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
                Beryll::Renderer::drawObject(obj, modelMatrix, m_simpleObjSunLightShadows);
            }
        }

        for(const auto& staticObj : m_allStaticEnv)
        {
            if(staticObj->getIsEnabledDraw())
            {
                modelMatrix = staticObj->getModelMatrix();
                m_simpleObjSunLightShadows->setMatrix4x4Float("MVPLightMatrix", m_sunLightVPMatrix * modelMatrix);
                m_simpleObjSunLightShadows->setMatrix4x4Float("modelMatrix", modelMatrix);
                m_simpleObjSunLightShadows->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
                Beryll::Renderer::drawObject(staticObj, modelMatrix, m_simpleObjSunLightShadows);
            }
        }

        m_animatedObjSunLightShadows->bind();
        m_animatedObjSunLightShadows->set3Float("sunLightDir", m_sunLightDir);
        m_animatedObjSunLightShadows->set3Float("cameraPos", Beryll::Camera::getCameraPos());
        m_animatedObjSunLightShadows->set1Float("ambientLight", 0.5f);
        m_animatedObjSunLightShadows->set1Float("specularLightStrength", 5.0f);

        for(const auto& animObj : m_allAnimated)
        {
            if(animObj->getIsEnabledDraw())
            {
                modelMatrix = animObj->getModelMatrix();
                m_animatedObjSunLightShadows->setMatrix4x4Float("MVPLightMatrix", m_sunLightVPMatrix * modelMatrix);
                m_animatedObjSunLightShadows->setMatrix4x4Float("modelMatrix", modelMatrix);
                m_animatedObjSunLightShadows->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
                Beryll::Renderer::drawObject(animObj, modelMatrix, m_animatedObjSunLightShadows);
            }
        }
    }

    void PlayStateSceneLayer::loadPlayerAndStaticEnv()
    {
        m_player = std::make_shared<Beryll::SimpleCollidingObject>("models3D/player/Player.fbx",
                                                                   EnumsAndVariables::playerMass,
                                                                   true,
                                                                   Beryll::CollisionFlags::DYNAMIC,
                                                                   Beryll::CollisionGroups::PLAYER,
                                                                   Beryll::CollisionGroups::GROUND | Beryll::CollisionGroups::BUILDING |
                                                                   Beryll::CollisionGroups::DYNAMIC_ENVIRONMENT,
                                                                   Beryll::SceneObjectGroups::PLAYER);

        m_allSceneObjects.push_back(m_player);
        m_simpleObjForShadowMap.push_back(m_player);

        //m_player->setOrigin(glm::vec3(-140.0f, 5.0f,-140.0f));
        m_player->setGravity(EnumsAndVariables::playerGravityOnGround);
        m_player->setFriction(EnumsAndVariables::playerFriction);
        m_player->setDamping(EnumsAndVariables::playerDamping, EnumsAndVariables::playerDamping);

        // Static env.
        const auto ground = std::make_shared<Beryll::SimpleCollidingObject>("models3D/Ground.fbx",
                                                                   0.0f,
                                                                   false,
                                                                   Beryll::CollisionFlags::STATIC,
                                                                   Beryll::CollisionGroups::GROUND,
                                                                   Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::DYNAMIC_ENVIRONMENT,
                                                                   Beryll::SceneObjectGroups::GROUND);

        m_allSceneObjects.push_back(ground);
        m_allStaticEnv.push_back(ground);
        m_simpleObjForShadowMap.push_back(ground);
        ground->setFriction(EnumsAndVariables::staticEnvFriction);

        const auto objects1 = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/Buildings.fbx",
                                                                                       0.0f,
                                                                                       false,
                                                                                       Beryll::CollisionFlags::STATIC,
                                                                                       Beryll::CollisionGroups::BUILDING,
                                                                                       Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::CAMERA |
                                                                                       Beryll::CollisionGroups::DYNAMIC_ENVIRONMENT,
                                                                                       Beryll::SceneObjectGroups::BUILDING);

        for(const auto& obj : objects1)
        {
            m_allSceneObjects.push_back(obj);
            m_allStaticEnv.push_back(obj);
            m_simpleObjForShadowMap.push_back(obj);
            obj->setFriction(EnumsAndVariables::staticEnvFriction);
        }
    }

    void PlayStateSceneLayer::loadDynamicEnv()
    {
        const auto objects1 = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/Garbage1.fbx",
                                                                                       EnumsAndVariables::garbageMass,
                                                                                       false,
                                                                                       Beryll::CollisionFlags::DYNAMIC,
                                                                                       Beryll::CollisionGroups::DYNAMIC_ENVIRONMENT,
                                                                                       Beryll::CollisionGroups::GROUND | Beryll::CollisionGroups::BUILDING |
                                                                                       Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::DYNAMIC_ENVIRONMENT,
                                                                                       Beryll::SceneObjectGroups::DYNAMIC_ENVIRONMENT);

        for(const auto& obj : objects1)
        {
            m_allSceneObjects.push_back(obj);
            m_allGarbage.push_back(obj);
            m_simpleObjForShadowMap.push_back(obj);

            obj->setDamping(EnumsAndVariables::garbageDamping, EnumsAndVariables::garbageDamping);
        }

        const auto objects2 = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/Garbage2.fbx",
                                                                                       EnumsAndVariables::garbageMass,
                                                                                       false,
                                                                                       Beryll::CollisionFlags::DYNAMIC,
                                                                                       Beryll::CollisionGroups::DYNAMIC_ENVIRONMENT,
                                                                                       Beryll::CollisionGroups::GROUND | Beryll::CollisionGroups::BUILDING |
                                                                                       Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::DYNAMIC_ENVIRONMENT,
                                                                                       Beryll::SceneObjectGroups::DYNAMIC_ENVIRONMENT);

        for(const auto& obj : objects2)
        {
            m_allSceneObjects.push_back(obj);
            m_allGarbage.push_back(obj);
            m_simpleObjForShadowMap.push_back(obj);

            obj->setDamping(EnumsAndVariables::garbageDamping, EnumsAndVariables::garbageDamping);
        }

        const auto objects3 = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/Garbage3.fbx",
                                                                                       EnumsAndVariables::garbageMass,
                                                                                       false,
                                                                                       Beryll::CollisionFlags::DYNAMIC,
                                                                                       Beryll::CollisionGroups::DYNAMIC_ENVIRONMENT,
                                                                                       Beryll::CollisionGroups::GROUND | Beryll::CollisionGroups::BUILDING |
                                                                                       Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::DYNAMIC_ENVIRONMENT,
                                                                                       Beryll::SceneObjectGroups::DYNAMIC_ENVIRONMENT);

        for(const auto& obj : objects3)
        {
            m_allSceneObjects.push_back(obj);
            m_allGarbage.push_back(obj);
            m_simpleObjForShadowMap.push_back(obj);

            obj->setDamping(EnumsAndVariables::garbageDamping, EnumsAndVariables::garbageDamping);
        }
    }

    void PlayStateSceneLayer::loadAnimatedModels()
    {
        float xPos = -50.0f;
        float distanceBetweenUnits = 20.0f;

        for(int x = 0; x < 20; ++x)
        {
            float zPos = -50.0f;
            for(int z = 0; z < 20; ++z)
            {
                auto unit = std::make_shared<Beryll::AnimatedCollidingCharacter>("models3D/AnimCollFootman1.fbx",
                                                                                 0.0f,
                                                                                 false,
                                                                                 Beryll::CollisionFlags::STATIC,
                                                                                 Beryll::CollisionGroups::CONSTRUCTION_1,
                                                                                 Beryll::CollisionGroups::CONSTRUCTION_1,
                                                                                 Beryll::SceneObjectGroups::CONSTRUCTION_1);

                unit->setOrigin(glm::vec3(xPos, unit->getController().getFromOriginToBottom(), zPos));
                unit->setCurrentAnimationByIndex(3, false, false);
                unit->setDefaultAnimationByIndex(3);

                m_allSceneObjects.push_back(unit);
                m_allAnimated.push_back(unit);
                m_animatedObjForShadowMap.push_back(unit);

                zPos -= distanceBetweenUnits;
            }

            xPos -= distanceBetweenUnits;
        }

        for(int i = 0; i < m_allAnimated.size(); ++i)
        {
            if(i < int(m_gui->sliderEnemy->getValue()))
            {
                m_allAnimated[i]->enableDraw();
                m_allAnimated[i]->enableUpdate();
                m_allAnimated[i]->enableCollisionMesh();
            }
            else
            {
                m_allAnimated[i]->disableDraw();
                m_allAnimated[i]->disableUpdate();
                m_allAnimated[i]->disableCollisionMesh();
            }
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

        m_animatedObjSunLightShadows = Beryll::Renderer::createShader("shaders/GLES/AnimatedObjectSunLightShadows.vert",
                                                                      "shaders/GLES/AnimatedObjectSunLightShadows.frag");
        m_animatedObjSunLightShadows->bind();
        m_animatedObjSunLightShadows->activateDiffuseTextureMat1();
        m_animatedObjSunLightShadows->activateShadowMapTexture();

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

            if(glm::distance(m_fingerUpPos, m_fingerDownPos) < 1.0f)
                return;

            glm::vec2 screenSwipe = (m_fingerUpPos - m_fingerDownPos);
            m_screenSwipeDir = glm::vec3{-screenSwipe.y, 0.0f, screenSwipe.x};
            float impulseLength = glm::length(m_screenSwipeDir);

            glm::qua cameraRotFromStartDir = glm::normalize(glm::rotation(m_startDir, Beryll::Camera::getCameraFrontDirectionXZ()));
            m_screenSwipeDir = cameraRotFromStartDir * glm::normalize(m_screenSwipeDir);
            m_screenSwipeDir *= impulseLength;

            m_player->applyCentralImpulse(m_screenSwipeDir * m_gui->sliderImpulse->getValue());

            float playerMassToGarbageMassRation = EnumsAndVariables::playerMass / EnumsAndVariables::garbageMass;
            playerMassToGarbageMassRation = 1.0f / playerMassToGarbageMassRation;
            for(const auto& obj : m_allGarbage)
            {
                if(glm::distance(m_player->getOrigin(), obj->getOrigin()) < EnumsAndVariables::playerMagneticRadius)
                {
                    obj->applyCentralImpulse((m_screenSwipeDir * m_gui->sliderImpulse->getValue()) * playerMassToGarbageMassRation);
                }
            }

            // Torque applied along right/left vector from impulse.
            glm::vec3 impulseLeft = glm::cross(BeryllConstants::worldUp, glm::normalize(m_screenSwipeDir));
            impulseLeft = glm::normalize(impulseLeft) * impulseLength;
            m_player->applyTorqueImpulse(impulseLeft * m_gui->sliderTorque->getValue());
        }
    }

    void PlayStateSceneLayer::updateGarbageGravity()
    {
        m_objectsInMagneticRadius = 0;

        for(const auto& obj : m_allGarbage)
        {
            if(glm::distance(m_player->getOrigin(), obj->getOrigin()) < EnumsAndVariables::playerMagneticRadius)
            {
                ++m_objectsInMagneticRadius;

                glm::vec3 grav = glm::normalize(m_player->getOrigin() - obj->getOrigin()) * m_gui->sliderGGrav->getValue();
                if(Beryll::Physics::getIsCollision(m_player->getID(), obj->getID()))
                {
                    obj->setGravity(-(grav * 0.5f), false, false);
                }
                else
                {
                    obj->setGravity(grav, false, true);
                }

                // Stop garbage if it stats rotating around player too fast.
                const glm::vec3 linVelocity = obj->getLinearVelocity();
                const glm::vec3 objMoveDir = glm::normalize(linVelocity);
                const float objSpeed = glm::length(linVelocity);
                const glm::vec3 objToPlayerDir = glm::normalize(m_player->getOrigin() - obj->getOrigin());

                if(objSpeed > m_playerMoveSpeed * 2.0f && BeryllUtils::Common::getAngleInRadians(objToPlayerDir, objMoveDir) > 0.35f) // > 20 degrees.
                {
                    obj->resetVelocities();
                }
            }
            else
            {
                obj->setGravity(EnumsAndVariables::garbageGravityDefault, false, false);
            }
        }
    }

    void PlayStateSceneLayer::updatePlayerGravity()
    {
        if(Beryll::Physics::getIsCollisionWithGroup(m_player->getID(), Beryll::CollisionGroups::BUILDING))
        {
            m_lastTimeOnBuilding = Beryll::TimeStep::getSecFromStart();
            m_player->setGravity(EnumsAndVariables::playerGravityOnBuilding);
        }
        else if(Beryll::Physics::getIsCollisionWithGroup(m_player->getID(), Beryll::CollisionGroups::GROUND))
        {
            m_player->setGravity(EnumsAndVariables::playerGravityOnGround);
        }
        else if(m_lastTimeOnBuilding + m_applyGravityDelay < Beryll::TimeStep::getSecFromStart())
        {
            //m_player->setGravity(EnumsAndVariables::playerGravityOnAir);
            m_player->setGravity(glm::vec3(0.0f, m_gui->sliderPGrav->getValue(), 0.0f));
        }
    }

    void PlayStateSceneLayer::checkPlayerSpeed()
    {
        // Prefer check m_playerMoveSpeed > 0.0f before use m_playerMoveDir.
        m_playerMoveSpeed = glm::length(m_player->getLinearVelocity());
        if(glm::isnan(m_playerMoveSpeed) || m_playerMoveSpeed == 0.0f)
        {
            m_playerMoveSpeed = 0.0f;
            m_playerMoveDir = glm::vec3{0.0f};
        }
        else
        {
            m_playerMoveDir = glm::normalize(m_player->getLinearVelocity());
        }
        //BR_INFO("moveDir X:%f Y:%f Z:%f", m_playerMoveDir.x, m_playerMoveDir.y, m_playerMoveDir.z);
    }

    void PlayStateSceneLayer::handleCamera()
    {
        if(Beryll::Physics::getIsCollisionWithGroup(m_player->getID(), Beryll::CollisionGroups::GROUND))
            rotateCameraToPlayerMoveDir();
        else
            rotateCameraOnBuilding();

        m_cameraFront = m_player->getOrigin();
        m_cameraFront.y += 15.0f;

        float maxCameraDistance = m_startCameraDistance + m_objectsInMagneticRadius * 0.15f;
        glm::vec3 cameraPosForRay = m_cameraFront + m_cameraOffset * (maxCameraDistance + 2.0f); // + 2m behind camera.

        // Check camera ray collisions.
        Beryll::RayClosestHit rayClosestHit = Beryll::Physics::castRayClosestHit(m_cameraFront,
                                                                                 cameraPosForRay,
                                                                                 Beryll::CollisionGroups::CAMERA,
                                                                                 Beryll::CollisionGroups::BUILDING);

        if(rayClosestHit)
        {
            float hitDistance = glm::max(glm::length(m_cameraFront - rayClosestHit.hitPoint), 5.0f);
            if(hitDistance < maxCameraDistance)
            {
                m_cameraDistance = hitDistance;
            }
        }
        else
        {
            if(m_cameraDistance < maxCameraDistance)
            {
                m_cameraDistance += EnumsAndVariables::cameraZoomMaxSpeed * Beryll::TimeStep::getTimeStepSec();
            }
        }

        if(m_cameraDistance > maxCameraDistance)
            m_cameraDistance = maxCameraDistance;

        Beryll::Camera::setCameraPos(m_cameraFront + m_cameraOffset * m_cameraDistance);
        Beryll::Camera::setCameraFrontPos(m_cameraFront);
    }

    void PlayStateSceneLayer::rotateCameraToPlayerMoveDir()
    {
        if(m_playerMoveSpeed < EnumsAndVariables::minPlayerSpeedToCameraFollow)
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

    void PlayStateSceneLayer::rotateCameraOnBuilding()
    {
        if(glm::length(m_screenSwipeDir) < 180.0f)
            return;

        const glm::vec3 screenSwipeBackXZ = -glm::normalize(glm::vec3(m_screenSwipeDir.x, 0.0f, m_screenSwipeDir.z));
        if(glm::any(glm::isnan(screenSwipeBackXZ)))
            return;
        //BR_INFO("playerMoveBackXZ X:%f Y:%f Z:%f", playerBackXZ.x, playerBackXZ.y, playerBackXZ.z);

        const glm::vec3 cameraBackXZ = Beryll::Camera::getCameraBackDirectionXZ();
        if(glm::any(glm::isnan(cameraBackXZ)))
            return;
        //BR_INFO("cameraBackXZ X:%f Y:%f Z:%f", cameraBackXZ.x, cameraBackXZ.y, cameraBackXZ.z);

        const glm::quat rotation = glm::rotation(cameraBackXZ, screenSwipeBackXZ);

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
