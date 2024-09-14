#include "BaseMap.h"
#include "DataBaseHelper.h"
#include "Sounds.h"

namespace MagneticBall3D
{
    BaseMap::BaseMap(std::shared_ptr<PlayStateGUILayer> gui) : m_gui(std::move(gui))
    {
        Beryll::LoadingScreen::showProgress(100.0f);

        // Allocate enough spase for all vectors to avoid vector reallocation.
        m_allGarbage.reserve(m_maxGarbageCount);
        m_allAnimatedEnemies.reserve(m_maxEnemiesCount);
        m_animatedOrDynamicObjects.reserve(m_maxEnemiesCount + m_maxGarbageCount);
        m_staticEnv.reserve(m_maxEnvCount);
        m_simpleObjForShadowMap.reserve(m_maxEnvCount + m_maxGarbageCount);
        m_animatedObjForShadowMap.reserve(m_maxEnemiesCount);
    }

    BaseMap::~BaseMap()
    {

    }

    void BaseMap::updateBeforePhysics()
    {
        if(m_gui->resurrectPlayer)
        {
            m_gui->resurrectPlayer = false;

            --EnumsAndVars::playerResurrectionAttempts;

            SendStatisticsHelper::canSendPlayerDie();

            m_player->resurrect();

            BR_INFO("%s", "m_player->resurrect().");

            float mapXLength = glm::distance(m_minX, m_maxX);
            float mapZLength = glm::distance(m_minZ, m_maxZ);
            respawnEnemiesAtNewDistance(glm::max(mapXLength, mapZLength) * 0.5f);

            // Set "stand" animation for all active enemies and pause them to give player some time to prepare for fight.
            for(const auto& enemy : m_allAnimatedEnemies)
            {
                if(enemy->getIsEnabledUpdate())
                    enemy->setCurrentAnimationByIndex(EnumsAndVars::AnimationIndexes::stand, false, false, true);
            }

            EnumsAndVars::enemiesPauseAllActionsTime = EnumsAndVars::mapPlayTimeSec;
            EnumsAndVars::enemiesPauseAllActionsDelay = 12.0f;
        }

        if(EnumsAndVars::gameOnPause)
            return;

        m_improvements.update();

        if(EnumsAndVars::improvementSystemOnScreen)
            return;

        EnumsAndVars::mapPlayTimeSec += Beryll::TimeStep::getTimeStepSec();

        Sounds::update();

        handlePlayerDie();
        handlePlayerWin();

        handleScreenSwipe();
        if(EnumsAndVars::enemiesPauseAllActionsTime + EnumsAndVars::enemiesPauseAllActionsDelay < EnumsAndVars::mapPlayTimeSec)
            updatePathfindingAndSpawnEnemies();
        spawnCommonGarbage();
        handleJumpPads();
        checkPlayerOutOfMap();
    }

    void BaseMap::updateAfterPhysics()
    {
        if(EnumsAndVars::gameOnPause || EnumsAndVars::improvementSystemOnScreen)
            return;

        const float distanceToEnableObjects = m_cameraDistance * 1.1f;

        for(const std::shared_ptr<Beryll::SceneObject>& so : m_animatedOrDynamicObjects)
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
            }
            else
            {
                so->disableDraw();
            }
        }

        m_player->update();
        if(EnumsAndVars::enemiesPauseAllActionsTime + EnumsAndVars::enemiesPauseAllActionsDelay < EnumsAndVars::mapPlayTimeSec)
            updateEnemiesAndTheirsAttacks();
        updateAndMagnetizeGarbage();
        killEnemies();

        if(EnumsAndVars::enemiesLastWavePhase && BaseEnemy::getActiveCount() <= 0)
        {
            BR_INFO("%s", "if(EnumsAndVars::enemiesLastWavePhase && BaseEnemy::getActiveCount() <= 0)");

            if(EnumsAndVars::mapHasBossPhase)
                startBossPhase();
            else
                EnumsAndVars::mapPlayerWin = true;
        }
        else if(EnumsAndVars::bossPhase)
            handlePossPhase();

        handleCamera();
        updateGUI();
    }

    void BaseMap::loadShaders()
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

        m_animatedObjSunLight = Beryll::Renderer::createShader("shaders/GLES/AnimatedObjectSunLight.vert",
                                                               "shaders/GLES/AnimatedObjectSunLight.frag");
        m_animatedObjSunLight->bind();
        m_animatedObjSunLight->activateDiffuseTextureMat1();

        m_shadowMap = Beryll::Renderer::createShadowMap(3500, 3500);
    }

    void BaseMap::loadPlayer()
    {
        auto playerAllBalls = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/player/Player6Models.fbx",
                                                                                       EnumsAndVars::playerMass,
                                                                                       true,
                                                                                       Beryll::CollisionFlags::DYNAMIC,
                                                                                       Beryll::CollisionGroups::PLAYER,
                                                                                       Beryll::CollisionGroups::GROUND | Beryll::CollisionGroups::BUILDING |
                                                                                       Beryll::CollisionGroups::GARBAGE | Beryll::CollisionGroups::ENEMY_ATTACK |
                                                                                       Beryll::CollisionGroups::JUMPPAD | Beryll::CollisionGroups::BOSS,
                                                                                       Beryll::SceneObjectGroups::PLAYER);

        // Sort by radius from small to large.
        std::sort(playerAllBalls.begin(), playerAllBalls.end(),
                  [](const std::shared_ptr<Beryll::SimpleCollidingObject>& o1, const std::shared_ptr<Beryll::SimpleCollidingObject>& o2) { return o1->getXZRadius() < o2->getXZRadius(); });
        // Disable all.
        for(const auto& item : playerAllBalls)
        {
            item->disableCollisionMesh();
            item->disableUpdate();
            item->disableDraw();
            item->setDamping(EnumsAndVars::playerLinearDamping, EnumsAndVars::playerAngularDamping);

            m_animatedOrDynamicObjects.push_back(item);
            m_simpleObjForShadowMap.push_back(item);
        }

        m_player = std::make_shared<Player>(playerAllBalls[0], EnumsAndVars::playerStartHP);
        m_player->setAllModels(playerAllBalls);

        m_player->getObj()->setGravity(EnumsAndVars::playerGravityOnGround);
        m_player->getObj()->setFriction(EnumsAndVars::playerFriction);
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
                m_fingerLastPos = f.SDL2ScreenPos;
                glm::vec3 touchImpulse = Beryll::Camera::getCameraFrontDirectionXZ() * 40.0f; // impulse 10 = +1 speed.
                if(m_firstTouch)
                {
                    m_firstTouch = false;
                    touchImpulse = Beryll::Camera::getCameraFrontDirectionXZ() * 500.0f;
                }
                m_player->handleScreenSwipe(touchImpulse, touchImpulse);
            }

            if(f.ID == m_fingerDownID)
            {
                fingerStillOnScreen = true;
                //BR_INFO("%s", "fingerStillOnScreen = true");
                m_fingerCurrentPos = f.SDL2ScreenPos;
                break;
            }
        }

        if(!fingerStillOnScreen && m_fingerDownID != -1)
        {
            ++EnumsAndVars::mapSwipeCount;
            m_fingerDownID = -1;
        }
        else if(fingerStillOnScreen && glm::distance(m_fingerCurrentPos, m_fingerLastPos) > 0.01f)
        {
            const glm::vec2 screenSwipe = (m_fingerCurrentPos - m_fingerLastPos);
            m_fingerLastPos = m_fingerCurrentPos;
            m_screenSwipe3D = glm::vec3{-screenSwipe.y, 0.0f, screenSwipe.x};
            const float screenSwipeLength = glm::length(m_screenSwipe3D);

            glm::qua cameraRotFromStartDir = glm::normalize(glm::rotation(m_startDir, Beryll::Camera::getCameraFrontDirectionXZ()));
            m_screenSwipe3D = glm::normalize(cameraRotFromStartDir * glm::normalize(m_screenSwipe3D));
            m_screenSwipe3D *= (screenSwipeLength * EnumsAndVars::swipePowerMultiplier);

            glm::vec3 powerForImpulse{0.0f};
            glm::vec3 powerForTorque{0.0f};
            float swipeFactorBasedOnAngleAndSpeed = 0.0f;
            if(m_player->getMoveSpeedXZ() > 0.001f)
            {
                if(!BeryllUtils::Common::getIsVectorsParallelInSameDir(m_player->getMoveDir(), glm::normalize(m_screenSwipe3D)))
                {
                    float moveToSwipeAngle = BeryllUtils::Common::getAngleInRadians(m_player->getMoveDir(), glm::normalize(m_screenSwipe3D));
                    swipeFactorBasedOnAngleAndSpeed = moveToSwipeAngle * (m_player->getMoveSpeedXZ() * 0.8f) * EnumsAndVars::playerLeftRightTurnPower;

                    // For turn back.
                    if(moveToSwipeAngle > 2.356f) // > 135 degrees.
                    {
                        swipeFactorBasedOnAngleAndSpeed *= 1.8f;
                        // With bigger impulse the turn back become too sharper. Reduce it with impulse grown.
                        swipeFactorBasedOnAngleAndSpeed *= EnumsAndVars::playerImpulseFactorOnGroundDefault / EnumsAndVars::playerImpulseFactorOnGround;
                    }
                }
            }

            // 2 m = radius for default ball.
            const float radiusForTorqueMultiplier = std::max(1.0f, m_player->getObj()->getXZRadius() * 0.5f);

            if(m_player->getIsOnGround())
            {
                powerForImpulse = m_screenSwipe3D * EnumsAndVars::playerImpulseFactorOnGround;
                powerForImpulse += powerForImpulse * swipeFactorBasedOnAngleAndSpeed;
                powerForTorque = m_screenSwipe3D * EnumsAndVars::playerTorqueFactorOnGround;
                powerForTorque += powerForTorque * swipeFactorBasedOnAngleAndSpeed;

                powerForTorque *= radiusForTorqueMultiplier;
            }
            else if(m_player->getLastTimeOnBuilding() + 0.1f > EnumsAndVars::mapPlayTimeSec)
            {
                // buildingNormalAngle = glm::half_pi<float>() if player on vertical wall. angleFactor will = 1.
                // buildingNormalAngle = 0 if player on horizontal roof. angleFactor will = 0.
                const float angleFactor = m_player->getBuildingNormalAngle() / glm::half_pi<float>();

                const float impulseDiff = EnumsAndVars::playerImpulseFactorOnBuildingWall - EnumsAndVars::playerImpulseFactorOnBuildingRoof;
                const float torqueDiff = EnumsAndVars::playerTorqueFactorOnBuildingWall - EnumsAndVars::playerTorqueFactorOnBuildingRoof;

                powerForImpulse = m_screenSwipe3D * (EnumsAndVars::playerImpulseFactorOnBuildingRoof + impulseDiff * angleFactor);
                powerForTorque = m_screenSwipe3D * (EnumsAndVars::playerTorqueFactorOnBuildingRoof + torqueDiff * angleFactor);

                powerForTorque *= radiusForTorqueMultiplier;

                powerForImpulse += powerForImpulse * (swipeFactorBasedOnAngleAndSpeed * (1.0f - angleFactor));
                powerForTorque += powerForTorque * (swipeFactorBasedOnAngleAndSpeed * (1.0f - angleFactor));
            }
            else // if(m_player->getIsOnAir())
            {
                powerForImpulse = m_screenSwipe3D * EnumsAndVars::playerImpulseFactorOnAir;
                powerForTorque = m_screenSwipe3D * EnumsAndVars::playerTorqueFactorOnAir;
            }

            // Help to player move faster on flat surface when speed is low.
            if(m_player->getMoveSpeedXZ() < EnumsAndVars::playerMaxSpeedXZDefault)
            {
                const float powerToHelpPlayer = (EnumsAndVars::playerMaxSpeedXZDefault - m_player->getMoveSpeedXZ()) * 0.009f;

                if(m_player->getLastTimeOnGround() + 0.1f > EnumsAndVars::mapPlayTimeSec ||
                   m_player->getLastTimeOnJumpPad() + 0.1f > EnumsAndVars::mapPlayTimeSec)
                {
                    powerForImpulse += powerForImpulse * powerToHelpPlayer;
                    powerForTorque += powerForTorque * powerToHelpPlayer;
                }
                else if(m_player->getLastTimeOnBuilding() + 0.1f > EnumsAndVars::mapPlayTimeSec)
                {
                    const float angleFactor = m_player->getBuildingNormalAngle() / glm::half_pi<float>();
                    powerForImpulse += powerForImpulse * (powerToHelpPlayer * (1.0f - angleFactor));
                    powerForTorque += powerForTorque * (powerToHelpPlayer * (1.0f - angleFactor));
                }
            }

            // If max allowed speed exceeded not all impulse power will applied.
            // In this case applyImpulseFactor shows how much was applied in range 0...1.
            float applyImpulseFactor = m_player->handleScreenSwipe(powerForImpulse, powerForTorque);

            glm::vec3 garbageImpulse{0.0f};

            if(m_player->getIsOnGround())
            {
                // Same as inside m_player->handleScreenSwipe(impulse, torque).
                garbageImpulse = (powerForImpulse * 0.00007f) * applyImpulseFactor;
                if(applyImpulseFactor == 1.0f)
                    garbageImpulse += powerForTorque * 0.00002f;
            }
            else if(m_player->getLastTimeOnBuilding() + 0.1f > EnumsAndVars::mapPlayTimeSec)
            {
                // Same as inside m_player->handleScreenSwipe(impulse, torque).
                glm::vec3 garbageImpulseOnRoof = (powerForImpulse * 0.00006f) * applyImpulseFactor;
                if(applyImpulseFactor == 1.0f)
                    garbageImpulseOnRoof += powerForTorque * 0.00001f;

                const glm::vec3 garbageImpulseOnWall = BeryllConstants::worldUp * screenSwipeLength * 0.000016f;

                const float angleFactor = m_player->getBuildingNormalAngle() / glm::half_pi<float>();
                garbageImpulse = glm::mix(garbageImpulseOnRoof, garbageImpulseOnWall, angleFactor);
            }

            if(!glm::any(glm::isnan(garbageImpulse)) && garbageImpulse != glm::vec3{0.0f})
            {
                for(const auto& wrapper : m_allGarbage)
                {
                    if(wrapper.isMagnetized)
                        wrapper.obj->applyCentralImpulse(garbageImpulse);
                }
            }
        }
    }

    void BaseMap::updateAndMagnetizeGarbage()
    {
        EnumsAndVars::garbageCountMagnetized = 0;

        // Check if magnetized garbage still in playerMagneticRadius.
        for(auto& wrapper : m_allGarbage)
        {
            if(wrapper.getIsEnabled())
                wrapper.update();

            if(wrapper.getIsEnabled() && wrapper.isMagnetized && wrapper.getCanBeMagnetized() &&
               glm::distance(m_player->getObj()->getOrigin(), wrapper.obj->getOrigin()) < EnumsAndVars::playerMagneticRadius)
            {
                ++EnumsAndVars::garbageCountMagnetized;
            }
            else
            {
                wrapper.isMagnetized = false;
            }
        }

        // Update gravity. And check for more garbage if we have limit for that.
        float gravPower = EnumsAndVars::garbageMinGravityPower + (m_player->getMoveSpeed() * EnumsAndVars::garbageGravityIncreasedByPlayerSpeed);

        const float speedToResetVelocity = m_player->getMoveSpeed() * 1.5f;
        const bool stopGarbage = m_player->getMoveSpeed() > 7.0f ||
                                 (m_player->getLastTimeOnGround() + 0.5f < EnumsAndVars::mapPlayTimeSec &&
                                  m_player->getLastTimeOnBuilding() + 0.5f < EnumsAndVars::mapPlayTimeSec);

        for(auto& wrapper : m_allGarbage)
        {
            // 1. Update gravity.
            if(wrapper.isMagnetized)
            {
                glm::vec3 gravDir = glm::normalize(m_player->getObj()->getOrigin() - wrapper.obj->getOrigin());
                wrapper.obj->setGravity(gravDir * gravPower, false, false);

                if(stopGarbage)
                {
                    // Stop garbage if it stats rotating around player too fast.
                    const glm::vec3 linVelocity = wrapper.obj->getLinearVelocity();
                    const glm::vec3 objMoveDir = glm::normalize(linVelocity);
                    const float objSpeed = glm::length(linVelocity);
                    const glm::vec3 objToPlayerDir = glm::normalize(m_player->getObj()->getOrigin() - wrapper.obj->getOrigin());

                    if(objSpeed > speedToResetVelocity && BeryllUtils::Common::getAngleInRadians(objToPlayerDir, objMoveDir) > 0.35f) // > 20 degrees.
                    {
                        wrapper.obj->setLinearVelocity(objToPlayerDir * 16.0f);
                    }
                }
            }
            else if(wrapper.getIsEnabled())
            {
                wrapper.obj->setGravity(EnumsAndVars::garbageGravityDefault, false, false);
            }

            // 2. Check for more garbage if we have limit for that.
            if(EnumsAndVars::garbageCountMagnetized < EnumsAndVars::garbageMaxCountMagnetized &&
               wrapper.getIsEnabled() && !wrapper.isMagnetized && wrapper.getCanBeMagnetized() &&
               glm::distance(m_player->getObj()->getOrigin(), wrapper.obj->getOrigin()) < EnumsAndVars::playerMagneticRadius)
            {
                ++EnumsAndVars::garbageCountMagnetized;
                wrapper.isMagnetized = true;
                wrapper.obj->activate();
                const glm::vec3 objToPlayerDir = glm::normalize(m_player->getObj()->getOrigin() - wrapper.obj->getOrigin());
                wrapper.obj->setLinearVelocity(objToPlayerDir * 16.0f);
            }
        }

        //BR_INFO("magnetized %d resetCount %d", EnumsAndVariables::garbageCountMagnetized, resetCount);
    }

    void BaseMap::updatePathfindingAndSpawnEnemies()
    {
        // In first frame:
        // 1. Find closest point to player.
        // 2. Find allowed points to spawn enemies.
        if(m_pathFindingIteration == 0)
        {
            ++m_pathFindingIteration; // Go to next iteration in next frame.

            m_pointsToSpawnEnemies.clear();
            m_pointsToSpawnCommonGarbage.clear();

            glm::vec2 playerPosXZ{m_player->getObj()->getOrigin().x, m_player->getObj()->getOrigin().z};
            float distanceToClosestPoint = std::numeric_limits<float>::max();
            float distanceToCurrent = 0.0f;

            for(const glm::ivec2& point : m_pathAllowedPositionsXZ)
            {
                distanceToCurrent = glm::distance(playerPosXZ, glm::vec2(float(point.x), float(point.y)));

                // 1.
                if(distanceToCurrent < distanceToClosestPoint)
                {
                    distanceToClosestPoint = distanceToCurrent;
                    m_playerClosestAllowedPos = point;
                }

                // 2.
                if(distanceToCurrent > EnumsAndVars::enemiesMinDistanceToSpawn && distanceToCurrent < EnumsAndVars::enemiesMaxDistanceToSpawn)
                {
                    // We can spawn enemy at this point.
                    m_pointsToSpawnEnemies.push_back(point);
                }

                if(distanceToCurrent > EnumsAndVars::garbageCommonSpawnMinDistance && distanceToCurrent < EnumsAndVars::garbageCommonSpawnMaxDistance)
                {
                    // We can spawn garbage at this point.
                    m_pointsToSpawnCommonGarbage.push_back(point);
                }
            }

            BR_ASSERT((!m_pointsToSpawnEnemies.empty()), "%s", "m_allowedPointsToSpawnEnemies empty.");
            BR_ASSERT((!m_pointsToSpawnCommonGarbage.empty()), "%s", "m_allowedPointsToSpawnGarbage empty.");
        }
        // In second frame:
        // 1. Clear blocked positions.
        // 2. Spawn enemies. In subclass.
        else if(m_pathFindingIteration == 1)
        {
            ++m_pathFindingIteration;

            // 1.
            m_pathFinderEnemies.clearBlockedPositions();

            // 2.
            spawnEnemies();
        }
        // In third frame:
        // 1. Update paths for enemies.
        else if(m_pathFindingIteration == 2)
        {
            int enemiesUpdated = 0;
            int& i = EnumsAndVars::enemiesCurrentPathfindingIndex;
            for( ; i < m_allAnimatedEnemies.size(); ++i)
            {
                if(enemiesUpdated >= EnumsAndVars::enemiesMaxPathfindingInOneFrame)
                    break;

                if(m_allAnimatedEnemies[i]->getIsEnabledUpdate() && m_allAnimatedEnemies[i]->getIsCanMove())
                {
                    m_allAnimatedEnemies[i]->setPathArray(m_pathFinderEnemies.findPath(m_allAnimatedEnemies[i]->getCurrentPointToMove2DInt(), m_playerClosestAllowedPos, 7), 0);

                    m_pathFinderEnemies.addBlockedPosition(m_allAnimatedEnemies[i]->getCurrentPointToMove2DInt());

                    ++enemiesUpdated;
                }
            }

            if(EnumsAndVars::enemiesCurrentPathfindingIndex >= m_allAnimatedEnemies.size())
            {
                // All enemies were updated.
                //BR_INFO("Path for all enemies updated. Last index: %d", EnumsAndVariables::enemiesCurrentPathfindingIndex);
                EnumsAndVars::enemiesCurrentPathfindingIndex = 0;
                m_pathFindingIteration = 0; // Start from start again in next frame.
            }

        }
    }

    void BaseMap::updateEnemiesAndTheirsAttacks()
    {
        glm::vec3 target = m_player->getObj()->getOrigin();
        target.y += m_player->getObj()->getFromOriginToTop() * 0.8f;

        for(const auto& enemy : m_allAnimatedEnemies)
        {
            if(!enemy->getIsEnabledUpdate())
                continue;

            // 1. Update enemy.
            enemy->update(m_player->getObj()->getOrigin());

            // 2. Handle enemy attack.
            if(enemy->unitState == UnitState::CAN_ATTACK)
            {
                Beryll::RayClosestHit rayAttack = Beryll::Physics::castRayClosestHit(enemy->getOrigin(),
                                                                                     target,
                                                                                     Beryll::CollisionGroups::ENEMY_ATTACK,
                                                                                     Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE);

                if(rayAttack)
                {
                    if(enemy->attackType != AttackType::MELEE_DAMAGE_ONE && enemy->attackType != AttackType::MAGNETIZE_GARBAGE)
                    {
                        // Spam particles.
                        glm::vec3 from = enemy->getOrigin(); // Calculate particles start point.

                        if(enemy->unitType == UnitType::ENEMY_SIT_WHEN_SHOOT)
                        {
                            from.y += 1.0f;
                            from += enemy->getFaceDirXZ() * 14.0f;
                        }
                        else if(enemy->unitType == UnitType::ENEMY_TANK)
                        {
                            from.y += enemy->getFromOriginToTop() * 0.8f;
                            from += enemy->getFaceDirXZ() * 30.0f;
                        }
                        else
                        {
                            from.y += enemy->getFromOriginToTop() * 0.75f;
                            from += enemy->getFaceDirXZ() * 15.0f;
                        }

                        emitParticlesLine(from, rayAttack.hitPoint, enemy->attackParticlesSize, enemy->attackParticlesSize * 0.4f,
                                          glm::vec4(enemy->attackParticlesColor, 1.0f), glm::vec4(enemy->attackParticlesColor, 0.7f), 0.4f);

                        if(enemy->attackType == AttackType::RANGE_DAMAGE_RADIUS)
                            emitParticlesExplosion(rayAttack.hitPoint, 6, enemy->attackParticlesSize * 2.5f, enemy->attackParticlesSize,
                                                   glm::vec4(enemy->attackParticlesColor, 1.0f), glm::vec4(enemy->attackParticlesColor, 0.7f), 1.0f);
                    }

                    enemy->attack(m_player->getObj()->getOrigin());

                    // Do damage.
                    if(rayAttack.hittedCollGroup == Beryll::CollisionGroups::PLAYER)
                    {
                        // Player attacked.
                        //BR_INFO("%s", "Player attacked");
                        m_player->takeDamage(enemy->damage);

                    }
                    else if(rayAttack.hittedCollGroup == Beryll::CollisionGroups::GARBAGE)
                    {
                        // Garbage under attack =).
                        if(enemy->attackType == AttackType::RANGE_DAMAGE_ONE || enemy->attackType == AttackType::MELEE_DAMAGE_ONE)
                        {
                            //BR_INFO("%s", "Garbage under attack =) by AttackType::RANGE_DAMAGE_ONE");
                            for(auto& wrapper : m_allGarbage)
                            {
                                if(rayAttack.hittedObjectID == wrapper.obj->getID())
                                {
                                    wrapper.takeDamage(enemy->damage);
                                    break;
                                }
                            }
                        }
                        else if(enemy->attackType == AttackType::RANGE_DAMAGE_RADIUS)
                        {
                            //BR_INFO("%s", "Garbage under attack =) by AttackType::RANGE_DAMAGE_RADIUS");
                            for(auto& wrapper : m_allGarbage)
                            {
                                if(wrapper.isMagnetized &&
                                   glm::distance(rayAttack.hitPoint, wrapper.obj->getOrigin()) < enemy->damageRadius)
                                {
                                    wrapper.takeDamage(enemy->damage);
                                }
                            }
                        }
                        else if(enemy->attackType == AttackType::MAGNETIZE_GARBAGE)
                        {
                            //BR_INFO("%s", "Garbage under attack =) by AttackType::MAGNETIZE_GARBAGE");
                            for(auto& wrapper : m_allGarbage)
                            {
                                if(rayAttack.hittedObjectID == wrapper.obj->getID())
                                {
                                    wrapper.pauseMagnetization(1.25f);
                                    wrapper.obj->setGravity(EnumsAndVars::garbageGravityDefault, false, false);
                                    glm::vec3 impulseDir = glm::normalize(enemy->getOrigin() - wrapper.obj->getOrigin());
                                    impulseDir.y += 0.4f;
                                    wrapper.obj->applyCentralImpulse(impulseDir * 0.1f);
                                    break;
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
        const glm::vec3 dir = glm::normalize(to - from);
        const float distance = glm::distance(to, from);
        const float spamOffsetMeters = sizeBegin * 7.5f;
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
        float radiusToKill = std::max(6.0f, m_player->getObj()->getXZRadius() * 1.4f) + EnumsAndVars::garbageCountMagnetized * 0.1f;

        if(m_player->getIsTouchGroundAfterFall() && m_player->getFallDistance() > 90.0f)
        {
            radiusToKill = EnumsAndVars::playerDamageGroundRadiusAfterFall;
            //BR_INFO("Damage radius after fall on ground: %f", radiusToKill);
            Sounds::playSoundEffect(SoundType::FELL_ON_GROUND);
        }

        float speedToReduce = 0.0f;
        int addToExp = 0;
        for(const auto& enemy : m_allAnimatedEnemies)
        {
            if(enemy->getIsEnabledUpdate() &&
               enemy->garbageAmountToDie < EnumsAndVars::garbageCountMagnetized &&
               glm::distance(enemy->getOrigin(), m_player->getObj()->getOrigin()) < radiusToKill + (enemy->getXZRadius() * 0.5f))
            {
                enemy->die();
                speedToReduce += enemy->reducePlayerSpeedWhenDie;
                addToExp += enemy->experienceWhenDie;
                ++EnumsAndVars::enemiesKilledCount;
                spawnGarbage(1, enemy->dieGarbageType, enemy->getOrigin());

                //BR_INFO("Kill enemy. active count: %d", AnimatedCollidingEnemy::getActiveCount());
            }
        }

        m_player->reduceSpeed(speedToReduce);
        m_player->addToExp(addToExp);
    }

    void BaseMap::handleCamera()
    {
        const glm::vec3& cameraBackXZ = Beryll::Camera::getCameraBackDirectionXZ();
        glm::vec3 desiredCameraBackXZ = Beryll::Camera::getCameraBackDirectionXZ();
        float cameraRotationSpeedFactor = 1.0f;

        if(m_player->getLastTimeOnBuilding() + 0.5f > EnumsAndVars::mapPlayTimeSec &&
           m_player->getMoveSpeedXZ() < 5.0f && m_player->getBuildingNormalAngle() > 1.3f)
        {
            desiredCameraBackXZ = glm::normalize(glm::vec3(m_player->getBuildingCollisionNormal().x, 0.0f, m_player->getBuildingCollisionNormal().z));
        }
        else if(m_player->getMoveSpeedXZ() > EnumsAndVars::minPlayerSpeedToCameraFollow)
        {
            desiredCameraBackXZ = -m_player->getMoveDirXZ();
            cameraRotationSpeedFactor *= m_player->getMoveSpeedXZ() / EnumsAndVars::playerMaxSpeedXZDefault;

            if(m_player->getLastTimeOnBuilding() + 0.5f > EnumsAndVars::mapPlayTimeSec)
            {
                const glm::vec3 swipeBackDir = -glm::normalize(glm::vec3(m_screenSwipe3D.x, 0.0f, m_screenSwipe3D.z));
                // On vertical wall angleFactor = 1. On horizontal roof angleFactor = 0.
                const float angleFactor = m_player->getBuildingNormalAngle() / glm::half_pi<float>();
                cameraRotationSpeedFactor = glm::mix(0.9f, 0.3f, angleFactor);
                desiredCameraBackXZ = glm::normalize(glm::mix(-m_player->getMoveDirXZ(), swipeBackDir, angleFactor));
                cameraRotationSpeedFactor *= m_player->getMoveSpeedXZ() / EnumsAndVars::playerMaxSpeedXZDefault;
            }
        }

        //BR_ASSERT((!glm::any(glm::isnan(cameraBackXZ)) && !glm::any(glm::isnan(desiredCameraBackXZ))), "%s", "Camera back dir in nan.");

        if(!glm::any(glm::isnan(cameraBackXZ)) && !glm::any(glm::isnan(desiredCameraBackXZ)) &&
           cameraBackXZ != desiredCameraBackXZ)
        {
            const glm::quat rotation = glm::rotation(cameraBackXZ, desiredCameraBackXZ);

            const float angleDifference = glm::angle(rotation);
            float angleRotate = angleDifference * 0.02f + 0.01f; // Good rotation speed for 60 FPS (0.01667 sec frametime).
            angleRotate *= Beryll::TimeStep::getTimeStepSec() / 0.01667f; // Make a correction if FPS != 60(0.01667 sec frametime).
            angleRotate *= cameraRotationSpeedFactor;
            if(angleRotate > angleDifference)
                angleRotate = angleDifference;
            if(angleRotate > 0.001f)
            {
                const glm::mat4 matr = glm::rotate(glm::mat4{1.0f}, angleRotate, glm::normalize(glm::axis(rotation)));
                m_cameraAngleOffset = matr * glm::vec4(cameraBackXZ, 1.0f);
            }
        }

        m_cameraAngleOffset.y = 0.0f;
        m_cameraAngleOffset = glm::normalize(m_cameraAngleOffset);
        m_cameraAngleOffset.y = 0.09f +
                                (EnumsAndVars::garbageCountMagnetized * 0.0015f) +
                                (m_player->getObj()->getOrigin().y * 0.006f);
        //m_cameraAngleOffset.y = std::min(1.1f, m_cameraAngleOffset.y);
        //BR_INFO("m_cameraAngleOffset.y %f", m_cameraAngleOffset.y);
        m_cameraAngleOffset = glm::normalize(m_cameraAngleOffset);

        m_cameraFront = m_player->getObj()->getOrigin();

        float maxCameraYOffset = m_startCameraYOffset +
                                 (EnumsAndVars::garbageCountMagnetized * 0.15f) +
                                 std::min(30.0f, m_player->getObj()->getOrigin().y * 0.08f + m_player->getMoveSpeedXZ() * 0.15f);

        if(!m_cameraHit)
        {
            if(glm::distance(m_cameraYOffset, maxCameraYOffset) < 0.105f)
            {
                m_cameraYOffset = maxCameraYOffset;
            }
            else
            {
                if(m_cameraYOffset < maxCameraYOffset)
                    m_cameraYOffset += EnumsAndVars::cameraUpOffsetMaxSpeed * std::min(0.04f, Beryll::TimeStep::getTimeStepSec());
                else
                    m_cameraYOffset -= EnumsAndVars::cameraUpOffsetMaxSpeed * std::min(0.04f, Beryll::TimeStep::getTimeStepSec());
            }
        }

        m_cameraFront.y += m_cameraYOffset;

        float maxCameraDistance = m_startCameraDistance +
                                  m_player->getMoveSpeedXZ() +
                                  (EnumsAndVars::garbageCountMagnetized * 0.6f) +
                                  (m_player->getObj()->getOrigin().y * 0.3f);

        glm::vec3 cameraPosForRay = m_cameraFront + m_cameraAngleOffset * maxCameraDistance;

        m_cameraHit = false;
        // Check camera ray collisions.
        Beryll::RayClosestHit rayCameraHit = Beryll::Physics::castRayClosestHit(m_cameraFront,
                                                                                 cameraPosForRay,
                                                                                 Beryll::CollisionGroups::CAMERA,
                                                                                 Beryll::CollisionGroups::BUILDING | Beryll::CollisionGroups::GROUND |
                                                                                 Beryll::CollisionGroups::BOSS);

        if(rayCameraHit)
        {
            m_cameraHit = true;

            float hitDistance = glm::length(m_cameraFront - rayCameraHit.hitPoint) * 0.9f;
            float hitDistanceFactor = hitDistance / maxCameraDistance;

            m_cameraDistance = maxCameraDistance * hitDistanceFactor;

            // m_cameraYOffset also should be changed if camera collision.
            float minCameraUpOffset = 8.0f + (EnumsAndVars::garbageCountMagnetized * 0.2f);
            m_cameraYOffset = std::max(minCameraUpOffset, maxCameraYOffset * hitDistanceFactor);
        }
        else if(glm::distance(m_cameraDistance, maxCameraDistance) < 0.565f)
        {
            m_cameraDistance = maxCameraDistance;
        }
        else
        {
            if(m_cameraDistance < maxCameraDistance)
                m_cameraDistance += EnumsAndVars::cameraZoomMaxSpeed * std::min(0.04f, Beryll::TimeStep::getTimeStepSec());
            else
                m_cameraDistance -= EnumsAndVars::cameraZoomMaxSpeed * std::min(0.04f, Beryll::TimeStep::getTimeStepSec());
        }

        if(glm::isnan(m_cameraDistance) || m_cameraDistance < 1.0f)
            m_cameraDistance = 1.0f;

        Beryll::Camera::setCameraPos(m_cameraFront + m_cameraAngleOffset * m_cameraDistance);
        Beryll::Camera::setCameraFrontPos(m_cameraFront);
    }

    void BaseMap::spawnGarbage(const int count, const GarbageType type, glm::vec3 spawnPoint)
    {
        int spawnedCount = 0;
        for(auto& wrapper : m_allGarbage)
        {
            if(spawnedCount >= count ||
               (type == GarbageType::COMMON && Garbage::getCommonActiveCount() >= EnumsAndVars::garbageCommonMaxCountOnMap))
                break;

            if(wrapper.getType() == type && !wrapper.getIsEnabled())
            {
                wrapper.enableGarbage();
                spawnPoint.x += (Beryll::RandomGenerator::getFloat() - 0.5f) * 6.0f;
                spawnPoint.z += (Beryll::RandomGenerator::getFloat() - 0.5f) * 8.0f;
                wrapper.obj->setOrigin(spawnPoint);

                ++spawnedCount;
            }
        }
    }

    void BaseMap::updateGUI()
    {
        m_gui->progressBarHP = float(m_player->getCurrentHP()) / float(m_player->getMaxHP());
        m_gui->progressBarXP = float(m_player->getCurrentLevelExp()) / float(m_player->getCurrentLevelMaxExp());
    }

    void BaseMap::handleJumpPads()
    {
        if(m_player->getIsOnJumpPad())
        {
            BR_INFO("%s", "Apply jumppad.");
            const glm::vec3 playerImpulse = BeryllConstants::worldUp * EnumsAndVars::jumpPadPower;
            const glm::vec3 garbageImpulse = playerImpulse * EnumsAndVars::playerMassToGarbageMassRatio;

            m_player->getObj()->applyCentralImpulse(playerImpulse);

            for(const auto& wrapper : m_allGarbage)
            {
                if(wrapper.isMagnetized)
                    wrapper.obj->applyCentralImpulse(garbageImpulse);
            }
        }
    }

    void BaseMap::handlePlayerDie()
    {
        if(m_player->getIsDie())
        {
            SendStatisticsHelper::sendPlayerDie();

            if(EnumsAndVars::playerResurrectionAttempts > 0)
            {
                m_gui->showMenuResurrect();
            }
            else // Lose menu.
            {
                m_gui->showMenuLose();

                SendStatisticsHelper::sendMapLose();
            }
        }
    }

    void BaseMap::handlePlayerWin()
    {
        if(EnumsAndVars::mapPlayerWin)
        {
            SendStatisticsHelper::sendMapWin();

            if(EnumsAndVars::MapsProgress::lastOpenedMapIndex == EnumsAndVars::MapsProgress::currentMapIndex)
            {
                ++EnumsAndVars::MapsProgress::lastOpenedMapIndex;

                BR_INFO("lastOpenedMapIndex: %d", EnumsAndVars::MapsProgress::lastOpenedMapIndex);
                DataBaseHelper::storeMapsProgressLastOpenedMapIndex(EnumsAndVars::MapsProgress::lastOpenedMapIndex);

                if(EnumsAndVars::MapsProgress::currentMapIndex < EnumsAndVars::MapsProgress::maxMapIndex)
                {
                    ++EnumsAndVars::MapsProgress::currentMapIndex;
                    BR_INFO("currentMapIndex: %d", EnumsAndVars::MapsProgress::currentMapIndex);
                    DataBaseHelper::storeMapsProgressCurrentMapIndex(EnumsAndVars::MapsProgress::currentMapIndex);
                }
            }

            m_gui->showMenuWin();
        }
    }

    void BaseMap::respawnEnemiesAtNewDistance(float minDistance)
    {
        BR_ASSERT((minDistance > 100.0f), "%s", "Wrong distance for respawn enemies.");

        std::vector<glm::ivec2> newPositions;
        glm::vec2 playerPosXZ{m_player->getObj()->getOrigin().x, m_player->getObj()->getOrigin().z};
        float distanceToCurrent = 0.0f;

        for(const glm::ivec2& point : m_pathAllowedPositionsXZ)
        {
            distanceToCurrent = glm::distance(playerPosXZ, glm::vec2(float(point.x), float(point.y)));
            if(distanceToCurrent > minDistance)
            {
                // We can move enemy to this point.
                newPositions.push_back(point);
            }
        }

        if(newPositions.empty())
            return;

        for(const auto &enemy: m_allAnimatedEnemies)
        {
            if(enemy->getIsEnabledUpdate() && enemy->getIsCanMove())
            {
                const glm::ivec2 spawnPoint2D = newPositions[Beryll::RandomGenerator::getInt(newPositions.size() - 1)];

                enemy->setPathArray(m_pathFinderEnemies.findPath(spawnPoint2D, m_playerClosestAllowedPos, 5), 1);

                enemy->setOrigin(enemy->getStartPointMoveFrom());
            }
        }
    }

    void BaseMap::spawnCommonGarbage()
    {
        if(!m_pointsToSpawnCommonGarbage.empty() &&
           EnumsAndVars::garbageCommonSpawnTime + EnumsAndVars::garbageCommonSpawnDelay < EnumsAndVars::mapPlayTimeSec)
        {
            EnumsAndVars::garbageCommonSpawnTime = EnumsAndVars::mapPlayTimeSec;

            const glm::ivec2 spawnPoint2D = m_pointsToSpawnCommonGarbage[Beryll::RandomGenerator::getInt(m_pointsToSpawnCommonGarbage.size() - 1)];
            const glm::vec3 spawnPoint3D{spawnPoint2D.x, 7.0f, spawnPoint2D.y};

            spawnGarbage(EnumsAndVars::garbageCommonSpawnCount, GarbageType::COMMON, spawnPoint3D);
            //BR_INFO("Garbage::getCommonActiveCount() %d", Garbage::getCommonActiveCount());
        }
    }

    void BaseMap::checkPlayerOutOfMap()
    {
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
            m_player->getObj()->applyCentralImpulse(glm::normalize(-playerOrig) * 140.0f);
        }
    }

    void BaseMap::lastWaveToBossPhase()
    {
        BR_INFO("%s", "lastWaveToBossPhase()");

        m_gui->showMenuKillAllToSpawnBoss();

        EnumsAndVars::enemiesLastWavePhase = true;
        EnumsAndVars::mapHasBossPhase = true;
        EnumsAndVars::bossPhase = false;
    }

    void BaseMap::lastWaveToWinPhase()
    {
        BR_INFO("%s", "lastWaveToWinPhase()");

        m_gui->showMenuKillAllToWin();

        EnumsAndVars::enemiesLastWavePhase = true;
        EnumsAndVars::mapHasBossPhase = false;
        EnumsAndVars::bossPhase = false;
    }
}
