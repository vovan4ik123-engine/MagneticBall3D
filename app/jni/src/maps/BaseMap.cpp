#include "BaseMap.h"
#include "DataBaseHelper.h"
#include "Sounds.h"

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
        if(m_gui->resurrectPlayer)
        {
            m_gui->resurrectPlayer = false;

            --EnAndVars::playerResurrectionAttempts;
            EnAndVars::CurrencyBalance::crystals -= EnAndVars::playerCostOfResurrectionCrystals;
            DataBaseHelper::storeCurrencyBalanceCrystals(EnAndVars::CurrencyBalance::crystals);

            SendStatisticsHelper::canSendPlayerDie();

            m_player->resurrect();

            BR_INFO("%s", "m_player->resurrect().");

            float mapXLength = glm::distance(m_minX, m_maxX);
            float mapZLength = glm::distance(m_minZ, m_maxZ);
            if(mapXLength > mapZLength)
                respawnEnemiesAtNewDistance(mapXLength * 0.4f, mapXLength * 0.8f);
            else
                respawnEnemiesAtNewDistance(mapZLength * 0.4f, mapZLength * 0.8f);
        }

        if(EnAndVars::gameOnPause)
            return;

        m_improvements.update();

        if(EnAndVars::improvementSystemOnScreen)
            return;

        EnAndVars::mapPlayTimeSec += Beryll::TimeStep::getTimeStepSec();

        Sounds::update();

        handlePlayerDie();
        handlePlayerWin();

        handleScreenSwipe();
        updatePathfindingAndSpawnEnemies();
        spawnCommonGarbage();
        handleJumpPads();
        checkPlayerOutOfMap();
    }

    void BaseMap::updateAfterPhysics()
    {
        if(EnAndVars::gameOnPause || EnAndVars::improvementSystemOnScreen)
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
        updateEnemiesAndTheirsAttacks();
        updateAndMagnetizeGarbage();
        killEnemies();

        if(EnAndVars::prepareToBossPhase && BaseEnemy::getActiveCount() <= 0)
            startBossPhase();
        else if(EnAndVars::bossPhase)
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

        m_shadowMap = Beryll::Renderer::createShadowMap(3200, 3200);
    }

    void BaseMap::loadPlayer()
    {
        auto playerAllBalls = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/player/Player6Models.fbx",
                                                                                       EnAndVars::playerMass,
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
            item->setDamping(EnAndVars::playerLinearDamping, EnAndVars::playerAngularDamping);

            m_animatedOrDynamicObjects.push_back(item);
            m_simpleObjForShadowMap.push_back(item);
        }

        m_player = std::make_shared<Player>(playerAllBalls[0], EnAndVars::playerStartHP);
        m_player->setAllModels(playerAllBalls);

        m_player->getObj()->setGravity(EnAndVars::playerGravityOnGround);
        m_player->getObj()->setFriction(EnAndVars::playerFriction);
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

            if(glm::distance(m_fingerUpPos, m_fingerDownPos) < 10.0f)
                return;

            ++EnAndVars::mapSwipeCount;

            glm::vec2 screenSwipe = (m_fingerUpPos - m_fingerDownPos);
            m_screenSwipe3D = glm::vec3{-screenSwipe.y, 0.0f, screenSwipe.x};
            float screenSwipeLength = glm::length(m_screenSwipe3D);
            if(screenSwipeLength > 500.0f)
            {
                screenSwipeLength = 500.0f;
                m_screenSwipe3D = glm::normalize(m_screenSwipe3D) * screenSwipeLength;
            }

            glm::qua cameraRotFromStartDir = glm::normalize(glm::rotation(m_startDir, Beryll::Camera::getCameraFrontDirectionXZ()));
            m_screenSwipe3D = glm::normalize(cameraRotFromStartDir * glm::normalize(m_screenSwipe3D));
            m_screenSwipe3D *= (screenSwipeLength * EnAndVars::swipePowerMultiplier);

            glm::vec3 powerForImpulse{0.0f};
            glm::vec3 powerForTorque{0.0f};
            float swipeFactorBasedOnAngleAndSpeed = 0.0f;
            if(m_player->getMoveSpeed() > 2.0f)
            {
                if(!BeryllUtils::Common::getIsVectorsParallelInSameDir(m_player->getMoveDir(), glm::normalize(m_screenSwipe3D)))
                {
                    float moveToSwipeAngle = BeryllUtils::Common::getAngleInRadians(m_player->getMoveDir(), glm::normalize(m_screenSwipe3D));
                    swipeFactorBasedOnAngleAndSpeed = moveToSwipeAngle * m_player->getMoveSpeed() * EnAndVars::playerLeftRightTurnPower;

                    // For turn back.
                    if(moveToSwipeAngle > 2.6f) // > 150 degrees.
                    {
                        swipeFactorBasedOnAngleAndSpeed *= 1.3f;
                        // With bigger impulse the turn back become too sharper. Reduce it with impulse grown.
                        swipeFactorBasedOnAngleAndSpeed *= EnAndVars::playerImpulseFactorOnGroundDefault / EnAndVars::playerImpulseFactorOnGround;
                    }
                }
            }

            // 2 m = radius for default ball.
            const float radiusForTorqueMultiplier = std::max(1.0f, m_player->getObj()->getXZRadius() * 0.5f);

            if(m_player->getIsOnGround())
            {
                powerForImpulse = m_screenSwipe3D * EnAndVars::playerImpulseFactorOnGround;
                powerForImpulse += powerForImpulse * swipeFactorBasedOnAngleAndSpeed;
                powerForTorque = m_screenSwipe3D * EnAndVars::playerTorqueFactorOnGround;
                powerForTorque += powerForTorque * swipeFactorBasedOnAngleAndSpeed;

                powerForTorque *= radiusForTorqueMultiplier;

                if(m_player->getMoveSpeed() < EnAndVars::playerMaxSpeedXZDefault)
                {
                    // Help to player move faster on ground when speed is low.
                    const float powerToHelpPlayer = (EnAndVars::playerMaxSpeedXZDefault - m_player->getMoveSpeed()) * 0.011f;
                    powerForImpulse += powerForImpulse * powerToHelpPlayer;
                    powerForTorque += powerForTorque * powerToHelpPlayer;
                }
            }
            else if(m_player->getLastTimeOnBuilding() + 1.0f > EnAndVars::mapPlayTimeSec)
            {
                // buildingNormalAngle = glm::half_pi<float>() if player on vertical wall. angleFactor will = 1.
                // buildingNormalAngle = 0 if player on horizontal roof. angleFactor will = 0.
                const float angleFactor = m_player->getBuildingNormalAngle() / glm::half_pi<float>();

                const float impulseDiff = EnAndVars::playerImpulseFactorOnBuildingWall - EnAndVars::playerImpulseFactorOnBuildingRoof;
                const float torqueDiff = EnAndVars::playerTorqueFactorOnBuildingWall - EnAndVars::playerTorqueFactorOnBuildingRoof;

                powerForImpulse = m_screenSwipe3D * (EnAndVars::playerImpulseFactorOnBuildingRoof + impulseDiff * angleFactor);
                powerForTorque = m_screenSwipe3D * (EnAndVars::playerTorqueFactorOnBuildingRoof + torqueDiff * angleFactor);

                //BR_INFO("torq %f", (EnAndVars::playerTorqueFactorOnBuildingRoof + torqueDiff * angleFactor));

                powerForTorque *= radiusForTorqueMultiplier;

                if(m_player->getBuildingNormalAngle() < 0.1745f) // Less than 10 degrees. Assume we are on flat roof.
                {
                    powerForImpulse += powerForImpulse * swipeFactorBasedOnAngleAndSpeed;
                    powerForTorque += powerForTorque * swipeFactorBasedOnAngleAndSpeed;
                }
            }
            else // if(m_player->getIsOnAir())
            {
                powerForImpulse = m_screenSwipe3D * EnAndVars::playerImpulseFactorOnAir;
                powerForTorque = m_screenSwipe3D * EnAndVars::playerTorqueFactorOnAir;
            }

            // If max allowed speed exceeded not all impulse power will applied.
            // In this case applyImpulseFactor shows how much was applied in range 0...1.
            float applyImpulseFactor = m_player->handleScreenSwipe(powerForImpulse, powerForTorque);

            if(m_player->getIsOnGround() ||
               (m_player->getLastTimeOnBuilding() + 1.0f > EnAndVars::mapPlayTimeSec && m_player->getBuildingNormalAngle() < 0.1745f)) // Less than 10 degrees. Assume we are on flat roof.
            {
                // Same as inside m_player->handleScreenSwipe(impulse, torque).
                glm::vec3 garbageImpulse = (powerForImpulse * 0.000065f) * applyImpulseFactor;
                if(applyImpulseFactor == 1.0f)
                    garbageImpulse += powerForTorque * 0.000021f;

                for(const auto& wrapper : m_allGarbage)
                {
                    if(wrapper.isMagnetized)
                        wrapper.obj->applyCentralImpulse(garbageImpulse);
                }
            }
            else if(m_player->getLastTimeOnBuilding() + 1.0f > EnAndVars::mapPlayTimeSec)
            {
                const float helpOnBuilding = 60.0f * (m_player->getBuildingNormalAngle() / glm::half_pi<float>());
                //BR_INFO("helpOnBuilding %f", helpOnBuilding);
                const glm::vec3 playerImpulse = BeryllConstants::worldUp * helpOnBuilding;
                const glm::vec3 garbageImpulse = playerImpulse * EnAndVars::playerMassToGarbageMassRatio * 2.5f;

                m_player->getObj()->applyCentralImpulse(playerImpulse);

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
        EnAndVars::garbageCountMagnetized = 0;

        // Check if magnetized garbage still in playerMagneticRadius.
        for(auto& wrapper : m_allGarbage)
        {
            if(wrapper.getIsEnabled())
                wrapper.update();

            if(wrapper.getIsEnabled() && wrapper.isMagnetized && wrapper.getCanBeMagnetized() &&
               glm::distance(m_player->getObj()->getOrigin(), wrapper.obj->getOrigin()) < EnAndVars::playerMagneticRadius)
            {
                ++EnAndVars::garbageCountMagnetized;
            }
            else
            {
                wrapper.isMagnetized = false;
            }
        }

        // Update gravity. And check for more garbage if we have limit for that.
        float gravPower = EnAndVars::garbageMinGravityPower + (m_player->getMoveSpeed() * EnAndVars::garbageGravityIncreasedByPlayerSpeed);
        if(gravPower > EnAndVars::garbageMaxGravityPower)
            gravPower = EnAndVars::garbageMaxGravityPower;

        float speedToResetVelocity = m_player->getMoveSpeed() * 1.5f;

        for(auto& wrapper : m_allGarbage)
        {
            // 1. And update gravity.
            if(wrapper.isMagnetized)
            {
                glm::vec3 gravDir = glm::normalize(m_player->getObj()->getOrigin() - wrapper.obj->getOrigin());
                wrapper.obj->setGravity(gravDir * gravPower, false, false);

                // Stop garbage if it stats rotating around player too fast.
                const glm::vec3 linVelocity = wrapper.obj->getLinearVelocity();
                const glm::vec3 objMoveDir = glm::normalize(linVelocity);
                const float objSpeed = glm::length(linVelocity);
                const glm::vec3 objToPlayerDir = glm::normalize(m_player->getObj()->getOrigin() - wrapper.obj->getOrigin());

                if(objSpeed > speedToResetVelocity && BeryllUtils::Common::getAngleInRadians(objToPlayerDir, objMoveDir) > 0.35f) // > 20 degrees.
                {
                    wrapper.obj->setLinearVelocity(objToPlayerDir * 4.0f);
                }
            }
            else if(wrapper.getIsEnabled())
            {
                wrapper.obj->setGravity(EnAndVars::garbageGravityDefault, false, false);
            }

            // 2. Check for more garbage if we have limit for that.
            if(EnAndVars::garbageCountMagnetized < EnAndVars::garbageMaxCountMagnetized &&
               wrapper.getIsEnabled() && !wrapper.isMagnetized && wrapper.getCanBeMagnetized() &&
               glm::distance(m_player->getObj()->getOrigin(), wrapper.obj->getOrigin()) < EnAndVars::playerMagneticRadius)
            {
                ++EnAndVars::garbageCountMagnetized;
                wrapper.isMagnetized = true;
                wrapper.obj->activate();
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
                if(distanceToCurrent > EnAndVars::enemiesMinDistanceToSpawn && distanceToCurrent < EnAndVars::enemiesMaxDistanceToSpawn)
                {
                    // We can spawn enemy at this point.
                    m_pointsToSpawnEnemies.push_back(point);
                }

                if(distanceToCurrent > EnAndVars::garbageCommonSpawnMinDistance && distanceToCurrent < EnAndVars::garbageCommonSpawnMaxDistance)
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
            m_pathFinder.clearBlockedPositions();

            // 2.
            spawnEnemies();
        }
        // In third frame:
        // 1. Update paths for enemies.
        else if(m_pathFindingIteration == 2)
        {
            int enemiesUpdated = 0;
            int& i = EnAndVars::enemiesCurrentPathfindingIndex;
            for( ; i < m_allAnimatedEnemies.size(); ++i)
            {
                if(enemiesUpdated >= EnAndVars::enemiesMaxPathfindingInOneFrame)
                    break;

                if(m_allAnimatedEnemies[i]->getIsEnabledUpdate() && m_allAnimatedEnemies[i]->getIsCanMove())
                {
                    m_allAnimatedEnemies[i]->setPathArray(m_pathFinder.findPath(m_allAnimatedEnemies[i]->getCurrentPointToMove2DInt(), m_playerClosestAllowedPos, 7), 0);

                    m_pathFinder.addBlockedPosition(m_allAnimatedEnemies[i]->getCurrentPointToMove2DInt());

                    ++enemiesUpdated;
                }
            }

            if(EnAndVars::enemiesCurrentPathfindingIndex >= m_allAnimatedEnemies.size())
            {
                // All enemies were updated.
                //BR_INFO("Path for all enemies updated. Last index: %d", EnumsAndVariables::enemiesCurrentPathfindingIndex);
                EnAndVars::enemiesCurrentPathfindingIndex = 0;
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
                    // Play shot sounds.
                    if(enemy->unitType == UnitType::GUN || enemy->unitType == UnitType::GUN_SHIELD)
                        Sounds::playSoundEffect(SoundType::PISTOL_SHOT);
                    else if(enemy->unitType == UnitType::SNIPER)
                        Sounds::playSoundEffect(SoundType::RIFLE_SHOT);
                    else if(enemy->unitType == UnitType::GRENADE_LAUNCHER)
                        Sounds::playSoundEffect(SoundType::GRENADE_LAUNCHER_SHOT);
                    else if(enemy->unitType == UnitType::TANK)
                        Sounds::playSoundEffect(SoundType::TANK_SHOT);

                    // Spam particles.
                    glm::vec3 from = enemy->getOrigin(); // Calculate particles start point.

                    if(enemy->unitType == UnitType::GUN ||
                       enemy->unitType == UnitType::GUN_SHIELD ||
                       enemy->unitType == UnitType::SNIPER)
                    {
                        from.y += enemy->getFromOriginToTop() * 0.8f;
                        from += enemy->getFaceDirXZ() * 14.0f;
                    }
                    else if(enemy->unitType == UnitType::GRENADE_LAUNCHER)
                    {
                        from.y += 1.0f;
                        from += enemy->getFaceDirXZ() * 14.0f;
                    }
                    else if(enemy->unitType == UnitType::TANK)
                    {
                        from.y += enemy->getFromOriginToTop() * 0.8f;
                        from += enemy->getFaceDirXZ() * 30.0f;
                    }

                    if(enemy->unitType == UnitType::GUN ||
                       enemy->unitType == UnitType::GUN_SHIELD)
                    {
                        emitParticlesLine(from, rayAttack.hitPoint, 0.2f, 0.2f,
                                          glm::vec4(0.9f, 0.9f, 0.0f, 1.0f), glm::vec4(0.9f, 0.9f, 0.0f, 0.7f), 0.4f);
                    }
                    else if(enemy->unitType == UnitType::GRENADE_LAUNCHER)
                    {
                        emitParticlesLine(from, rayAttack.hitPoint, 0.5f, 0.5f,
                                          glm::vec4(1.0f, 0.5f, 0.0f, 1.0f), glm::vec4(1.0f, 0.5f, 0.0f, 0.7f), 0.4f);

                        emitParticlesExplosion(rayAttack.hitPoint, 6, 1.5f, 1.5f,
                                               glm::vec4(1.0f, 0.5f, 0.0f, 1.0f), glm::vec4(1.0f, 0.5f, 0.0f, 0.7f), 0.8f);
                    }
                    else if(enemy->unitType == UnitType::SNIPER)
                    {
                        emitParticlesLine(from, rayAttack.hitPoint, 0.2f, 0.2f,
                                          glm::vec4(0.95f, 0.05f, 0.0f, 1.0f), glm::vec4(0.95f, 0.05f, 0.0f, 0.7f), 0.4f);
                    }
                    else if(enemy->unitType == UnitType::TANK)
                    {
                        emitParticlesLine(from, rayAttack.hitPoint, 1.0f, 1.0f,
                                          glm::vec4(1.0f, 0.5f, 0.0f, 1.0f), glm::vec4(1.0f, 0.5f, 0.0f, 0.7f), 0.6f);

                        emitParticlesExplosion(rayAttack.hitPoint, 6, 3.0f, 3.0f,
                                               glm::vec4(1.0f, 0.5f, 0.0f, 1.0f), glm::vec4(1.0f, 0.5f, 0.0f, 0.7f), 1.2f);
                    }

                    enemy->attack(m_player->getObj()->getOrigin());

                    if(rayAttack.hittedCollGroup == Beryll::CollisionGroups::PLAYER)
                    {
                        // Player attacked.
                        //BR_INFO("%s", "Player attacked");
                        m_player->takeDamage(enemy->damage);

                    }
                    else if(rayAttack.hittedCollGroup == Beryll::CollisionGroups::GARBAGE)
                    {
                        // Garbage under attack =).
                        if(enemy->attackType == AttackType::RANGE_DAMAGE_ONE)
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
                        else if(enemy->attackType == AttackType::STEAL_GARBAGE)
                        {
                            //BR_INFO("%s", "Garbage under attack =) by AttackType::STEAL_GARBAGE");
                            for(auto& wrapper : m_allGarbage)
                            {
                                if(rayAttack.hittedObjectID == wrapper.obj->getID())
                                {
                                    wrapper.pauseMagnetization(1.0f);
                                    wrapper.obj->setGravity(EnAndVars::garbageGravityDefault, false, false);
                                    wrapper.obj->applyCentralImpulse(glm::normalize(enemy->getOrigin() - wrapper.obj->getOrigin()) * 0.1f);
                                    break;
                                }
                            }
                        }
                    }

                    // Play hit sounds.
                    if(enemy->unitType == UnitType::GUN ||
                       enemy->unitType == UnitType::GUN_SHIELD ||
                       enemy->unitType == UnitType::SNIPER)
                        Sounds::playSoundEffect(SoundType::PISTOL_HIT);
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
        float radiusToKill = std::max(8.0f, m_player->getObj()->getXZRadius() * 1.4f) + EnAndVars::garbageCountMagnetized * 0.11f;

        if(m_player->getIsTouchGroundAfterFall() && m_player->getFallDistance() > 90.0f)
        {
            radiusToKill = EnAndVars::playerDamageGroundRadiusAfterFall;
            //BR_INFO("Damage radius after fall on ground: %f", radiusToKill);
        }

        float speedToReduce = 0.0f;
        int addToExp = 0;
        for(const auto& enemy : m_allAnimatedEnemies)
        {
            if(enemy->getIsEnabledUpdate() &&
               enemy->garbageAmountToDie < EnAndVars::garbageCountMagnetized &&
               glm::distance(enemy->getOrigin(), m_player->getObj()->getOrigin()) < radiusToKill)
            {
                enemy->disableEnemy();
                speedToReduce += enemy->reducePlayerSpeedWhenDie;
                addToExp += enemy->experienceWhenDie;
                ++EnAndVars::enemiesKilledCount;
                Sounds::playSoundEffect(SoundType::POP);

                if(enemy->unitType == UnitType::GUN)
                {
                    spawnGarbage(1, GarbageType::COP_WITH_PISTOL, enemy->getOrigin());
                }
                else if(enemy->unitType == UnitType::GUN_SHIELD)
                {
                    spawnGarbage(1, GarbageType::COP_WITH_SHIELD, enemy->getOrigin());
                }
                else if(enemy->unitType == UnitType::GRENADE_LAUNCHER)
                {
                    spawnGarbage(1, GarbageType::COP_WITH_GRENADE_LAUNCHER, enemy->getOrigin());
                }
                else if(enemy->unitType == UnitType::SNIPER)
                {
                    spawnGarbage(1, GarbageType::SNIPER, enemy->getOrigin());
                    enemy->freeStaticPosition();
                }
                else if(enemy->unitType == UnitType::TANK)
                {
                    spawnGarbage(1, GarbageType::TANK, enemy->getOrigin());
                }

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

        if(m_player->getLastTimeOnBuilding() + 1.0f > EnAndVars::mapPlayTimeSec &&
           m_player->getBuildingNormalAngle() > 1.22f) // > 70 degrees. Assume player on vertical surface.
        {
            if(glm::length(m_screenSwipe3D) > 180.0f * EnAndVars::swipePowerMultiplier)
                desiredCameraBackXZ = -glm::normalize(glm::vec3(m_screenSwipe3D.x, 0.0f, m_screenSwipe3D.z));
        }
        else if(m_player->getMoveSpeedXZ() > EnAndVars::minPlayerSpeedToCameraFollow)
        {
            desiredCameraBackXZ = -m_player->getMoveDirXZ();
        }

        BR_ASSERT((!glm::any(glm::isnan(cameraBackXZ)) && !glm::any(glm::isnan(desiredCameraBackXZ))), "%s", "Camera back dir in nan.");

        //if(!glm::any(glm::isnan(cameraBackXZ)) && !glm::any(glm::isnan(desiredCameraBackXZ)))
        {
            const glm::quat rotation = glm::rotation(cameraBackXZ, desiredCameraBackXZ);

            const float angleDifference = glm::angle(rotation);
            if(angleDifference > 0.036f) // More than 2 degrees.
            {
                float angleRotate = angleDifference * 0.05f + 0.025f; // Default and fastest camera rotation speed.
                float currentFPS = Beryll::GameLoop::getFPS();

                if(currentFPS > EnAndVars::minFPSForCameraRotation)
                {
                    // Reduce camera rotation speed if FPS is high. Camera will more smooth.
                    const float currentFPSToMinFPSRatio = currentFPS / EnAndVars::minFPSForCameraRotation;
                    angleRotate = angleRotate / currentFPSToMinFPSRatio;
                }

                const glm::mat4 matr = glm::rotate(glm::mat4{1.0f}, angleRotate, glm::normalize(glm::axis(rotation)));

                m_cameraOffset = matr * glm::vec4(cameraBackXZ, 1.0f);
            }
        }

        m_cameraOffset.y = 0.0f;
        m_cameraOffset = glm::normalize(m_cameraOffset);
        m_cameraOffset.y = 0.09f +
                           (EnAndVars::garbageCountMagnetized * 0.0015f) +
                           (m_player->getObj()->getOrigin().y * 0.005f);
        m_cameraOffset.y = std::min(1.1f, m_cameraOffset.y);
        m_cameraOffset = glm::normalize(m_cameraOffset);

        m_cameraFront = m_player->getObj()->getOrigin();

        float maxCameraYOffset = m_startCameraYOffset +
                                 (EnAndVars::garbageCountMagnetized * 0.15f) +
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
                    m_cameraYOffset += EnAndVars::cameraUpOffsetMaxSpeed * std::min(0.04f, Beryll::TimeStep::getTimeStepSec());
                else
                    m_cameraYOffset -= EnAndVars::cameraUpOffsetMaxSpeed * std::min(0.04f, Beryll::TimeStep::getTimeStepSec());
            }
        }

        m_cameraFront.y += m_cameraYOffset;

        float maxCameraDistance = m_startCameraDistance +
                                  (EnAndVars::garbageCountMagnetized * 0.7f) +
                                  std::min(100.0f, m_player->getMoveSpeedXZ() * 0.8f) +
                                  std::min(100.0f, m_player->getObj()->getOrigin().y * 0.28f);

        glm::vec3 cameraPosForRay = m_cameraFront + m_cameraOffset * maxCameraDistance;

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
            float minCameraUpOffset = m_startCameraYOffset + (EnAndVars::garbageCountMagnetized * 0.2f);
            m_cameraYOffset = std::max(minCameraUpOffset, maxCameraYOffset * hitDistanceFactor);
        }
        else if(glm::distance(m_cameraDistance, maxCameraDistance) < 0.565f)
        {
            m_cameraDistance = maxCameraDistance;
        }
        else
        {
            if(m_cameraDistance < maxCameraDistance)
                m_cameraDistance += EnAndVars::cameraZoomMaxSpeed * std::min(0.04f, Beryll::TimeStep::getTimeStepSec());
            else
                m_cameraDistance -= EnAndVars::cameraZoomMaxSpeed * std::min(0.04f, Beryll::TimeStep::getTimeStepSec());
        }


        Beryll::Camera::setCameraPos(m_cameraFront + m_cameraOffset * m_cameraDistance);
        Beryll::Camera::setCameraFrontPos(m_cameraFront);
    }

    void BaseMap::spawnGarbage(const int count, const GarbageType type, glm::vec3 spawnPoint)
    {
        int spawnedCount = 0;
        for(auto& wrapper : m_allGarbage)
        {
            if(spawnedCount >= count ||
               (type == GarbageType::COMMON && Garbage::getCommonActiveCount() >= EnAndVars::garbageCommonMaxCountOnMap))
                break;

            if(!wrapper.getIsEnabled() && wrapper.getType() == type)
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
            const glm::vec3 playerImpulse = BeryllConstants::worldUp * EnAndVars::jumpPadPower;
            const glm::vec3 garbageImpulse = playerImpulse * EnAndVars::playerMassToGarbageMassRatio;

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

            if(EnAndVars::playerResurrectionAttempts > 0)
            {
                if(EnAndVars::CurrencyBalance::crystals >= EnAndVars::playerCostOfResurrectionCrystals)
                {
                    m_gui->showMenuResurrect();
                }
                else // Can resurrect but no crystals.
                {
                    m_gui->showMenuResurrectNoCrystals();
                }
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
        if(EnAndVars::mapPlayerWin)
        {
            SendStatisticsHelper::sendMapWin();

            if(EnAndVars::MapsProgress::lastOpenedMapIndex == EnAndVars::MapsProgress::currentMapIndex)
            {
                ++EnAndVars::MapsProgress::lastOpenedMapIndex;

                BR_INFO("lastOpenedMapIndex: %d", EnAndVars::MapsProgress::lastOpenedMapIndex);
                DataBaseHelper::storeMapsProgressLastOpenedMapIndex(EnAndVars::MapsProgress::lastOpenedMapIndex);

                if(EnAndVars::MapsProgress::currentMapIndex < EnAndVars::MapsProgress::maxMapIndex)
                {
                    ++EnAndVars::MapsProgress::currentMapIndex;
                    BR_INFO("currentMapIndex: %d", EnAndVars::MapsProgress::currentMapIndex);
                    DataBaseHelper::storeMapsProgressCurrentMapIndex(EnAndVars::MapsProgress::currentMapIndex);
                }
            }

            m_gui->showMenuWin();
        }
    }

    void BaseMap::respawnEnemiesAtNewDistance(float minDistance, float maxDistance)
    {
        BR_ASSERT((minDistance > 10.0f && maxDistance > 20.0f && maxDistance > minDistance), "%s", "Wrong distance for respawn enemies.");

        std::vector<glm::ivec2> newPositions;
        glm::vec2 playerPosXZ{m_player->getObj()->getOrigin().x, m_player->getObj()->getOrigin().z};
        float distanceToCurrent = 0.0f;

        for(const glm::ivec2& point : m_pathAllowedPositionsXZ)
        {
            distanceToCurrent = glm::distance(playerPosXZ, glm::vec2(float(point.x), float(point.y)));
            if(distanceToCurrent > minDistance && distanceToCurrent < maxDistance)
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

                enemy->setPathArray(m_pathFinder.findPath(spawnPoint2D, m_playerClosestAllowedPos, 5), 1);

                enemy->setOrigin(enemy->getStartPointMoveFrom());
            }
        }
    }

    void BaseMap::spawnCommonGarbage()
    {
        if(!m_pointsToSpawnCommonGarbage.empty() &&
           EnAndVars::garbageCommonSpawnTime + EnAndVars::garbageCommonSpawnDelay < EnAndVars::mapPlayTimeSec)
        {
            EnAndVars::garbageCommonSpawnTime = EnAndVars::mapPlayTimeSec;

            const glm::ivec2 spawnPoint2D = m_pointsToSpawnCommonGarbage[Beryll::RandomGenerator::getInt(m_pointsToSpawnCommonGarbage.size() - 1)];
            glm::vec3 spawnPoint3D{spawnPoint2D.x, 7.0f, spawnPoint2D.y};

            spawnGarbage(EnAndVars::garbageCommonSpawnCount, GarbageType::COMMON, spawnPoint3D);
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
            m_player->getObj()->applyCentralImpulse(glm::normalize(-playerOrig) * 100.0f);
        }
    }

    void BaseMap::prepareToBossPhase()
    {
        BR_INFO("%s", "prepareToBossPhase()");

        m_gui->showMenuKillAllBeforeBoss();
        EnAndVars::prepareToBossPhase = true;
        EnAndVars::bossPhase = false;
    }
}
