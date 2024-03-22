#include "PlayStateSceneLayer.h"
#include "EnumsAndVariables.h"
#include "enemies/CopWithPistol.h"
#include "enemies/CopWithGrenadeLauncher.h"

namespace MagneticBall3D
{
    PlayStateSceneLayer::PlayStateSceneLayer(std::shared_ptr<PlayStateGUILayer> gui)
    {
        m_ID = Beryll::LayerID::PLAY_SCENE;
        
        m_gui = std::move(gui);

        Beryll::Physics::hardRemoveAllObjects();

        loadPlayerAndStaticEnv();
        loadGarbage();
        loadEnemies();
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

        std::vector<glm::vec3> walls = BeryllUtils::Common::loadMeshVerticesToVector("models3D/Walls.fbx");
        for(const auto& wall : walls)
        {
            m_pathFinder.addWallPosition({(int)std::roundf(wall.x), (int)std::roundf(wall.z)});
        }

        BR_INFO("walls.size() %d", walls.size());

        std::vector<glm::vec3> allowedPoints = BeryllUtils::Common::loadMeshVerticesToVector("models3D/AllowedPositions.fbx");
        m_pathGridXZ.reserve(allowedPoints.size());
        for(const auto& point : allowedPoints)
        {
            m_pathGridXZ.push_back({(int)std::roundf(point.x), (int)std::roundf(point.z)});
        }

        m_allowedPointsToSpawnEnemies.reserve(m_pathGridXZ.size());

        BR_INFO("m_pathGridXZ.size() %d", m_pathGridXZ.size());

//        std::vector<glm::ivec2> path = m_pathFinder.findPath({0, 0}, {60, 0});
//
//        for(const auto& point : path)
//        {
//            BR_INFO("point in path 1 %d %d", point.x, point.y);
//        }

        //BR_INFO(" X:%f Y:%f Z:%f", .x, .y, .z);
        //BR_INFO("%s", "");
        //BR_INFO(" %f", );
        //BR_INFO(" %d", );
    }

    PlayStateSceneLayer::~PlayStateSceneLayer()
    {

    }

    void PlayStateSceneLayer::updateBeforePhysics()
    {
        handleScreenSwipe();
        magnetizeGarbageAndUpdateGravity();
        updatePathfindingAndSpawnEnemies();
        for(const auto& enemy : m_allAnimatedEnemies)
        {
            if(enemy->getIsEnabledUpdate())
            {
                enemy->update(m_player->getOrigin());
            }
        }

        handleEnemiesAttacks();

        for(auto& garbage : m_allGarbageWrappers)
        {
            if(garbage.getIsEnabled())
            {
                garbage.update();
            }
        }

//        if(m_gui->sliderFPS->getIsValueChanging())
//            Beryll::GameLoop::setFPSLimit(m_gui->sliderFPS->getValue());

        EnumsAndVariables::maxActiveEnemiesCount = int(m_gui->sliderEnemy->getValue());
        EnumsAndVariables::playerMaxSpeedXZ = m_gui->sliderSpeed->getValue();
        EnumsAndVariables::playerImpulseFactor = m_gui->sliderImpulse->getValue();
        EnumsAndVariables::playerTorqueFactor = m_gui->sliderTorque->getValue();

        if(m_gui->buttonA->getIsPressed())
        {
            const float powerToOneKg = 100.0f;

            m_player->applyCentralImpulse(glm::vec3(0.0f, powerToOneKg * m_player->getCollisionMass(), 0.0f));

            for(const auto& wrapper : m_allGarbageWrappers)
            {
                if(wrapper.isMagnetized)
                    wrapper.obj->applyCentralImpulse(glm::vec3(0.0f, powerToOneKg * wrapper.obj->getCollisionMass(), 0.0f));
            }
        }
    }

    void PlayStateSceneLayer::updateAfterPhysics()
    {
        const float distanceToEnableObjects = m_cameraDistance * 0.65f;

        for(const std::shared_ptr<Beryll::SceneObject>& so : m_allSceneObjects)
        {
            if(so->getIsEnabledUpdate())
            {
                so->updateAfterPhysics();

                if(so->getOrigin().y < -500.0f)
                    so->disableForEver();

                if(so->getSceneObjectGroup() == Beryll::SceneObjectGroups::ENEMY ||
                   so->getSceneObjectGroup() == Beryll::SceneObjectGroups::GARBAGE)
                {
                    if(glm::distance(m_player->getOrigin(), so->getOrigin()) < distanceToEnableObjects ||
                       Beryll::Camera::getIsSeeObject(so->getOrigin(), 0.8f))
                        so->enableDraw();
                    else
                        so->disableDraw();
                }
            }
        }

        m_player->updateSpeed();
        m_player->updateGravity();

        //Beryll::AsyncRun::Run(m_allSceneObjects, m_updateAfterPhysics);
        killEnemies();
        handleCamera(); // Last call before draw.
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

        for(const auto& wrapper : m_allGarbageWrappers)
        {
            if(wrapper.obj->getIsEnabledDraw())
            {
                modelMatrix = wrapper.obj->getModelMatrix();
                m_simpleObjSunLightShadows->setMatrix4x4Float("MVPLightMatrix", m_sunLightVPMatrix * modelMatrix);
                m_simpleObjSunLightShadows->setMatrix4x4Float("modelMatrix", modelMatrix);
                m_simpleObjSunLightShadows->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
                Beryll::Renderer::drawObject(wrapper.obj, modelMatrix, m_simpleObjSunLightShadows);
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

        for(const auto& animObj : m_allAnimatedEnemies)
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

        //m_skyBox->draw();
        Beryll::ParticleSystem::draw();
    }

    void PlayStateSceneLayer::loadPlayerAndStaticEnv()
    {
        m_player = std::make_shared<Player>("models3D/player/Player.fbx",
                                            EnumsAndVariables::playerMass,
                                            true,
                                            Beryll::CollisionFlags::DYNAMIC,
                                            Beryll::CollisionGroups::PLAYER,
                                            Beryll::CollisionGroups::GROUND | Beryll::CollisionGroups::BUILDING |
                                            Beryll::CollisionGroups::GARBAGE | Beryll::CollisionGroups::ENEMY_ATTACK,
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
                                                                   Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE,
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
                                                                                       Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE |
                                                                                       Beryll::CollisionGroups::RAY_FOR_BUILDING_CHECK  | Beryll::CollisionGroups::ENEMY_ATTACK,
                                                                                       Beryll::SceneObjectGroups::BUILDING);

        for(const auto& obj : objects1)
        {
            m_allSceneObjects.push_back(obj);
            m_allStaticEnv.push_back(obj);
            m_simpleObjForShadowMap.push_back(obj);
            obj->setFriction(EnumsAndVariables::staticEnvFriction);
        }
    }

    void PlayStateSceneLayer::loadGarbage()
    {
        const auto objects1 = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/Garbage3.fbx",
                                                                                       EnumsAndVariables::garbageMass,
                                                                                       false,
                                                                                       Beryll::CollisionFlags::DYNAMIC,
                                                                                       Beryll::CollisionGroups::GARBAGE,
                                                                                       Beryll::CollisionGroups::GROUND | Beryll::CollisionGroups::BUILDING |
                                                                                       Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE |
                                                                                       Beryll::CollisionGroups::ENEMY_ATTACK,
                                                                                       Beryll::SceneObjectGroups::GARBAGE);

        for(const auto& obj : objects1)
        {
            m_allGarbageWrappers.emplace_back(obj, GarbageType::DEFAULT, 20);

            m_allSceneObjects.push_back(obj);
            m_simpleObjForShadowMap.push_back(obj);

            obj->setDamping(EnumsAndVariables::garbageDamping, EnumsAndVariables::garbageDamping);
            obj->setGravity(EnumsAndVariables::garbageGravityDefault, false, false);
        }

        const auto objects2 = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/Garbage2.fbx",
                                                                                       EnumsAndVariables::garbageMass,
                                                                                       false,
                                                                                       Beryll::CollisionFlags::DYNAMIC,
                                                                                       Beryll::CollisionGroups::GARBAGE,
                                                                                       Beryll::CollisionGroups::GROUND | Beryll::CollisionGroups::BUILDING |
                                                                                       Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE |
                                                                                       Beryll::CollisionGroups::ENEMY_ATTACK,
                                                                                       Beryll::SceneObjectGroups::GARBAGE);

        for(const auto& obj : objects2)
        {
            m_allGarbageWrappers.emplace_back(obj, GarbageType::DEFAULT, 20);

            m_allSceneObjects.push_back(obj);
            m_simpleObjForShadowMap.push_back(obj);

            obj->setDamping(EnumsAndVariables::garbageDamping, EnumsAndVariables::garbageDamping);
            obj->setGravity(EnumsAndVariables::garbageGravityDefault, false, false);
        }

        const auto objects3 = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/Garbage1.fbx",
                                                                                       EnumsAndVariables::garbageMass,
                                                                                       false,
                                                                                       Beryll::CollisionFlags::DYNAMIC,
                                                                                       Beryll::CollisionGroups::GARBAGE,
                                                                                       Beryll::CollisionGroups::GROUND | Beryll::CollisionGroups::BUILDING |
                                                                                       Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE |
                                                                                       Beryll::CollisionGroups::ENEMY_ATTACK,
                                                                                       Beryll::SceneObjectGroups::GARBAGE);

        for(const auto& obj : objects3)
        {
            m_allGarbageWrappers.emplace_back(obj, GarbageType::DEFAULT, 20);

            m_allSceneObjects.push_back(obj);
            m_simpleObjForShadowMap.push_back(obj);

            obj->setDamping(EnumsAndVariables::garbageDamping, EnumsAndVariables::garbageDamping);
            obj->setGravity(EnumsAndVariables::garbageGravityDefault, false, false);
        }
    }

    void PlayStateSceneLayer::loadEnemies()
    {
        for(int x = 0; x < 250; ++x)
        {
            auto unit = std::make_shared<CopWithPistol>("models3D/AnimCollFootman1.fbx",
                                                    0.0f,
                                                    false,
                                                    Beryll::CollisionFlags::STATIC,
                                                    Beryll::CollisionGroups::NONE,
                                                    Beryll::CollisionGroups::NONE,
                                                    Beryll::SceneObjectGroups::ENEMY);

            unit->setCurrentAnimationByIndex(EnumsAndVariables::AnimationIndexes::RUN, false, false);
            unit->setDefaultAnimationByIndex(EnumsAndVariables::AnimationIndexes::IDLE);
            unit->getController().moveSpeed = 20.0f;

            m_allSceneObjects.push_back(unit);
            m_allAnimatedEnemies.push_back(unit);
            m_animatedObjForShadowMap.push_back(unit);

            auto unit2 = std::make_shared<CopWithGrenadeLauncher>("models3D/StoneThrower.fbx",
                                                        0.0f,
                                                        false,
                                                        Beryll::CollisionFlags::STATIC,
                                                        Beryll::CollisionGroups::NONE,
                                                        Beryll::CollisionGroups::NONE,
                                                        Beryll::SceneObjectGroups::ENEMY);

            unit2->setCurrentAnimationByIndex(EnumsAndVariables::AnimationIndexes::RUN, false, false);
            unit2->setDefaultAnimationByIndex(EnumsAndVariables::AnimationIndexes::IDLE);
            unit2->getController().moveSpeed = 20.0f;

            m_allSceneObjects.push_back(unit2);
            m_allAnimatedEnemies.push_back(unit2);
            m_animatedObjForShadowMap.push_back(unit2);
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

    void PlayStateSceneLayer::handleScreenSwipe()
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
            m_screenSwipe3D = glm::vec3{-screenSwipe.y, 0.0f, screenSwipe.x};
            float screenSwipeLength = glm::length(m_screenSwipe3D);
            if(screenSwipeLength > 1000.0f)
            {
                screenSwipeLength = 1000.0f;
                m_screenSwipe3D = glm::normalize(m_screenSwipe3D) * screenSwipeLength;
            }

            glm::qua cameraRotFromStartDir = glm::normalize(glm::rotation(m_startDir, Beryll::Camera::getCameraFrontDirectionXZ()));
            m_screenSwipe3D = glm::normalize(cameraRotFromStartDir * glm::normalize(m_screenSwipe3D));
            m_screenSwipe3D *= screenSwipeLength;

            float factorImpulseApplied = m_player->handleScreenSwipe(m_screenSwipe3D * EnumsAndVariables::playerImpulseFactor,
                                                                     m_screenSwipe3D * EnumsAndVariables::playerTorqueFactor);

            // Apply impulse to garbage too.
            glm::vec3 impulseForGarbage = (m_screenSwipe3D * EnumsAndVariables::playerImpulseFactor) * factorImpulseApplied * EnumsAndVariables::playerMassToGarbageMassRatio;
            // We apply only impulse for garbage. But for player were applied impulse + torque.
            // This compensate player speed increased by torque.
            impulseForGarbage *= 1.25f;
            for(const auto& wrapper : m_allGarbageWrappers)
            {
                if(wrapper.isMagnetized)
                    wrapper.obj->applyCentralImpulse(impulseForGarbage);
            }
        }
    }

    void PlayStateSceneLayer::magnetizeGarbageAndUpdateGravity()
    {
        EnumsAndVariables::garbageCountMagnetized = 0;

        // Check if magnetized garbage still in playerMagneticRadius.
        for(auto& wrapper : m_allGarbageWrappers)
        {
            if(wrapper.getIsEnabled() && wrapper.isMagnetized &&
               glm::distance(m_player->getOrigin(), wrapper.obj->getOrigin()) < EnumsAndVariables::playerMagneticRadius)
            {
                ++EnumsAndVariables::garbageCountMagnetized;
            }
            else
            {
                wrapper.isMagnetized = false;
            }
        }

        // Check for more garbage if we have limit to that.
        if(EnumsAndVariables::garbageMaxCountMagnetized - EnumsAndVariables::garbageCountMagnetized > 0)
        {
            for(auto& wrapper : m_allGarbageWrappers)
            {
                if(wrapper.getIsEnabled() && !wrapper.isMagnetized &&
                   glm::distance(m_player->getOrigin(), wrapper.obj->getOrigin()) < EnumsAndVariables::playerMagneticRadius)
                {
                    ++EnumsAndVariables::garbageCountMagnetized;
                    wrapper.isMagnetized = true;

                    if(EnumsAndVariables::garbageMaxCountMagnetized == EnumsAndVariables::garbageCountMagnetized)
                        break;
                }
            }
        }

        for(auto& wrapper : m_allGarbageWrappers)
        {
            if(wrapper.isMagnetized)
            {
                glm::vec3 gravDir = glm::normalize(m_player->getOrigin() - wrapper.obj->getOrigin());
                float gravPower = EnumsAndVariables::garbageMinGravityPower + (m_player->getMoveSpeed() * EnumsAndVariables::garbageGravityIncreasedByPlayerSpeed);
                if(Beryll::Physics::getIsCollision(m_player->getID(), wrapper.obj->getID()))
                {
                    wrapper.obj->setGravity(-(gravDir * gravPower * 0.1f), false, false);
                }
                else
                {
                    wrapper.obj->setGravity(gravDir * gravPower, false, true);
                }

                // Stop garbage if it stats rotating around player too fast.
                const glm::vec3 linVelocity = wrapper.obj->getLinearVelocity();
                const glm::vec3 objMoveDir = glm::normalize(linVelocity);
                const float objSpeed = glm::length(linVelocity);
                const glm::vec3 objToPlayerDir = glm::normalize(m_player->getOrigin() - wrapper.obj->getOrigin());

                if(objSpeed > m_player->getMoveSpeed() * 2.0f && BeryllUtils::Common::getAngleInRadians(objToPlayerDir, objMoveDir) > 0.35f) // > 20 degrees.
                {
                    wrapper.obj->setLinearVelocity(objToPlayerDir * 5.0f);
                }
            }
            else if(wrapper.getIsEnabled())
            {
                wrapper.obj->setGravity(EnumsAndVariables::garbageGravityDefault, false, false);
            }
        }
    }

    void PlayStateSceneLayer::updatePathfindingAndSpawnEnemies()
    {
        // In first frame:
        // 1. Find closest point to player.
        // 2. Find allowed points to spawn enemies.
        if(m_pathFindingIteration == 0)
        {
            ++m_pathFindingIteration; // Go to next iteration in next frame.

            m_allowedPointsToSpawnEnemies.clear();

            glm::vec2 playerPosXZ{m_player->getOrigin().x, m_player->getOrigin().z};
            float distanceToClosestPoint = std::numeric_limits<float>::max();
            float distanceToCurrent = 0.0f;

            for(const glm::ivec2& point : m_pathGridXZ)
            {
                distanceToCurrent = glm::distance(playerPosXZ, glm::vec2(point));

                // 1.
                if(distanceToCurrent < distanceToClosestPoint)
                {
                    distanceToClosestPoint = distanceToCurrent;
                    m_playerClosestAllowedPos = point;
                }

                // 2.
                if(distanceToCurrent > EnumsAndVariables::minDistanceToSpawnEnemies)
                {
                    // We can spawn enemy at this point.
                    m_allowedPointsToSpawnEnemies.push_back(point);
                }
            }

            BR_ASSERT((!m_allowedPointsToSpawnEnemies.empty()), "%s", "m_allowedPointsToSpawnEnemies empty.");
        }
        // In second frame:
        // 1. Clear blocked positions.
        // 2. Spawn enemies.
        else if(m_pathFindingIteration == 1)
        {
            ++m_pathFindingIteration;

            // 1.
            m_pathFinder.clearBlockedPositions();

            // 2.
            int countToSpawn = EnumsAndVariables::maxActiveEnemiesCount - BaseEnemy::getActiveCount();
            int spawnedCount = 0;
            if(countToSpawn > 0)
            {
                for(const auto& enemy : m_allAnimatedEnemies)
                {
                    if(!enemy->getIsEnabledUpdate())
                    {
                        enemy->enableEnemy();
                        enemy->disableDraw();

                        glm::ivec2 spawnPoint2D = m_allowedPointsToSpawnEnemies[Beryll::RandomGenerator::getInt(m_allowedPointsToSpawnEnemies.size() - 1)];
                        glm::vec3 spawnPoint3D{spawnPoint2D.x,
                                               enemy->getController().getFromOriginToBottom(),
                                               spawnPoint2D.y};
                        enemy->setOrigin(spawnPoint3D);

                        enemy->pathArray = m_pathFinder.findPath(spawnPoint2D, m_playerClosestAllowedPos, 6);
                        if(enemy->pathArray.size() > 1)
                            enemy->indexInPathArray = 1;
                        else
                            enemy->indexInPathArray = 0;

                        enemy->currentPointToMove2DIntegers = enemy->pathArray[enemy->indexInPathArray];
                        enemy->currentPointToMove3DFloats = glm::vec3(enemy->currentPointToMove2DIntegers.x,
                                                                      enemy->getController().getFromOriginToBottom(),
                                                                      enemy->currentPointToMove2DIntegers.y);

                        ++spawnedCount;

                        //BR_INFO("Spawn enemy %d %d active count: %d", spawnPoint2D.x, spawnPoint2D.y, AnimatedCollidingEnemy::getActiveCount());

                        if(spawnedCount >= countToSpawn)
                            break;
                    }
                }
            }
        }
        // In third frame:
        // 1. Update paths for enemies.
        else if(m_pathFindingIteration == 2)
        {
            int enemiesUpdated = 0;
            int& i = EnumsAndVariables::currentPathfindingEnemyIndex;
            for( ; i < m_allAnimatedEnemies.size(); ++i)
            {
                if(enemiesUpdated >= EnumsAndVariables::maxPathfindingInOneFrame)
                    break;

                if(m_allAnimatedEnemies[i]->getIsEnabledUpdate())
                {
                    m_allAnimatedEnemies[i]->pathArray = m_pathFinder.findPath(m_allAnimatedEnemies[i]->currentPointToMove2DIntegers, m_playerClosestAllowedPos, 8);
                    m_allAnimatedEnemies[i]->indexInPathArray = 0;
                    m_allAnimatedEnemies[i]->currentPointToMove2DIntegers = m_allAnimatedEnemies[i]->pathArray[m_allAnimatedEnemies[i]->indexInPathArray];
                    m_allAnimatedEnemies[i]->currentPointToMove3DFloats = glm::vec3(m_allAnimatedEnemies[i]->currentPointToMove2DIntegers.x,
                                                                                    m_allAnimatedEnemies[i]->getController().getFromOriginToBottom(),
                                                                                    m_allAnimatedEnemies[i]->currentPointToMove2DIntegers.y);

                    m_pathFinder.addBlockedPosition(m_allAnimatedEnemies[i]->currentPointToMove2DIntegers);

                    ++enemiesUpdated;
                }
            }

            if(EnumsAndVariables::currentPathfindingEnemyIndex >= m_allAnimatedEnemies.size() -1)
            {
                // All enemies were updated.
                EnumsAndVariables::currentPathfindingEnemyIndex = 0;
                m_pathFindingIteration = 0; // Start from start again in next frame.
            }

        }
    }

    void PlayStateSceneLayer::handleEnemiesAttacks()
    {
        for(const auto& enemy : m_allAnimatedEnemies)
        {
            if(enemy->getIsEnabledUpdate() && enemy->unitState == UnitState::CAN_ATTACK)
            {
                glm::vec3 from = enemy->getOrigin();
                from.y += enemy->getController().getFromOriginToTop() * 0.5f;
                glm::vec3 target = m_player->getOrigin();
                target.y += 1.5f;
                Beryll::RayClosestHit rayAttack = Beryll::Physics::castRayClosestHit(from,
                                                                                     target,
                                                                                     Beryll::CollisionGroups::ENEMY_ATTACK,
                                                                                     Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE);

                if(rayAttack)
                {
                    // Spam particles.
                    if(enemy->getAttackType() == AttackType::RANGE_DAMAGE_ONE)
                    {
                        emitParticlesLine(from, rayAttack.hitPoint, 0.2f, 0.2f,
                                          glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), 1);
                    }
                    else if(enemy->getAttackType() == AttackType::RANGE_DAMAGE_RADIUS)
                    {
                        emitParticlesLine(from, rayAttack.hitPoint, 0.2f, 0.2f,
                                          glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), 1);

                        emitParticlesExplosion(rayAttack.hitPoint, 10, 0.5f, 0.5f,
                                               glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), 1);
                    }

                    enemy->attack(m_player->getOrigin());

                    if(rayAttack.hittedCollGroup == Beryll::CollisionGroups::PLAYER)
                    {
                        // Player attacked.
                        //BR_INFO("%s", "Player attacked");

                    }
                    else if(rayAttack.hittedCollGroup == Beryll::CollisionGroups::GARBAGE)
                    {
                        // Garbage under attack =).
                        if(enemy->getAttackType() == AttackType::RANGE_DAMAGE_ONE)
                        {
                            //BR_INFO("%s", "Garbage under attack =) by AttackType::RANGE_DAMAGE_ONE");
                            for(auto& wrapper : m_allGarbageWrappers)
                            {
                                if(wrapper.obj->getIsEnabledUpdate() && rayAttack.hittedObjectID == wrapper.obj->getID())
                                {
                                    wrapper.currentHP -= enemy->getDamage();
                                    break;
                                }
                            }
                        }
                        else if(enemy->getAttackType() == AttackType::RANGE_DAMAGE_RADIUS)
                        {
                            //BR_INFO("%s", "Garbage under attack =) by AttackType::RANGE_DAMAGE_RADIUS");
                            for(auto& wrapper : m_allGarbageWrappers)
                            {
                                if(wrapper.obj->getIsEnabledUpdate() && rayAttack.hittedObjectID == wrapper.obj->getID() &&
                                   glm::distance(rayAttack.hitPoint, wrapper.obj->getOrigin()) < enemy->getDamageRadius())
                                {
                                    wrapper.currentHP -= enemy->getDamage();
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    void PlayStateSceneLayer::emitParticlesLine(const glm::vec3& from, const glm::vec3& to, const float sizeBegin, const float sizeEnd,
                                                const glm::vec4& colorBegin, const glm::vec4& colorEnd, const float lifeTime)
    {
        glm::vec3 dir = glm::normalize(to - from);
        float distance = glm::distance(to, from);
        const float spamOffsetMeters = 1.0f;
        glm::vec3 spamPoint{0.0f};
        for(float i = 0.0f; i < distance; i += spamOffsetMeters)
        {
            spamPoint = from + (dir * i);

            Beryll::ParticleSystem::EmitCubesFromCenter(1, lifeTime, sizeBegin, sizeEnd,
                                                        colorBegin, colorEnd,
                                                        spamPoint, glm::vec3(0.0f), 0);
        }
    }

    void PlayStateSceneLayer::emitParticlesExplosion(const glm::vec3& orig, const int count, const float sizeBegin, const float sizeEnd,
                                                     const glm::vec4& colorBegin, const glm::vec4& colorEnd, const float lifeTime)
    {
        Beryll::ParticleSystem::EmitCubesFromCenter(count, lifeTime, sizeBegin, sizeEnd,
                                                    colorBegin, colorEnd,
                                                    orig, glm::vec3(0.0f), 10);
    }

    void PlayStateSceneLayer::killEnemies()
    {
        float speedToReduce = 0.0f;
        for(const auto& enemy : m_allAnimatedEnemies)
        {
            if(enemy->getIsEnabledUpdate() && glm::distance(enemy->getOrigin(), m_player->getOrigin()) < EnumsAndVariables::radiusToKillEnemies)
            {
                enemy->disableEnemy();
                speedToReduce += enemy->getPlayerSpeedReduceWhenDie();

                // Spam one garbage.
                for(auto& wrapper : m_allGarbageWrappers)
                {
                    if(!wrapper.getIsEnabled())
                    {
                        wrapper.enableGarbage();
                        wrapper.obj->setOrigin(enemy->getOrigin());
                        break;
                    }
                }

                if(enemy->getUnitType() == UnitType::COP_WITH_PISTOL)
                {
                    // Spawn garbage COP_WITH_PISTOL + PISTOL.
                }
                else if(enemy->getUnitType() == UnitType::COP_WITH_GRENADE_LAUNCHER)
                {
                    // Spawn garbage COP_WITH_GRENADE_LAUNCHER + GRENADE_LAUNCHER.
                }

                //BR_INFO("Kill enemy. active count: %d", AnimatedCollidingEnemy::getActiveCount());
            }
        }

        if(speedToReduce > 0.0f)
        {
            m_player->reduceSpeed(speedToReduce);
        }
    }

    void PlayStateSceneLayer::handleCamera()
    {
        const glm::vec3 cameraBackXZ = Beryll::Camera::getCameraBackDirectionXZ();
        glm::vec3 desiredCameraBackXZ = Beryll::Camera::getCameraBackDirectionXZ();

        if(Beryll::Physics::getIsCollisionWithGroup(m_player->getID(), Beryll::CollisionGroups::GROUND))
        {
            if(m_player->getMoveSpeed() > EnumsAndVariables::minPlayerSpeedToCameraFollow)
                desiredCameraBackXZ = -glm::normalize(glm::vec3(m_player->getMoveDir().x, 0.0f, m_player->getMoveDir().z));

            m_screenSwipe3D = glm::vec3{0.0f};
        }
        else
        {
            if(glm::length(m_screenSwipe3D) > 200.0f)
                desiredCameraBackXZ = -glm::normalize(glm::vec3(m_screenSwipe3D.x, 0.0f, m_screenSwipe3D.z));
        }

        if(!glm::any(glm::isnan(cameraBackXZ)) && !glm::any(glm::isnan(desiredCameraBackXZ)))
        {
            const glm::quat rotation = glm::rotation(cameraBackXZ, desiredCameraBackXZ);

            const float angleDifference = glm::angle(rotation);
            if(angleDifference > 0.035f) // More than 2 degrees.
            {
                const glm::mat4 cameraRotateMatr = glm::rotate(glm::mat4{1.0f},
                                                               angleDifference * 0.03f + 0.007f,
                                                               glm::normalize(glm::axis(rotation)));
                m_cameraOffset = cameraRotateMatr * glm::vec4(cameraBackXZ, 1.0f);
            }
        }

        m_cameraOffset.y = 0.0f;
        m_cameraOffset = glm::normalize(m_cameraOffset);
        m_cameraOffset.y = 0.15f + EnumsAndVariables::cameraYAccordingPlayerY * m_player->getOrigin().y;
        m_cameraOffset = glm::normalize(m_cameraOffset);

        m_cameraFront = m_player->getOrigin();
        m_cameraFront.y += 15.0f;

        float maxCameraDistance = m_startCameraDistance + EnumsAndVariables::garbageCountMagnetized * 0.15f;
        glm::vec3 cameraPosForRay = m_cameraFront + m_cameraOffset * (maxCameraDistance + 2.0f); // + 2m behind camera.

        // Check camera ray collisions.
        Beryll::RayClosestHit rayClosestHit = Beryll::Physics::castRayClosestHit(m_cameraFront,
                                                                                 cameraPosForRay,
                                                                                 Beryll::CollisionGroups::RAY_FOR_BUILDING_CHECK,
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
}
