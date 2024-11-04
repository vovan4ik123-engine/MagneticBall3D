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
        m_objWithNormalMap.reserve(20);
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

            EnumsAndVars::playerResurrectTime = EnumsAndVars::mapPlayTimeSec;
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

        handleControls();
        if(EnumsAndVars::playerResurrectTime + 14.0f < EnumsAndVars::mapPlayTimeSec)
            updatePathfindingAndSpawnEnemies();
        spawnCommonGarbage();
        handleJumpPads();
        checkPlayerOutOfMap();
    }

    void BaseMap::updateAfterPhysics()
    {
        if(EnumsAndVars::gameOnPause || EnumsAndVars::improvementSystemOnScreen)
        {
            m_gui->playerJoystick->disable();
            return;
        }

        for(const std::shared_ptr<Beryll::SceneObject>& so : m_animatedOrDynamicObjects)
        {
            if(so->getIsEnabledUpdate())
            {
                so->updateAfterPhysics();

                if(so->getSceneObjectGroup() == Beryll::SceneObjectGroups::ENEMY ||
                   so->getSceneObjectGroup() == Beryll::SceneObjectGroups::GARBAGE)
                {
                    if(Beryll::Camera::getIsSeeObject(so->getOrigin(), 0.98f))
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
        if(EnumsAndVars::playerResurrectTime + 14.0f < EnumsAndVars::mapPlayTimeSec)
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
            item->setDamping(EnumsAndVars::playerDamping, EnumsAndVars::playerDamping);

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

    void BaseMap::handleControls()
    {
        if(m_gui->playerJoystick->getIsTouched())
        {
            const glm::vec2 joyDir2D = m_gui->playerJoystick->getDirection();

            if(glm::any(glm::isnan(joyDir2D)) || glm::length(joyDir2D) < 0.001f)
                return;

            // Default face direction is {1.0f, 0.0f, 0.0f}.
            // needRotationToCamera = rotation between camera and default face dir.
            const glm::quat fromDefaultDirToCamera = glm::rotation(glm::vec3{1.0f, 0.0f, 0.0f}, Beryll::Camera::getCameraFrontDirectionXZ());
            // Move on XZ plane.
            m_joystickDir3D = fromDefaultDirToCamera * glm::vec4(joyDir2D.y, 0.0f, joyDir2D.x, 1.0f);
            m_joystickDir3D = glm::normalize(m_joystickDir3D) * (EnumsAndVars::joystickPowerInOneSec * Beryll::TimeStep::getTimeStepSec());
        }
        else
        {
            m_gui->playerJoystick->disable();

            std::vector<Beryll::Finger>& fingers = Beryll::EventHandler::getFingers();
            for(Beryll::Finger& f : fingers)
            {
                if(f.handled)
                    continue;

                if(f.normalizedPos.x < 0.3f && f.normalizedPos.y > 0.3f)
                {
                    if(f.downEvent)
                    {
                        m_joystickEnabledTime = EnumsAndVars::mapPlayTimeSec;
                        m_gui->playerJoystick->enable();
                        m_gui->playerJoystick->setOrigin(f.normalizedPos);
                        f.handled = true;
                        break;
                    }
                }
            }

            return;
        }

        m_moveDirToJoystickDirAngle = 0.0f;
        float moveFactorBasedOnAngleAndSpeed = 0.0f;
        if(m_player->getMoveSpeedXZ() > 1.0f &&
           !BeryllUtils::Common::getIsVectorsParallelInSameDir(m_player->getMoveDirXZ(), glm::normalize(m_joystickDir3D)))
        {
            m_moveDirToJoystickDirAngle = BeryllUtils::Common::getAngleInRadians(m_player->getMoveDirXZ(), glm::normalize(m_joystickDir3D));
            moveFactorBasedOnAngleAndSpeed = m_moveDirToJoystickDirAngle * std::min(90.0f, m_player->getMoveSpeedXZ()) * 0.06f;

            if(m_joystickEnabledTime + 0.4f < EnumsAndVars::mapPlayTimeSec &&
               (m_player->getIsOnGround() ||
               (m_player->getLastTimeOnBuilding() + 0.1f > EnumsAndVars::mapPlayTimeSec && m_player->getBuildingNormalAngle() < 0.1745f)))
            {
                //BR_INFO("m_moveDirToJoystickDirAngle %f calculate damping", m_moveDirToJoystickDirAngle);
                float newDamping = glm::mix(0.0f, 1.0f, m_moveDirToJoystickDirAngle / glm::pi<float>()) * 1.9f;
                newDamping = std::min(0.9f, newDamping);
                m_player->getObj()->setDamping(newDamping, newDamping);
                // Increase control power if damping increased.
                moveFactorBasedOnAngleAndSpeed *= (1.0f + newDamping);
            }
        }

        glm::vec3 powerForImpulse{0.0f};
        glm::vec3 powerForTorque{0.0f};
        const float radiusForTorqueMultiplier = std::max(1.0f, m_player->getObj()->getXZRadius() * 0.5f); // 2 m = radius for default ball.

        if(m_player->getIsOnGround())
        {
            powerForImpulse = m_joystickDir3D * EnumsAndVars::playerImpulseFactorOnGround;
            powerForImpulse += powerForImpulse * moveFactorBasedOnAngleAndSpeed;
            powerForTorque = m_joystickDir3D * EnumsAndVars::playerTorqueFactorOnGround;
            powerForTorque += powerForTorque * moveFactorBasedOnAngleAndSpeed;

            powerForTorque *= radiusForTorqueMultiplier;
        }
        else if(m_player->getLastTimeOnBuilding() + 0.1f > EnumsAndVars::mapPlayTimeSec)
        {
            // buildingNormalAngle = glm::half_pi<float>() if player on vertical wall. angleFactor will = 1.
            // buildingNormalAngle = 0 if player on horizontal roof. angleFactor will = 0.
            const float angleFactor = m_player->getBuildingNormalAngle() / glm::half_pi<float>();

            const float impulseDiff = EnumsAndVars::playerImpulseFactorOnBuildingWall - EnumsAndVars::playerImpulseFactorOnBuildingRoof;
            const float torqueDiff = EnumsAndVars::playerTorqueFactorOnBuildingWall - EnumsAndVars::playerTorqueFactorOnBuildingRoof;

            powerForImpulse = m_joystickDir3D * (EnumsAndVars::playerImpulseFactorOnBuildingRoof + impulseDiff * angleFactor);
            powerForTorque = m_joystickDir3D * (EnumsAndVars::playerTorqueFactorOnBuildingRoof + torqueDiff * angleFactor);

            powerForTorque *= radiusForTorqueMultiplier;

            powerForImpulse += powerForImpulse * (moveFactorBasedOnAngleAndSpeed * (1.0f - angleFactor));
            powerForTorque += powerForTorque * (moveFactorBasedOnAngleAndSpeed * (1.0f - angleFactor));
        }
        else // if(m_player->getIsOnAir())
        {
            powerForImpulse = m_joystickDir3D * EnumsAndVars::playerImpulseFactorOnAir;
            powerForTorque = m_joystickDir3D * EnumsAndVars::playerTorqueFactorOnAir;
        }

        // If max allowed speed exceeded not all impulse power will applied.
        // In this case applyImpulseFactor shows how much was applied in range 0...1.
        float applyImpulseFactor = m_player->applyPowers(powerForImpulse, powerForTorque);

        // Also apply powers for magnetized garbage.
        glm::vec3 garbageImpulse{0.0f};

        if(m_player->getIsOnGround())
        {
            // Same as inside m_player->applyPowers(impulse, torque).
            garbageImpulse = (powerForImpulse * 0.00006f) * applyImpulseFactor;
            const float addSpeedAndAngle = (std::min(70.0f, m_player->getMoveSpeedXZ()) / 70.0f) * std::min(1.3f, m_moveDirToJoystickDirAngle);
            garbageImpulse *= std::max(1.0f, addSpeedAndAngle);
            if(applyImpulseFactor == 1.0f)
                garbageImpulse += powerForTorque * 0.00002f;
        }
        else if(m_player->getLastTimeOnBuilding() + 0.1f > EnumsAndVars::mapPlayTimeSec)
        {
            // Same as inside m_player->applyPowers(impulse, torque).
            glm::vec3 garbageImpulseOnRoof = (powerForImpulse * 0.00006f) * applyImpulseFactor;
            const float addSpeedAndAngle = (std::min(70.0f, m_player->getMoveSpeedXZ()) / 70.0f) * std::min(1.3f, m_moveDirToJoystickDirAngle);
            garbageImpulseOnRoof *= std::max(1.0f, addSpeedAndAngle);
            if(applyImpulseFactor == 1.0f)
                garbageImpulseOnRoof += powerForTorque * 0.00001f;

            const glm::vec3 garbageImpulseOnWall = BeryllConstants::worldUp * Beryll::TimeStep::getTimeStepSec() * 0.02f;

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
        float gravPower = EnumsAndVars::garbageMinGravityPower;
        if(m_player->getIsOnAir())
            gravPower *= 4.0f;
        gravPower += m_player->getMoveSpeed() * EnumsAndVars::garbageGravityIncreasedByPlayerSpeed;

        float speedToResetVelocity = m_player->getMoveSpeed() * glm::mix(1.4f, 1.2f, std::min(1.0f, m_moveDirToJoystickDirAngle * 0.9f));
        if(m_player->getIsOnAir())
        {
            if(m_player->getMoveSpeed() < 15.0f)
                speedToResetVelocity = m_player->getMoveSpeed() * 1.2f;
            else
                speedToResetVelocity = m_player->getMoveSpeed() * 1.5f;
        }
        else if(m_player->getLastTimeOnBuilding() + 0.1f > EnumsAndVars::mapPlayTimeSec &&
                m_player->getBuildingNormalAngle() > 1.3f && m_player->getBuildingNormalAngle() < 1.83f) // On vertical wall.
        {
            speedToResetVelocity = m_player->getMoveSpeed() * 1.4f;
        }

        for(auto& wrapper : m_allGarbage)
        {
            // 1. Update gravity.
            if(wrapper.isMagnetized)
            {
                glm::vec3 gravDir = glm::normalize(m_player->getObj()->getOrigin() - wrapper.obj->getOrigin());
                wrapper.obj->setGravity(gravDir * gravPower, false, false);

                if(m_player->getIsNeedCheckGarbageVelocity() && wrapper.getCanVelocityBeReseted())
                {
                    // Stop garbage if it stats rotating around player too fast.
                    const glm::vec3 linVelocity = wrapper.obj->getLinearVelocity();
                    const glm::vec3 objMoveDir = glm::normalize(linVelocity);
                    const float objSpeed = glm::length(linVelocity);
                    glm::vec3 objToPlayerDir = glm::normalize(m_player->getObj()->getOrigin() - wrapper.obj->getOrigin());

                    if(objSpeed > speedToResetVelocity && BeryllUtils::Common::getAngleInRadians(objToPlayerDir, objMoveDir) > 0.35f) // > 20 degrees.
                    {
                        if(m_player->getIsOnGround())
                            objToPlayerDir.y += 0.1f;

                        wrapper.obj->setLinearVelocity(objToPlayerDir * std::max(15.0f, m_player->getMoveSpeedXZ() * 0.7f));
                    }
                }
            }
            else if(wrapper.getIsEnabled())
            {
                wrapper.obj->setGravity(EnumsAndVars::garbageGravityDefault, false, false);
            }

            // 2. Magnetize more garbage if we have limit for that.
            if(EnumsAndVars::garbageCountMagnetized < EnumsAndVars::garbageMaxCountMagnetized &&
               wrapper.getIsEnabled() && !wrapper.isMagnetized && wrapper.getCanBeMagnetized() &&
               glm::distance(m_player->getObj()->getOrigin(), wrapper.obj->getOrigin()) < EnumsAndVars::playerMagneticRadius)
            {
                ++EnumsAndVars::garbageCountMagnetized;
                wrapper.isMagnetized = true;
                wrapper.obj->activate();

                glm::vec3 objToPlayerDir = glm::normalize(m_player->getObj()->getOrigin() - wrapper.obj->getOrigin());
                if(m_player->getMoveSpeed() > 0.1f)
                    objToPlayerDir = glm::normalize(objToPlayerDir + m_player->getMoveDir() * m_player->getMoveSpeed() * 0.05f);

                objToPlayerDir.y += 0.15f;

                wrapper.obj->setLinearVelocity(objToPlayerDir * std::max(15.0f, m_player->getMoveSpeed() * 0.8f));
                wrapper.pauseResetVelocity(0.25f);
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
        // 2. Spawn or respawn enemies. In subclass.
        else if(m_pathFindingIteration == 1)
        {
            ++m_pathFindingIteration;

            // 1.
            m_pathFinderEnemies.clearBlockedPositions();

            // 2. Spawn new enemies or respawn long distance enemies closer to player only in 28 sec after resurrection.
            if(EnumsAndVars::playerResurrectTime + 28.0f < EnumsAndVars::mapPlayTimeSec)
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
                        const int garbageIndex = rayAttack.hittedObjectID - m_idOfFirstGarbage;
                        BR_ASSERT((garbageIndex >= 0 && garbageIndex < m_allGarbage.size()), "%s", "garbageIndex out of range.");

                        if(enemy->attackType == AttackType::RANGE_DAMAGE_ONE || enemy->attackType == AttackType::MELEE_DAMAGE_ONE)
                        {
                            //BR_INFO("%s", "Garbage under attack =) by AttackType::RANGE_DAMAGE_ONE");
                            m_allGarbage[garbageIndex].takeDamage(enemy->damage);
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
                            m_allGarbage[garbageIndex].pauseMagnetization(1.25f);
                            m_allGarbage[garbageIndex].obj->setGravity(EnumsAndVars::garbageGravityDefault, false, false);
                            glm::vec3 impulseDir = glm::normalize(enemy->getOrigin() - m_allGarbage[garbageIndex].obj->getOrigin());
                            impulseDir.y += 0.4f;
                            m_allGarbage[garbageIndex].obj->applyCentralImpulse(impulseDir * 0.1f);
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
        m_idOfNearestEnemy = 0;
        float distanceToNearestEnemy = 999999.0f;
        for(const auto& enemy : m_allAnimatedEnemies)
        {
            if(!enemy->getIsEnabledUpdate())
                continue;

            const float distancePlayerToEnemy = glm::distance(enemy->getOrigin(), m_player->getObj()->getOrigin());

            if(distancePlayerToEnemy < distanceToNearestEnemy)
            {
                distanceToNearestEnemy = distancePlayerToEnemy;
                m_idOfNearestEnemy = enemy->getID();
            }

            if(enemy->garbageAmountToDie <= EnumsAndVars::garbageCountMagnetized && distancePlayerToEnemy < radiusToKill + (enemy->getXZRadius() * 0.5f))
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
        const std::vector<Beryll::Finger>& fingers = Beryll::EventHandler::getFingers();
        for(const Beryll::Finger& f : fingers)
        {
            if(f.handled || f.normalizedPos.x < 0.5f)
                continue;

            if(f.downEvent)
            {
                m_lastFingerMovePosX = f.normalizedPos.x;
                m_lastFingerMovePosY = f.normalizedPos.y;
                break;
            }
            else
            {
                float deltaX = (f.normalizedPos.x - m_lastFingerMovePosX) * EnumsAndVars::SettingsMenu::cameraHorizontalSpeed;
                const float deltaXInOneSecAbs = std::fabs(deltaX * (1.0f / Beryll::TimeStep::getTimeStepSec()));

                if(deltaXInOneSecAbs > EnumsAndVars::SettingsMenu::cameraSpeedThresholdToAccelerate)
                {
                    const float accelFactor = std::powf((deltaXInOneSecAbs - EnumsAndVars::SettingsMenu::cameraSpeedThresholdToAccelerate), 1.1f) * 0.001f;
                    deltaX = deltaX + deltaX * accelFactor;
                }

                float deltaY = (f.normalizedPos.y - m_lastFingerMovePosY) * EnumsAndVars::SettingsMenu::cameraVerticalSpeed;

                m_lastFingerMovePosX = f.normalizedPos.x;
                m_lastFingerMovePosY = f.normalizedPos.y;

                m_eyesLookAngleXZ += deltaX;
                m_eyesLookAngleY -= deltaY;
                if(m_eyesLookAngleY > 45.0f) m_eyesLookAngleY = 45.0f; // Eye up.
                if(m_eyesLookAngleY < -88.0f) m_eyesLookAngleY = -88.0f; // Eye down.
                //BR_INFO("m_eyesLookAngleXZ %f m_eyesLookAngleY %f", m_eyesLookAngleXZ, m_eyesLookAngleY);
                break;
            }
        }

        // Euler angles.
        float m_eyesLookAngleXZRadians = glm::radians(m_eyesLookAngleXZ);
        float m_eyesLookAngleYRadians = glm::radians(m_eyesLookAngleY);
        m_cameraAngleOffset.x = glm::cos(m_eyesLookAngleXZRadians) * glm::cos(m_eyesLookAngleYRadians);
        m_cameraAngleOffset.y = glm::sin(m_eyesLookAngleYRadians);
        m_cameraAngleOffset.z = glm::sin(m_eyesLookAngleXZRadians) * glm::cos(m_eyesLookAngleYRadians);
        m_cameraAngleOffset = glm::normalize(m_cameraAngleOffset);

        m_cameraFront = m_player->getObj()->getOrigin();

        float maxCameraYOffset = m_startCameraYOffset + (m_player->getObj()->getXZRadius() - 2.0f) + (EnumsAndVars::garbageCountMagnetized * 0.07f);

        if(m_player->getIsOnGround())
            maxCameraYOffset += m_player->getMoveSpeedXZ() * 0.07f;


        if(!m_cameraHit)
        {
            if(glm::distance(m_cameraYOffset, maxCameraYOffset) < 0.061f)
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
                                  (m_player->getMoveSpeedXZ() * 0.4f) +
                                  (EnumsAndVars::garbageCountMagnetized * 0.3f) +
                                  std::min(45.0f, m_player->getObj()->getOrigin().y * 0.16f)
                                  - std::min(0.0f, m_eyesLookAngleY * 1.1f);

        glm::vec3 cameraPosForRay = m_cameraFront - m_cameraAngleOffset * maxCameraDistance;

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

            const float hitDistance = glm::length(m_cameraFront - rayCameraHit.hitPoint);
            const float hitDistanceFraction = hitDistance / maxCameraDistance;

            m_cameraDistance = maxCameraDistance * hitDistanceFraction;
            m_cameraYOffset = maxCameraYOffset * hitDistanceFraction; // Change together with distance when camera collision.
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

        Beryll::Camera::setCameraPos(m_cameraFront - m_cameraAngleOffset * m_cameraDistance);
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
            m_player->getObj()->applyCentralImpulse(glm::normalize(-playerOrig) * 100.0f);
            m_player->checkVelocityOfGarbage();
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
