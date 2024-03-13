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

        std::vector<glm::vec3> walls = BeryllUtils::Common::loadMeshVerticesToVector("models3D/Walls.fbx");
        for(const auto& wall : walls)
        {
            m_pathFinder.addWall({(int)std::roundf(wall.x), (int)std::roundf(wall.z)});
        }

        BR_INFO("walls.size() %d", walls.size());

        std::vector<glm::vec3> allowedPoints = BeryllUtils::Common::loadMeshVerticesToVector("models3D/AllowedPositions.fbx");
        m_allowedPointsToMoveXZ.reserve(allowedPoints.size());
        for(const auto& point : allowedPoints)
        {
            m_allowedPointsToMoveXZ.push_back({(int)std::roundf(point.x), (int)std::roundf(point.z)});
        }

        m_allowedPointsToSpawnEnemies.reserve(m_allowedPointsToMoveXZ.size());

        BR_INFO("m_allowedPointsToMoveXZ.size() %d", m_allowedPointsToMoveXZ.size());

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
        controlPlayer();
        updateGarbageGravity();
        for(auto enemy : m_allAnimatedEnemies)
        {
            if(enemy->getIsEnabledUpdate())
            {
                enemy->update(m_player->getOrigin());
            }
        }

//        if(m_gui->sliderFPS->getIsValueChanging())
//            Beryll::GameLoop::setFPSLimit(m_gui->sliderFPS->getValue());

        EnumsAndVariables::maxActiveEnemiesCount = int(m_gui->sliderEnemy->getValue());

        if(m_gui->sliderGarbage->getIsValueChanging())
        {
            for(int i = 0; i < m_allGarbage.size(); ++i)
            {
                if(i < int(m_gui->sliderGarbage->getValue()))
                {
                    m_allGarbage[i]->enableDraw();
                    m_allGarbage[i]->enableUpdate();
                    m_allGarbage[i]->enableCollisionMesh();
                }
                else
                {
                    m_allGarbage[i]->disableDraw();
                    m_allGarbage[i]->disableUpdate();
                    m_allGarbage[i]->disableCollisionMesh();
                }
            }
        }
    }

    void PlayStateSceneLayer::updateAfterPhysics()
    {
        const float distanceToEnableObjects = m_cameraDistance * 0.6f;

        for(const std::shared_ptr<Beryll::SceneObject>& so : m_allSceneObjects)
        {
            if(so->getIsEnabledUpdate())
            {
                so->updateAfterPhysics();

                if(so->getOrigin().y < -500.0f)
                    so->disableForEver();

                if(so->getSceneObjectGroup() == Beryll::SceneObjectGroups::ENEMY ||
                   so->getSceneObjectGroup() == Beryll::SceneObjectGroups::DYNAMIC_ENVIRONMENT)
                {
                    if(glm::distance(m_player->getOrigin(), so->getOrigin()) < distanceToEnableObjects ||
                       Beryll::Camera::getIsSeeObject(so->getOrigin(), 0.5f))
                        so->enableDraw();
                    else
                        so->disableDraw();
                }
            }
        }

        //Beryll::AsyncRun::Run(m_allSceneObjects, m_updateAfterPhysics);

        updatePlayerGravity();
        updatePlayerSpeed();
        updatePathfindingAndSpawnEnemies();
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
                                                                                       Beryll::CollisionGroups::DYNAMIC_ENVIRONMENT | Beryll::CollisionGroups::RAY_FOR_BUILDING_CHECK,
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
        const auto objects1 = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/Garbage3.fbx",
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

        const auto objects3 = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/Garbage1.fbx",
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
        for(int x = 0; x < 500; ++x)
        {
            auto unit = std::make_shared<AnimatedCollidingEnemy>("models3D/AnimCollFootman1.fbx",
                                                                 0.0f,
                                                                 false,
                                                                 Beryll::CollisionFlags::STATIC,
                                                                 Beryll::CollisionGroups::NONE,
                                                                 Beryll::CollisionGroups::NONE,
                                                                 Beryll::SceneObjectGroups::ENEMY);

            unit->setCurrentAnimationByIndex(3, false, false);
            unit->setDefaultAnimationByIndex(3);
            unit->getController().moveSpeed = 20.0f;

            m_allSceneObjects.push_back(unit);
            m_allAnimatedEnemies.push_back(unit);
            m_animatedObjForShadowMap.push_back(unit);

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
                if(!obj->getIsEnabledUpdate())
                    continue;

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
            if(!obj->getIsEnabledUpdate())
                continue;

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
        if(Beryll::Physics::getIsCollisionWithGroup(m_player->getID(), Beryll::CollisionGroups::GROUND))
        {
            m_player->setGravity(EnumsAndVariables::playerGravityOnGround);
        }
        else if(Beryll::Physics::getIsCollisionWithGroup(m_player->getID(), Beryll::CollisionGroups::BUILDING))
        {
            m_lastTimeOnBuilding = Beryll::TimeStep::getSecFromStart();
            m_player->setGravity(EnumsAndVariables::playerGravityOnBuilding);
        }
        else if(m_lastTimeOnBuilding + m_applyGravityDelay < Beryll::TimeStep::getSecFromStart())
        {
            m_player->setGravity(EnumsAndVariables::playerGravityOnAir);
        }
    }

    void PlayStateSceneLayer::updatePlayerSpeed()
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

    void PlayStateSceneLayer::updatePathfindingAndSpawnEnemies()
    {
        // In first frame find closest point to player and all available to spawn enemies. Spawn enemies.
        if(m_pathFindingIteration == 0)
        {
            ++m_pathFindingIteration; // Go to next iteration in next frame.

            m_allowedPointsToSpawnEnemies.clear();

            glm::vec2 playerPosXZ{m_player->getOrigin().x, m_player->getOrigin().z};
            float distanceToClosestPoint = std::numeric_limits<float>::max();
            float distanceToCurrent = 0.0f;

            for(const glm::ivec2& point : m_allowedPointsToMoveXZ)
            {
                distanceToCurrent = glm::distance(playerPosXZ, glm::vec2(point.x, point.y));

                if(distanceToCurrent < distanceToClosestPoint)
                {
                    distanceToClosestPoint = distanceToCurrent;
                    m_playerClosestAllowedPos = point;
                }

                if(distanceToCurrent > 150.0f)
                {
                    // We can spawn enemy at this point.
                    m_allowedPointsToSpawnEnemies.push_back(point);
                }
            }

            BR_ASSERT((!m_allowedPointsToSpawnEnemies.empty()), "%s", "m_allowedPointsToSpawnEnemies empty.");

            // Spawn enemies.
            int countToSpawn = EnumsAndVariables::maxActiveEnemiesCount - AnimatedCollidingEnemy::getActiveCount();
            int spawnedCount = 0;
            if(countToSpawn > 0)
            {
                for(auto enemy : m_allAnimatedEnemies)
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

                        enemy->pathArray = m_pathFinder.findPath(spawnPoint2D, m_playerClosestAllowedPos, 7);
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
        // In second frame calculate path.
        else if(m_pathFindingIteration == 1)
        {
            m_pathFindingIteration = 0; // Start from start again in next frame.

            int enemiesUpdated = 0;
            int& i = EnumsAndVariables::currentPathfindingEnemyIndex;
            for( ; i < m_allAnimatedEnemies.size(); ++i)
            {
                if(enemiesUpdated >= EnumsAndVariables::maxPathfindingInOneFrame)
                    break;

                if(m_allAnimatedEnemies[i]->getIsEnabledUpdate())
                {
                    // First canst ray to check if enemy see player.
                    Beryll::RayClosestHit rayCollisionBithBuilding = Beryll::Physics::castRayClosestHit(m_allAnimatedEnemies[i]->getOrigin(),
                                                                                                        m_player->getOrigin(),
                                                                                                        Beryll::CollisionGroups::RAY_FOR_BUILDING_CHECK,
                                                                                                        Beryll::CollisionGroups::BUILDING);

                    if(rayCollisionBithBuilding) // Enemy dont see player now. Move according A Star path.
                    {
                        glm::ivec2 currentPointToMove = m_allAnimatedEnemies[i]->currentPointToMove2DIntegers;

                        if(m_allAnimatedEnemies[i]->seePlayer) // Saw player previous frame but stop see from now.
                        {
                            //BR_INFO("%s", "Saw player previous frame but stop see from now.");
                            // Find closest point to enemy. And find path from that point.
                            glm::vec2 enemyPosXZ{m_allAnimatedEnemies[i]->getOrigin().x, m_allAnimatedEnemies[i]->getOrigin().z};
                            float distanceToClosestPoint = std::numeric_limits<float>::max();
                            float distanceToCurrent = 0.0f;

                            for(const glm::ivec2& point : m_allowedPointsToMoveXZ)
                            {
                                distanceToCurrent = glm::distance(enemyPosXZ, glm::vec2(point.x, point.y));

                                if(distanceToCurrent < distanceToClosestPoint)
                                {
                                    distanceToClosestPoint = distanceToCurrent;
                                    currentPointToMove = point;
                                }
                            }
                        }

                        m_allAnimatedEnemies[i]->seePlayer = false;

                        m_allAnimatedEnemies[i]->pathArray = m_pathFinder.findPath(currentPointToMove, m_playerClosestAllowedPos, 7);
                        m_allAnimatedEnemies[i]->indexInPathArray = 0;
                        m_allAnimatedEnemies[i]->currentPointToMove2DIntegers = m_allAnimatedEnemies[i]->pathArray[m_allAnimatedEnemies[i]->indexInPathArray];
                        m_allAnimatedEnemies[i]->currentPointToMove3DFloats = glm::vec3(m_allAnimatedEnemies[i]->currentPointToMove2DIntegers.x,
                                                                                        m_allAnimatedEnemies[i]->getController().getFromOriginToBottom(),
                                                                                        m_allAnimatedEnemies[i]->currentPointToMove2DIntegers.y);
                    }
                    else // Enemy see player now. Move directly to closest allowed point to player.
                    {
                        m_allAnimatedEnemies[i]->seePlayer = true;

                        m_allAnimatedEnemies[i]->currentPointToMove3DFloats = glm::vec3(m_playerClosestAllowedPos.x,
                                                                                        m_allAnimatedEnemies[i]->getController().getFromOriginToBottom(),
                                                                                        m_playerClosestAllowedPos.y);
                    }

                    ++enemiesUpdated;
                }
            }

            if(EnumsAndVariables::currentPathfindingEnemyIndex >= m_allAnimatedEnemies.size() -1)
                EnumsAndVariables::currentPathfindingEnemyIndex = 0;

        }
    }

    void PlayStateSceneLayer::killEnemies()
    {
        for(auto enemy : m_allAnimatedEnemies)
        {
            if(enemy->getIsEnabledUpdate() && glm::distance(enemy->getOrigin(), m_player->getOrigin()) < 10.0f)
            {
                enemy->disableEnemy();
                //BR_INFO("Kill enemy. active count: %d", AnimatedCollidingEnemy::getActiveCount());
            }
        }
    }

    void PlayStateSceneLayer::handleCamera()
    {
        if(Beryll::Physics::getIsCollisionWithGroup(m_player->getID(), Beryll::CollisionGroups::GROUND))
            rotateCameraToPlayerMoveDir();
        else
            rotateCameraOnBuilding();

        m_cameraFront = m_player->getOrigin();
        m_cameraFront.y += 15.0f;

        float maxCameraDistance = m_startCameraDistance + m_objectsInMagneticRadius * 0.2f;
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
