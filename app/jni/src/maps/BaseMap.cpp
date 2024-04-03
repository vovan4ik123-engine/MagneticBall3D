#include "BaseMap.h"
#include "EnumsAndVariables.h"

namespace MagneticBall3D
{
    BaseMap::BaseMap(std::shared_ptr<PlayStateGUILayer> gui)
    {
        m_gui = std::move(gui);
    }

    BaseMap::~BaseMap()
    {

    }

    void BaseMap::updateBeforePhysics()
    {
        handleScreenSwipe();
        magnetizeGarbageAndUpdateGravity();
        updatePathfindingAndSpawnEnemies();
        for(const auto& enemy : m_allAnimatedEnemies)
        {
            if(enemy->getIsEnabledUpdate())
            {
                enemy->update(m_player->getObj()->getOrigin());
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

        if(EnumsAndVariables::lastTimeSpawnGarbage + EnumsAndVariables::spawnGarbageDelay < Beryll::TimeStep::getSecFromStart())
        {
            spawnGarbage(3, GarbageType::DEFAULT);
            EnumsAndVariables::lastTimeSpawnGarbage = Beryll::TimeStep::getSecFromStart();
        }

        EnumsAndVariables::maxActiveEnemiesCount = int(m_gui->sliderEnemy->getValue());
        EnumsAndVariables::playerMaxSpeedXZ = m_gui->sliderSpeed->getValue();

        if(m_gui->buttonA->getIsPressed())
        {
            const float powerToOneKg = 100.0f;

            m_player->getObj()->applyCentralImpulse(glm::vec3(0.0f, powerToOneKg * m_player->getObj()->getCollisionMass(), 0.0f));

            for(const auto& wrapper : m_allGarbageWrappers)
            {
                if(wrapper.isMagnetized)
                    wrapper.obj->applyCentralImpulse(glm::vec3(0.0f, powerToOneKg * wrapper.obj->getCollisionMass(), 0.0f));
            }
        }
    }

    void BaseMap::updateAfterPhysics()
    {
        const float distanceToEnableObjects = m_cameraDistance * 0.65f;

        for(const std::shared_ptr<Beryll::SceneObject>& so : m_allSceneObjects)
        {
            if(so->getIsEnabledUpdate())
            {
                so->updateAfterPhysics();

                if(so->getSceneObjectGroup() == Beryll::SceneObjectGroups::ENEMY ||
                   so->getSceneObjectGroup() == Beryll::SceneObjectGroups::GARBAGE)
                {
                    if(glm::distance(m_player->getObj()->getOrigin(), so->getOrigin()) < distanceToEnableObjects ||
                       Beryll::Camera::getIsSeeObject(so->getOrigin(), 1.1f))
                        so->enableDraw();
                    else
                        so->disableDraw();
                }
                else if(so->getSceneObjectGroup() == Beryll::SceneObjectGroups::BUILDING)
                {
//                    if(glm::distance(m_player->getObj()->getOrigin(), so->getOrigin()) < 100.0f ||
//                       Beryll::Camera::getIsSeeObject(so->getOrigin(), 1.0f, 1000.0f))
//                        so->enableDraw();
//                    else
//                        so->disableDraw();
                }
            }
        }

        m_player->updateSpeed();
        m_player->updateGravity();

        const glm::vec3& playerOrig = m_player->getObj()->getOrigin();
        bool playerOutOfMap = false;
        if(playerOrig.x < m_minX)
            playerOutOfMap = true;
        else if(playerOrig.x > m_maxX)
            playerOutOfMap = true;
        else if(playerOrig.z < m_minZ)
            playerOutOfMap = true;
        else if(playerOrig.z > m_maxZ)
            playerOutOfMap = true;

        if(playerOutOfMap)
        {
            m_player->getObj()->resetVelocities();
            m_player->getObj()->setLinearVelocity(glm::normalize(-playerOrig) * 35.0f);
        }

        killEnemies();
        handleCamera(); // Last call before draw.
    }

    void BaseMap::draw()
    {
        //BR_INFO("%s", "scene draw call");
        // 1. Draw into shadow map.
        glm::vec3 sunPos = m_player->getObj()->getOrigin() + (Beryll::Camera::getCameraFrontDirectionXZ() * 230.0f) + (m_dirToSun * m_sunDistance);
        updateSunPosition(sunPos, 700, 700, m_sunDistance * 2.0f);
        Beryll::Renderer::disableFaceCulling();
        m_shadowMap->drawIntoShadowMap(m_simpleObjForShadowMap, m_animatedObjForShadowMap, m_sunLightVPMatrix);
        Beryll::Renderer::enableFaceCulling();

        // 2. Draw scene.
        glm::mat4 modelMatrix{1.0f};

        m_simpleObjSunLightShadows->bind();
        m_simpleObjSunLightShadows->set3Float("sunLightDir", m_sunLightDir);
        m_simpleObjSunLightShadows->set3Float("cameraPos", Beryll::Camera::getCameraPos());
        m_simpleObjSunLightShadows->set1Float("ambientLight", 0.5f);
        m_simpleObjSunLightShadows->set1Float("specularLightStrength", 3.0f);


        modelMatrix = m_player->getObj()->getModelMatrix();
        m_simpleObjSunLightShadows->setMatrix4x4Float("MVPLightMatrix", m_sunLightVPMatrix * modelMatrix);
        m_simpleObjSunLightShadows->setMatrix4x4Float("modelMatrix", modelMatrix);
        m_simpleObjSunLightShadows->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
        Beryll::Renderer::drawObject(m_player->getObj(), modelMatrix, m_simpleObjSunLightShadows);
        if(m_player->getIsMeteor())
            m_player->spamMeteorParticles();

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

        m_simpleObjSunLightShadows->set1Float("specularLightStrength", 2.0f);
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
        m_animatedObjSunLightShadows->set1Float("specularLightStrength", 1.5f);

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

    void BaseMap::loadShaders()
    {
        m_simpleObjSunLightShadows = Beryll::Renderer::createShader("shaders/GLES/SimpleObjectSunLightShadows.vert",
                                                                    "shaders/GLES/SimpleObjectSunLightShadows.frag");
        m_simpleObjSunLightShadows->bind();
        m_simpleObjSunLightShadows->activateDiffuseTextureMat1();
        m_simpleObjSunLightShadows->activateShadowMapTexture();

//        m_simpleObjSunLightShadowsNormals = Beryll::Renderer::createShader("shaders/GLES/SimpleObjectSunLightShadowsNormals.vert",
//                                                                           "shaders/GLES/SimpleObjectSunLightShadowsNormals.frag");
//        m_simpleObjSunLightShadowsNormals->bind();
//        m_simpleObjSunLightShadowsNormals->activateDiffuseTextureMat1();
//        m_simpleObjSunLightShadowsNormals->activateNormalMapTextureMat1();
//        m_simpleObjSunLightShadowsNormals->activateShadowMapTexture();

        m_animatedObjSunLightShadows = Beryll::Renderer::createShader("shaders/GLES/AnimatedObjectSunLightShadows.vert",
                                                                      "shaders/GLES/AnimatedObjectSunLightShadows.frag");
        m_animatedObjSunLightShadows->bind();
        m_animatedObjSunLightShadows->activateDiffuseTextureMat1();
        m_animatedObjSunLightShadows->activateShadowMapTexture();

        m_shadowMap = Beryll::Renderer::createShadowMap(2500, 2500);
    }

    void BaseMap::updateSunPosition(const glm::vec3& pos, float clipCubeWidth, float clipCubeHeight, float clipCubeDepth)
    {
        glm::mat4 lightProjection = glm::ortho(-clipCubeWidth * 0.5f, clipCubeWidth * 0.5f, -clipCubeHeight * 0.5f, clipCubeHeight * 0.5f, 15.0f, clipCubeDepth);
        glm::mat4 lightView = glm::lookAt(pos, pos + m_sunLightDir, BeryllConstants::worldUp);

        m_sunLightVPMatrix = lightProjection * lightView;
    }

    void BaseMap::handleScreenSwipe()
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

            glm::vec3 powerForImpulse = m_screenSwipe3D * EnumsAndVariables::playerImpulseFactorOnGround;
            glm::vec3 powerForTorque = m_screenSwipe3D * EnumsAndVariables::playerTorqueFactorOnGround;
            if(m_player->getIsOnBuilding())
            {
                powerForImpulse = m_screenSwipe3D *  EnumsAndVariables::playerImpulseFactorOnBuilding;
                powerForTorque = m_screenSwipe3D * EnumsAndVariables::playerTorqueFactorOnBuilding;
            }
            else if(m_player->getIsOnAir())
            {
                powerForImpulse = m_screenSwipe3D *  EnumsAndVariables::playerImpulseFactorOnAir;
                powerForTorque = m_screenSwipe3D * EnumsAndVariables::playerTorqueFactorOnAir;
            }

            float factorImpulseApplied = m_player->handleScreenSwipe(powerForImpulse, powerForTorque);

            // Apply impulse to garbage too.
            glm::vec3 impulseForGarbage = powerForImpulse * factorImpulseApplied * EnumsAndVariables::playerMassToGarbageMassRatio;
            // We apply only impulse for garbage. But for player were applied impulse + torque.
            // This compensate player speed increased by torque.
            impulseForGarbage *= 1.2f;
            for(const auto& wrapper : m_allGarbageWrappers)
            {
                if(wrapper.isMagnetized)
                    wrapper.obj->applyCentralImpulse(impulseForGarbage);
            }
        }
    }

    void BaseMap::magnetizeGarbageAndUpdateGravity()
    {
        EnumsAndVariables::garbageCountMagnetized = 0;

        // Check if magnetized garbage still in playerMagneticRadius.
        for(auto& wrapper : m_allGarbageWrappers)
        {
            if(wrapper.getIsEnabled() && wrapper.isMagnetized &&
               glm::distance(m_player->getObj()->getOrigin(), wrapper.obj->getOrigin()) < EnumsAndVariables::playerMagneticRadius)
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
                   glm::distance(m_player->getObj()->getOrigin(), wrapper.obj->getOrigin()) < EnumsAndVariables::playerMagneticRadius)
                {
                    ++EnumsAndVariables::garbageCountMagnetized;
                    wrapper.isMagnetized = true;

                    if(EnumsAndVariables::garbageMaxCountMagnetized == EnumsAndVariables::garbageCountMagnetized)
                        break;
                }
            }
        }

        float gravPower = EnumsAndVariables::garbageMinGravityPower + (m_player->getMoveSpeed() * EnumsAndVariables::garbageGravityIncreasedByPlayerSpeed);
        if(gravPower > EnumsAndVariables::garbageMaxGravityPower)
            gravPower = EnumsAndVariables::garbageMaxGravityPower;

        for(auto& wrapper : m_allGarbageWrappers)
        {
            if(wrapper.isMagnetized)
            {
                glm::vec3 gravDir = glm::normalize(m_player->getObj()->getOrigin() - wrapper.obj->getOrigin());
                if(Beryll::Physics::getIsCollision(m_player->getObj()->getID(), wrapper.obj->getID()))
                {
                    wrapper.obj->setGravity(-(gravDir * gravPower * 0.05f), false, false);
                }
                else
                {
                    wrapper.obj->setGravity(gravDir * gravPower, false, true);
                }

                // Stop garbage if it stats rotating around player too fast.
                const glm::vec3 linVelocity = wrapper.obj->getLinearVelocity();
                const glm::vec3 objMoveDir = glm::normalize(linVelocity);
                const float objSpeed = glm::length(linVelocity);
                const glm::vec3 objToPlayerDir = glm::normalize(m_player->getObj()->getOrigin() - wrapper.obj->getOrigin());

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

    void BaseMap::updatePathfindingAndSpawnEnemies()
    {
        // In first frame:
        // 1. Find closest point to player.
        // 2. Find allowed points to spawn enemies.
        if(m_pathFindingIteration == 0)
        {
            ++m_pathFindingIteration; // Go to next iteration in next frame.

            m_allowedPointsToSpawnEnemies.clear();
            m_allowedPointsToSpawnGarbage.clear();

            glm::vec2 playerPosXZ{m_player->getObj()->getOrigin().x, m_player->getObj()->getOrigin().z};
            float distanceToClosestPoint = std::numeric_limits<float>::max();
            float distanceToCurrent = 0.0f;

            for(const glm::ivec2& point : m_pathAllowedPositionsXZ)
            {
                distanceToCurrent = glm::distance(playerPosXZ, glm::vec2(point));

                // 1.
                if(distanceToCurrent < distanceToClosestPoint)
                {
                    distanceToClosestPoint = distanceToCurrent;
                    m_playerClosestAllowedPos = point;
                }

                // 2.
                if(distanceToCurrent > EnumsAndVariables::minDistanceToSpawnEnemies && distanceToCurrent < EnumsAndVariables::maxDistanceToSpawnEnemies)
                {
                    // We can spawn enemy at this point.
                    m_allowedPointsToSpawnEnemies.push_back(point);
                }

                if(distanceToCurrent > EnumsAndVariables::minDistanceToSpawnGarbage && distanceToCurrent < EnumsAndVariables::maxDistanceToSpawnGarbage)
                {
                    // We can spawn garbage at this point.
                    m_allowedPointsToSpawnGarbage.push_back(point);
                }
            }

            BR_ASSERT((!m_allowedPointsToSpawnEnemies.empty()), "%s", "m_allowedPointsToSpawnEnemies empty.");
            BR_ASSERT((!m_allowedPointsToSpawnGarbage.empty()), "%s", "m_allowedPointsToSpawnGarbage empty.");
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

                        const glm::ivec2& spawnPoint2D = m_allowedPointsToSpawnEnemies[Beryll::RandomGenerator::getInt(m_allowedPointsToSpawnEnemies.size() - 1)];
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

            if(EnumsAndVariables::currentPathfindingEnemyIndex + 1 >= m_allAnimatedEnemies.size())
            {
                // All enemies were updated.
                EnumsAndVariables::currentPathfindingEnemyIndex = 0;
                m_pathFindingIteration = 0; // Start from start again in next frame.
            }

        }
    }

    void BaseMap::handleEnemiesAttacks()
    {
        for(const auto& enemy : m_allAnimatedEnemies)
        {
            if(enemy->getIsEnabledUpdate() && enemy->unitState == UnitState::CAN_ATTACK)
            {
                glm::vec3 from = enemy->getOrigin();
                from.y += enemy->getController().getFromOriginToTop() * 0.7f;
                from += enemy->getFaceDirXZ() * 6.0f;
                glm::vec3 target = m_player->getObj()->getOrigin();
                target.y += 1.8f;
                Beryll::RayClosestHit rayAttack = Beryll::Physics::castRayClosestHit(from, target,
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

                    enemy->attack(m_player->getObj()->getOrigin());

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

    void BaseMap::emitParticlesLine(const glm::vec3& from, const glm::vec3& to, const float sizeBegin, const float sizeEnd,
                                                const glm::vec4& colorBegin, const glm::vec4& colorEnd, const float lifeTime)
    {
        glm::vec3 dir = glm::normalize(to - from);
        float distance = glm::distance(to, from);
        const float spamOffsetMeters = 0.6f;
        glm::vec3 spamPoint{0.0f};
        for(float i = 0.0f; i < distance; i += spamOffsetMeters)
        {
            spamPoint = from + (dir * i);

            Beryll::ParticleSystem::EmitCubesFromCenter(1, lifeTime, sizeBegin, sizeEnd,
                                                        colorBegin, colorEnd,
                                                        spamPoint, glm::vec3(0.0f), 0);
        }
    }

    void BaseMap::emitParticlesExplosion(const glm::vec3& orig, const int count, const float sizeBegin, const float sizeEnd,
                                                     const glm::vec4& colorBegin, const glm::vec4& colorEnd, const float lifeTime)
    {
        Beryll::ParticleSystem::EmitCubesFromCenter(count, lifeTime, sizeBegin, sizeEnd,
                                                    colorBegin, colorEnd,
                                                    orig, glm::vec3(0.0f), 10);
    }

    void BaseMap::killEnemies()
    {
        float speedToReduce = 0.0f;
        for(const auto& enemy : m_allAnimatedEnemies)
        {
            if(enemy->getIsEnabledUpdate() && glm::distance(enemy->getOrigin(), m_player->getObj()->getOrigin()) < EnumsAndVariables::radiusToKillEnemies)
            {
                enemy->disableEnemy();
                speedToReduce += enemy->getPlayerSpeedReduceWhenDie();

                // Spawn one garbage.
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

        m_player->reduceSpeed(speedToReduce);
    }

    void BaseMap::handleCamera()
    {
        const glm::vec3 cameraBackXZ = Beryll::Camera::getCameraBackDirectionXZ();
        glm::vec3 desiredCameraBackXZ = Beryll::Camera::getCameraBackDirectionXZ();

        if(Beryll::Physics::getIsCollisionWithGroup(m_player->getObj()->getID(), Beryll::CollisionGroups::GROUND))
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

        if(EnumsAndVariables::cameraRotateTime + EnumsAndVariables::cameraRotateDelay < Beryll::TimeStep::getSecFromStart() &&
           !glm::any(glm::isnan(cameraBackXZ)) && !glm::any(glm::isnan(desiredCameraBackXZ)))
        {
            EnumsAndVariables::cameraRotateTime = Beryll::TimeStep::getSecFromStart();

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
        m_cameraOffset.y = 0.035f + EnumsAndVariables::cameraYAccordingPlayerY * m_player->getObj()->getOrigin().y;
        m_cameraOffset = glm::normalize(m_cameraOffset);

        m_cameraFront = m_player->getObj()->getOrigin();
        m_cameraFront.y += 15.0f;

        float maxCameraDistance = m_startCameraDistance + EnumsAndVariables::garbageCountMagnetized * 0.15f;
        glm::vec3 cameraPosForRay = m_cameraFront + m_cameraOffset * (maxCameraDistance + 2.0f); // + 2m behind camera.

        // Check camera ray collisions.
        Beryll::RayClosestHit rayBuildingHit = Beryll::Physics::castRayClosestHit(m_cameraFront,
                                                                                 cameraPosForRay,
                                                                                 Beryll::CollisionGroups::RAY_FOR_BUILDING_CHECK,
                                                                                 Beryll::CollisionGroups::BUILDING);

        if(rayBuildingHit)
        {
            float hitDistance = glm::max(glm::length(m_cameraFront - rayBuildingHit.hitPoint), 3.0f);
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

    void BaseMap::spawnGarbage(const int count, GarbageType type)
    {
        if(count == 0 ||
           (type == GarbageType::DEFAULT && Garbage::getActiveDefaultGarbageCount() >= EnumsAndVariables::garbageMaxCountActiveDefault))
            return;

        int spawnedCount = 0;
        const glm::ivec2& spawnPoint2D = m_allowedPointsToSpawnGarbage[Beryll::RandomGenerator::getInt(m_allowedPointsToSpawnGarbage.size() - 1)];
        glm::vec3 spawnPoint3D{spawnPoint2D.x, 7.0f, spawnPoint2D.y};

        for(auto& wrapper : m_allGarbageWrappers)
        {
            if(!wrapper.getIsEnabled() && wrapper.getType() == type)
            {
                wrapper.enableGarbage();
                spawnPoint3D.x += (Beryll::RandomGenerator::getFloat() - 0.5f) * 5.0f;
                spawnPoint3D.z += (Beryll::RandomGenerator::getFloat() - 0.5f) * 4.0f;
                wrapper.obj->setOrigin(spawnPoint3D);

                ++spawnedCount;
            }

            if(spawnedCount >= count)
                break;
        }
    }
}
