#include "Map5.h"
#include "EnumsAndVariables.h"
#include "enemies/MovableEnemy.h"
#include "Sounds.h"

namespace MagneticBall3D
{
    Map5::Map5(std::shared_ptr<PlayStateGUILayer> gui) : BaseMap(gui)
    {
        loadPlayer();
        m_player->getObj()->setOrigin(glm::vec3(-600.0f, 2.0f,-545.0f));
        m_improvements.setPlayer(m_player);
        loadEnv();
        loadGarbage();
        BR_ASSERT((m_allGarbage.size() < m_maxGarbageCount), "%s", "m_allGarbage reallocation happened. Increase maxGarbageCount.");
        loadEnemies();

        loadShaders();
        m_eyesLookAngleY = -5.0f;
        handleCamera();

        m_minX = -800.0f;
        m_maxX = 800.0f;
        m_minZ = -800.0f;
        m_maxZ = 800.0f;

        m_pathFinderEnemies = AStar(m_minX, m_maxX, m_minZ, m_maxZ, 20);
        std::vector<glm::vec3> walls = BeryllUtils::Common::loadMeshVerticesToVector("models3D/map5/PathEnemiesWalls.fbx");
        for(const auto& wall : walls)
        {
            m_pathFinderEnemies.addWallPosition({(int)std::roundf(wall.x), (int)std::roundf(wall.z)});
        }

        BR_INFO("Map5 pathfinder walls: %d", walls.size());

        std::vector<glm::vec3> allowedPoints = BeryllUtils::Common::loadMeshVerticesToVector("models3D/map5/PathEnemiesAllowedPositions.fbx");
        m_pathAllowedPositionsXZ.reserve(allowedPoints.size());
        for(const auto& point : allowedPoints)
        {
            m_pathAllowedPositionsXZ.push_back({(int)std::roundf(point.x), (int)std::roundf(point.z)});
        }

        BR_INFO("Map5 pathfinder allowed points: %d", m_pathAllowedPositionsXZ.size());
        m_pointsToSpawnEnemies.reserve(m_pathAllowedPositionsXZ.size());
        m_pointsToSpawnCommonGarbage.reserve(m_pathAllowedPositionsXZ.size());

        m_dirToSun = glm::normalize(glm::vec3(-1.0f, 1.8f, 0.5f));
        m_sunLightDir = -m_dirToSun;

        m_skyBox = Beryll::Renderer::createSkyBox("skyboxes/nightClouds");

        SendStatisticsHelper::sendMapStart();

        Sounds::startBackgroundMusic(SoundType::BACKGROUND_MUSIC_1);

        //BR_INFO(" %f", );
        //BR_INFO("%s", "");
    }

    Map5::~Map5()
    {
        Sounds::stopBackgroundMusic();
    }

    void Map5::draw()
    {
        //BR_INFO("%s", "Scene draw call.");
        m_improvements.draw();

        // 1. Draw into shadow map.
        glm::vec3 sunPos = m_player->getObj()->getOrigin() +
                           (Beryll::Camera::getCameraFrontDirectionXZ() * 200.0f) +
                           (m_dirToSun * (600.0f - m_player->getObj()->getOrigin().y)); // sunPos.y is 600 max.
        updateSunPosition(sunPos, m_shadowsCubeWidth, m_shadowsCubeHeight, m_shadowsCubeDepth);
        Beryll::Renderer::disableFaceCulling();
        if(BaseEnemy::getActiveCount() < 200)
            m_shadowMap->drawIntoShadowMap(m_simpleObjForShadowMap, m_animatedObjForShadowMap, m_sunLightVPMatrix);
        else
            m_shadowMap->drawIntoShadowMap(m_simpleObjForShadowMap, {}, m_sunLightVPMatrix);
        Beryll::Renderer::enableFaceCulling();

        // 2. Draw scene.
        // Sync HP bar rotations with camera direction.
        m_enemySize1HPBar.addToRotation(glm::rotation(m_enemySize1HPBar.getFaceDirXYZ(), Beryll::Camera::getCameraFrontDirectionXYZ()));
        m_enemySize1HPBar.addToRotation(glm::rotation(m_enemySize1HPBar.getUpDirXYZ(), Beryll::Camera::getCameraUp()));

        m_enemySize2HPBar.addToRotation(glm::rotation(m_enemySize2HPBar.getFaceDirXYZ(), Beryll::Camera::getCameraFrontDirectionXYZ()));
        m_enemySize2HPBar.addToRotation(glm::rotation(m_enemySize2HPBar.getUpDirXYZ(), Beryll::Camera::getCameraUp()));
        glm::vec3 HPBarOrigin{0.0f};
        for(const auto& enemy : m_allAnimatedEnemies)
        {
            if(enemy->getIsEnabledDraw() && enemy->getIsNeedShowHPBar())
            {
                if(enemy->getSceneObjectGroup() == Beryll::SceneObjectGroups::ENEMY_SIZE_1)
                {
                    HPBarOrigin = enemy->getOrigin();
                    HPBarOrigin.y += enemy->getFromOriginToTop() * 1.5f;
                    m_enemySize1HPBar.setOrigin(HPBarOrigin);
                    m_enemySize1HPBar.progress = 1.0f - (enemy->getCurrentHP() / enemy->getMaxHP());
                    m_enemySize1HPBar.draw();
                }
                else
                {
                    HPBarOrigin = enemy->getOrigin();
                    HPBarOrigin.y += enemy->getFromOriginToTop() * 1.7f;
                    m_enemySize2HPBar.setOrigin(HPBarOrigin);
                    m_enemySize2HPBar.progress = 1.0f - (enemy->getCurrentHP() / enemy->getMaxHP());
                    m_enemySize2HPBar.draw();
                }
            }
        }

        glm::mat4 modelMatrix{1.0f};

        if(EnumsAndVars::gameOnPause || EnumsAndVars::improvementSystemOnScreen)
            m_ambientLight = 0.15f;
        else
            m_ambientLight = 0.6f;

        m_animatedObjSunLight->bind();
        m_animatedObjSunLight->set3Float("sunLightDir", m_sunLightDir);
        m_animatedObjSunLight->set1Float("ambientLight", m_ambientLight);

        for(const auto& animObj : m_allAnimatedEnemies)
        {
            if(animObj->getIsEnabledDraw())
            {
                modelMatrix = animObj->getModelMatrix();
                m_animatedObjSunLight->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
                Beryll::Renderer::drawObject(animObj, modelMatrix, m_animatedObjSunLight);
            }
        }

        m_simpleObjSunLightShadows->bind();
        m_simpleObjSunLightShadows->set3Float("sunLightDir", m_sunLightDir);
        m_simpleObjSunLightShadows->set3Float("cameraPos", Beryll::Camera::getCameraPos());
        m_simpleObjSunLightShadows->set1Float("ambientLight", m_ambientLight);
        m_simpleObjSunLightShadows->set1Float("sunLightStrength", 0.25f);
        m_simpleObjSunLightShadows->set1Float("specularLightStrength", 1.0f);
        m_simpleObjSunLightShadows->set1Float("alphaTransparency", 1.0f);

        modelMatrix = m_player->getObj()->getModelMatrix();
        m_simpleObjSunLightShadows->setMatrix4x4Float("MVPLightMatrix", m_sunLightVPMatrix * modelMatrix);
        m_simpleObjSunLightShadows->setMatrix4x4Float("modelMatrix", modelMatrix);
        m_simpleObjSunLightShadows->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
        Beryll::Renderer::drawObject(m_player->getObj(), modelMatrix, m_simpleObjSunLightShadows);

        m_simpleObjSunLightShadows->set1Float("sunLightStrength", 1.0f);
        m_simpleObjSunLightShadows->set1Float("specularLightStrength", 1.0f);
        for(const auto& wrapper : m_allGarbage)
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

        m_simpleObjSunLightShadows->set1Float("specularLightStrength", 0.7f);
        for(const auto& staticObj : m_staticEnv)
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

        m_simpleObjSunLightShadowsNormals->bind();
        m_simpleObjSunLightShadowsNormals->set3Float("sunLightDir", m_sunLightDir);
        m_simpleObjSunLightShadowsNormals->set3Float("cameraPos", Beryll::Camera::getCameraPos());
        m_simpleObjSunLightShadowsNormals->set1Float("ambientLight", m_ambientLight * 0.5f);
        m_simpleObjSunLightShadowsNormals->set1Float("specularLightStrength", 0.25f);

        for(const auto& normalMapObj : m_objWithNormalMap)
        {
            modelMatrix = normalMapObj->getModelMatrix();
            m_simpleObjSunLightShadowsNormals->setMatrix4x4Float("MVPLightMatrix", m_sunLightVPMatrix * modelMatrix);
            m_simpleObjSunLightShadowsNormals->setMatrix4x4Float("modelMatrix", modelMatrix);
            m_simpleObjSunLightShadowsNormals->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
            Beryll::Renderer::drawObject(normalMapObj, modelMatrix, m_simpleObjSunLightShadowsNormals);
        }

        m_skyBox->draw();
        Beryll::ParticleSystem::draw();
    }

    void Map5::loadEnv()
    {
        const auto mainGround = std::make_shared<Beryll::SimpleCollidingObject>("models3D/map5/MainGround.fbx",
                                                                                0.0f,
                                                                                false,
                                                                                Beryll::CollisionFlags::STATIC,
                                                                                Beryll::CollisionGroups::GROUND,
                                                                                Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE |
                                                                                Beryll::CollisionGroups::MOVABLE_ENEMY | Beryll::CollisionGroups::CAMERA,
                                                                                Beryll::SceneObjectGroups::GROUND);

        m_objWithNormalMap.push_back(mainGround);
        mainGround->setFriction(EnumsAndVars::staticEnvFriction);

        const auto heapNormalMap = std::make_shared<Beryll::SimpleCollidingObject>("models3D/map5/GroundHeapNormalMap.fbx",
                                                                                   0.0f,
                                                                                   false,
                                                                                   Beryll::CollisionFlags::STATIC,
                                                                                   Beryll::CollisionGroups::GROUND,
                                                                                   Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE |
                                                                                   Beryll::CollisionGroups::CAMERA | Beryll::CollisionGroups::MOVABLE_ENEMY |
                                                                                   Beryll::CollisionGroups::RAY_FOR_ENVIRONMENT,
                                                                                   Beryll::SceneObjectGroups::GROUND);

        m_objWithNormalMap.push_back(heapNormalMap);
        heapNormalMap->setFriction(EnumsAndVars::staticEnvFriction);

        const auto otherGrounds = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map5/OtherGrounds.fbx",
                                                                                           0.0f,
                                                                                           false,
                                                                                           Beryll::CollisionFlags::STATIC,
                                                                                           Beryll::CollisionGroups::GROUND,
                                                                                           Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE |
                                                                                           Beryll::CollisionGroups::CAMERA | Beryll::CollisionGroups::MOVABLE_ENEMY |
                                                                                           Beryll::CollisionGroups::RAY_FOR_ENVIRONMENT,
                                                                                           Beryll::SceneObjectGroups::GROUND);

        for(const auto& obj : otherGrounds)
        {
            m_staticEnv.push_back(obj);
            obj->setFriction(EnumsAndVars::staticEnvFriction);
        }

        const auto buildings = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map5/Buildings.fbx",
                                                                                        0.0f,
                                                                                        false,
                                                                                        Beryll::CollisionFlags::STATIC,
                                                                                        Beryll::CollisionGroups::BUILDING,
                                                                                        Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE |
                                                                                        Beryll::CollisionGroups::RAY_FOR_ENVIRONMENT | Beryll::CollisionGroups::CAMERA,
                                                                                        Beryll::SceneObjectGroups::BUILDING);

        for(const auto& obj : buildings)
        {
            m_staticEnv.push_back(obj);
            m_simpleObjForShadowMap.push_back(obj);
            obj->setFriction(EnumsAndVars::staticEnvFriction);
        }

        const auto jumpPads = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map5/JumpPads.fbx",
                                                                                       0.0f,
                                                                                       false,
                                                                                       Beryll::CollisionFlags::STATIC,
                                                                                       Beryll::CollisionGroups::JUMPPAD,
                                                                                       Beryll::CollisionGroups::PLAYER,
                                                                                       Beryll::SceneObjectGroups::JUMPPAD);

        for(const auto& obj : jumpPads)
        {
            m_staticEnv.push_back(obj);
        }

        const auto envNoColliders1 = Beryll::SimpleObject::loadManyModelsFromOneFile("models3D/map5/EnvNoColliders.fbx", Beryll::SceneObjectGroups::BUILDING);

        for(const auto& obj : envNoColliders1)
        {
            m_staticEnv.push_back(obj);
            m_simpleObjForShadowMap.push_back(obj);
        }
    }

    void Map5::loadGarbage()
    {
        m_idOfFirstGarbage = BeryllUtils::Common::getLastGeneratedID() + 1;

        for(int i = 0; i < 6; ++i) // 6 * 31 = 186
        {
            const auto garbageCommon = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map5/GarbageCommon_31items.fbx",
                                                                                                EnumsAndVars::garbageMass,
                                                                                                false,
                                                                                                Beryll::CollisionFlags::DYNAMIC,
                                                                                                Beryll::CollisionGroups::GARBAGE,
                                                                                                Beryll::CollisionGroups::GROUND | Beryll::CollisionGroups::BUILDING |
                                                                                                Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE |
                                                                                                Beryll::CollisionGroups::ENEMY_ATTACK,
                                                                                                Beryll::SceneObjectGroups::GARBAGE);

            for(const auto& obj : garbageCommon)
            {
                m_allGarbage.emplace_back(obj, GarbageType::COMMON, EnumsAndVars::garbageStartHP);

                m_animatedOrDynamicObjects.push_back(obj);
                m_simpleObjForShadowMap.push_back(obj);

                obj->setDamping(EnumsAndVars::garbageLinearDamping, EnumsAndVars::garbageAngularDamping);
                obj->setGravity(EnumsAndVars::garbageGravityDefault, false, false);
            }
        }

        for(int i = 0; i < 5; ++i) // 5 * 3 = 15
        {
            const auto garbageEnemy = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map5/GarbageEnemySkeleton_3items.fbx",
                                                                                               EnumsAndVars::garbageMass,
                                                                                               false,
                                                                                               Beryll::CollisionFlags::DYNAMIC,
                                                                                               Beryll::CollisionGroups::GARBAGE,
                                                                                               Beryll::CollisionGroups::GROUND | Beryll::CollisionGroups::BUILDING |
                                                                                               Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE |
                                                                                               Beryll::CollisionGroups::ENEMY_ATTACK,
                                                                                               Beryll::SceneObjectGroups::GARBAGE);

            for(const auto& obj : garbageEnemy)
            {
                m_allGarbage.emplace_back(obj, GarbageType::ENEMY_GARBAGE1, EnumsAndVars::garbageStartHP);

                m_animatedOrDynamicObjects.push_back(obj);
                m_simpleObjForShadowMap.push_back(obj);

                obj->setDamping(EnumsAndVars::garbageLinearDamping, EnumsAndVars::garbageAngularDamping);
                obj->setGravity(EnumsAndVars::garbageGravityDefault, false, false);
            }
        }

        for(int i = 0; i < 4; ++i) // 4 * 4 = 16
        {
            const auto garbageEnemy = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map5/GarbageEnemyGunShield_4items.fbx",
                                                                                               EnumsAndVars::garbageMass,
                                                                                               false,
                                                                                               Beryll::CollisionFlags::DYNAMIC,
                                                                                               Beryll::CollisionGroups::GARBAGE,
                                                                                               Beryll::CollisionGroups::GROUND | Beryll::CollisionGroups::BUILDING |
                                                                                               Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE |
                                                                                               Beryll::CollisionGroups::ENEMY_ATTACK,
                                                                                               Beryll::SceneObjectGroups::GARBAGE);

            for(const auto& obj : garbageEnemy)
            {
                m_allGarbage.emplace_back(obj, GarbageType::ENEMY_GARBAGE2, EnumsAndVars::garbageStartHP);

                m_animatedOrDynamicObjects.push_back(obj);
                m_simpleObjForShadowMap.push_back(obj);

                obj->setDamping(EnumsAndVars::garbageLinearDamping, EnumsAndVars::garbageAngularDamping);
                obj->setGravity(EnumsAndVars::garbageGravityDefault, false, false);
            }
        }

        for(int i = 0; i < 5; ++i) // 5 * 3 = 15
        {
            const auto garbageEnemy = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map5/GarbageEnemyGhoul_3items.fbx",
                                                                                               EnumsAndVars::garbageMass,
                                                                                               false,
                                                                                               Beryll::CollisionFlags::DYNAMIC,
                                                                                               Beryll::CollisionGroups::GARBAGE,
                                                                                               Beryll::CollisionGroups::GROUND | Beryll::CollisionGroups::BUILDING |
                                                                                               Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE |
                                                                                               Beryll::CollisionGroups::ENEMY_ATTACK,
                                                                                               Beryll::SceneObjectGroups::GARBAGE);

            for(const auto& obj : garbageEnemy)
            {
                m_allGarbage.emplace_back(obj, GarbageType::ENEMY_GARBAGE3, EnumsAndVars::garbageStartHP);

                m_animatedOrDynamicObjects.push_back(obj);
                m_simpleObjForShadowMap.push_back(obj);

                obj->setDamping(EnumsAndVars::garbageLinearDamping, EnumsAndVars::garbageAngularDamping);
                obj->setGravity(EnumsAndVars::garbageGravityDefault, false, false);
            }
        }
    }

    void Map5::loadEnemies()
    {
        m_idOfFirstEnemy = BeryllUtils::Common::getLastGeneratedID() + 1;

        for(int i = 0; i < 120; ++i)
        {
            auto skeleton = std::make_shared<MovableEnemy>("models3D/enemies/Skeleton.fbx",
                                                           0.0f,
                                                           false,
                                                           Beryll::CollisionFlags::STATIC,
                                                           Beryll::CollisionGroups::NONE,
                                                           Beryll::CollisionGroups::NONE,
                                                           Beryll::SceneObjectGroups::ENEMY_SIZE_1,
                                                           13.0f);

            skeleton->setCurrentAnimationByIndex(EnumsAndVars::AnimationIndexes::run, false, false, true);
            skeleton->setDefaultAnimationByIndex(EnumsAndVars::AnimationIndexes::stand);
            skeleton->unitType = UnitType::ENEMY_GUN1;
            skeleton->attackType = AttackType::RANGE_DAMAGE_ONE;
            skeleton->attackSound = SoundType::NONE;
            skeleton->attackHitSound = SoundType::STONE_HIT;
            skeleton->attackParticlesColor = glm::vec3{0.5f, 0.66f, 0.86f};
            skeleton->attackParticlesSize = 0.3f;
            skeleton->dieGarbageType = GarbageType::ENEMY_GARBAGE1;
            skeleton->castRayToFindYPos = true;

            skeleton->damage = 2.0f;
            skeleton->attackDistance = 80.0f + Beryll::RandomGenerator::getFloat() * 120.0f;
            skeleton->timeBetweenAttacks = 2.0f + Beryll::RandomGenerator::getFloat() * 0.5f;

            skeleton->reducePlayerSpeedWhenTakeSmashDamage = 1.0f;
            skeleton->experienceWhenDie = 30;
            skeleton->getController().moveSpeed = 55.0f;

            m_animatedOrDynamicObjects.push_back(skeleton);
            m_allAnimatedEnemies.push_back(skeleton);
            m_animatedObjForShadowMap.push_back(skeleton);
        }

        for(int i = 0; i < 100; ++i)
        {
            auto copShield = std::make_shared<MovableEnemy>("models3D/enemies/CopWithPistolShield.fbx",
                                                            0.0f,
                                                            false,
                                                            Beryll::CollisionFlags::STATIC,
                                                            Beryll::CollisionGroups::NONE,
                                                            Beryll::CollisionGroups::NONE,
                                                            Beryll::SceneObjectGroups::ENEMY_SIZE_1,
                                                            13.0f);

            copShield->setCurrentAnimationByIndex(EnumsAndVars::AnimationIndexes::run, false, false, true);
            copShield->setDefaultAnimationByIndex(EnumsAndVars::AnimationIndexes::stand);
            copShield->unitType = UnitType::ENEMY_GUN_SHIELD;
            copShield->attackType = AttackType::RANGE_DAMAGE_ONE;
            copShield->attackSound = SoundType::PISTOL_SHOT;
            copShield->attackHitSound = SoundType::PISTOL_HIT;
            copShield->attackParticlesColor = glm::vec3{0.9f, 0.9f, 0.0f};
            copShield->attackParticlesSize = 0.3f;
            copShield->dieGarbageType = GarbageType::ENEMY_GARBAGE2;
            copShield->castRayToFindYPos = true;

            copShield->damage = 2.0f;
            copShield->attackDistance = 200.0f + Beryll::RandomGenerator::getFloat() * 150.0f;
            copShield->timeBetweenAttacks = 2.5f + Beryll::RandomGenerator::getFloat() * 0.5f;

            copShield->reducePlayerSpeedWhenTakeSmashDamage = 1.0f;
            copShield->experienceWhenDie = 30;
            copShield->getController().moveSpeed = 45.0f;

            m_animatedOrDynamicObjects.push_back(copShield);
            m_allAnimatedEnemies.push_back(copShield);
            m_animatedObjForShadowMap.push_back(copShield);
        }

        for(int i = 0; i < 100; ++i)
        {
            auto ghoul = std::make_shared<MovableEnemy>("models3D/enemies/Ghoul.fbx",
                                                           0.0f,
                                                           false,
                                                           Beryll::CollisionFlags::STATIC,
                                                           Beryll::CollisionGroups::NONE,
                                                           Beryll::CollisionGroups::NONE,
                                                           Beryll::SceneObjectGroups::ENEMY_SIZE_2,
                                                           21.0f);

            ghoul->setCurrentAnimationByIndex(EnumsAndVars::AnimationIndexes::run, false, false, true);
            ghoul->setDefaultAnimationByIndex(EnumsAndVars::AnimationIndexes::stand);
            ghoul->unitType = UnitType::ENEMY_TANK;
            ghoul->attackType = AttackType::RANGE_DAMAGE_RADIUS;
            ghoul->attackSound = SoundType::NONE;
            ghoul->attackHitSound = SoundType::STONE_HIT;
            ghoul->attackParticlesColor = glm::vec3{0.43f, 0.84f, 0.808f};
            ghoul->attackParticlesSize = 0.6f;
            ghoul->dieGarbageType = GarbageType::ENEMY_GARBAGE3;
            ghoul->castRayToFindYPos = true;

            ghoul->damage = 1.5f;
            ghoul->damageRadius = 10.0f;
            ghoul->attackDistance = 150.0f + Beryll::RandomGenerator::getFloat() * 150.0f;
            ghoul->timeBetweenAttacks = 3.0f + Beryll::RandomGenerator::getFloat() * 0.5f;

            ghoul->reducePlayerSpeedWhenTakeSmashDamage = 1.0f;
            ghoul->experienceWhenDie = 35;
            ghoul->getController().moveSpeed = 35.0f;

            m_animatedOrDynamicObjects.push_back(ghoul);
            m_allAnimatedEnemies.push_back(ghoul);
            m_animatedObjForShadowMap.push_back(ghoul);
        }
    }

    void Map5::spawnEnemies()
    {
        // Prepare waves.
        if(m_prepareWave1 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave1Time)
        {
            m_prepareWave1 = false;

            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;
            }

            BR_INFO("Prepare wave 1. Max enemies: %d", 0);
        }
        else if(m_prepareWave2 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave2Time)
        {
            m_prepareWave2 = false;

            int gun1Count = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(gun1Count < 20 && enemy->unitType == UnitType::ENEMY_GUN1)
                {
                    enemy->isCanBeSpawned = true;
                    ++gun1Count;
                }
            }

            BR_INFO("Prepare wave 2. Max enemies: %d", gun1Count);
        }
        else if(m_prepareWave3 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave3Time)
        {
            m_prepareWave3 = false;

            int gun1Count = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(gun1Count < 40 && enemy->unitType == UnitType::ENEMY_GUN1)
                {
                    enemy->isCanBeSpawned = true;
                    ++gun1Count;
                }
            }

            BR_INFO("Prepare wave 3. Max enemies: %d", gun1Count);
        }
        else if(m_prepareWave4 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave4Time)
        {
            m_prepareWave4 = false;

            int gun1Count = 0;
            int gunShieldCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(gun1Count < 40 && enemy->unitType == UnitType::ENEMY_GUN1)
                {
                    enemy->isCanBeSpawned = true;
                    ++gun1Count;
                }
                if(gunShieldCount < 30 && enemy->unitType == UnitType::ENEMY_GUN_SHIELD)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunShieldCount;
                }
            }

            BR_INFO("Prepare wave 4. Max enemies: %d", gun1Count + gunShieldCount);
        }
        else if(m_prepareWave5 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave5Time)
        {
            m_prepareWave5 = false;

            int gun1Count = 0;
            int gunShieldCount = 0;
            int tankCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(gun1Count < 50 && enemy->unitType == UnitType::ENEMY_GUN1)
                {
                    enemy->isCanBeSpawned = true;
                    ++gun1Count;
                }
                if(gunShieldCount < 30 && enemy->unitType == UnitType::ENEMY_GUN_SHIELD)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunShieldCount;
                }
                if(tankCount < 20 && enemy->unitType == UnitType::ENEMY_TANK)
                {
                    enemy->isCanBeSpawned = true;
                    ++tankCount;
                }
            }

            BR_INFO("Prepare wave 5. Max enemies: %d", gun1Count + gunShieldCount + tankCount);
        }
        else if(m_prepareWave6 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave6Time)
        {
            m_prepareWave6 = false;

            int gun1Count = 0;
            int gunShieldCount = 0;
            int tankCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(gun1Count < 60 && enemy->unitType == UnitType::ENEMY_GUN1)
                {
                    enemy->isCanBeSpawned = true;
                    ++gun1Count;
                }
                if(gunShieldCount < 40 && enemy->unitType == UnitType::ENEMY_GUN_SHIELD)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunShieldCount;
                }
                if(tankCount < 40 && enemy->unitType == UnitType::ENEMY_TANK)
                {
                    enemy->isCanBeSpawned = true;
                    ++tankCount;
                }
            }

            BR_INFO("Prepare wave 6. Max enemies: %d", gun1Count + gunShieldCount + tankCount);
        }
        else if(m_prepareWave7 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave7Time)
        {
            m_prepareWave7 = false;

            int gun1Count = 0;
            int gunShieldCount = 0;
            int tankCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(gun1Count < 80 && enemy->unitType == UnitType::ENEMY_GUN1)
                {
                    enemy->isCanBeSpawned = true;
                    ++gun1Count;
                }
                if(gunShieldCount < 50 && enemy->unitType == UnitType::ENEMY_GUN_SHIELD)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunShieldCount;
                }
                if(tankCount < 60 && enemy->unitType == UnitType::ENEMY_TANK)
                {
                    enemy->isCanBeSpawned = true;
                    ++tankCount;
                }
            }

            BR_INFO("Prepare wave 7. Max enemies: %d", gun1Count + gunShieldCount + tankCount);
        }
        else if(m_prepareWave8 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave8Time)
        {
            m_prepareWave8 = false;

            int gun1Count = 0;
            int gunShieldCount = 0;
            int tankCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(gun1Count < 100 && enemy->unitType == UnitType::ENEMY_GUN1)
                {
                    enemy->isCanBeSpawned = true;
                    ++gun1Count;
                }
                if(gunShieldCount < 80 && enemy->unitType == UnitType::ENEMY_GUN_SHIELD)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunShieldCount;
                }
                if(tankCount < 80 && enemy->unitType == UnitType::ENEMY_TANK)
                {
                    enemy->isCanBeSpawned = true;
                    ++tankCount;
                }
            }

            BR_INFO("Prepare wave 8. Max enemies: %d", gun1Count + gunShieldCount + tankCount);
        }
        else if(m_prepareWave9 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave9Time)
        {
            m_prepareWave9 = false;

            int gun1Count = 0;
            int gunShieldCount = 0;
            int tankCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(gun1Count < 120 && enemy->unitType == UnitType::ENEMY_GUN1)
                {
                    enemy->isCanBeSpawned = true;
                    ++gun1Count;
                }
                if(gunShieldCount < 100 && enemy->unitType == UnitType::ENEMY_GUN_SHIELD)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunShieldCount;
                }
                if(tankCount < 100 && enemy->unitType == UnitType::ENEMY_TANK)
                {
                    enemy->isCanBeSpawned = true;
                    ++tankCount;
                }
            }

            BR_INFO("Prepare wave 9. Max enemies: %d", gun1Count + gunShieldCount + tankCount);
        }
        else if(m_prepareLastWave && EnumsAndVars::mapPlayTimeSec > m_prepareLastWaveTime)
        {
            m_prepareLastWave = false;

            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;
            }

            BR_INFO("enemiesLastWavePhase(). Max enemies: %d", 0);

            lastWaveToWinPhase();
        }

        // Spawn new of respawn long distance enemies.
        if(!m_pointsToSpawnEnemies.empty())
        {
            for(const auto& enemy : m_allAnimatedEnemies)
            {
                if((enemy->getIsEnabled() && glm::distance(m_player->getObj()->getOriginXZ(), enemy->getOriginXZ()) > EnumsAndVars::enemiesRespawnDistance) ||
                   (!enemy->getIsEnabledUpdate() && enemy->isCanBeSpawned))
                {
                    enemy->enableEnemy();
                    enemy->disableDraw();

                    const glm::ivec2 spawnPoint2D = m_pointsToSpawnEnemies[Beryll::RandomGenerator::getInt(m_pointsToSpawnEnemies.size() - 1)];
                    enemy->setPathArray(m_pathFinderEnemies.findPath(spawnPoint2D, m_playerClosestAllowedPos, 6), 1);
                    m_pathFinderEnemies.addBlockedPosition(enemy->getCurrentPointToMove2DInt());
                    enemy->setOrigin(enemy->getStartPointMoveFrom());
                }
            }
        }

        //BR_INFO("BaseEnemy::getActiveCount(): %d", BaseEnemy::getActiveCount());
    }

    void Map5::spawnCommonGarbage()
    {
        if(!m_pointsToSpawnCommonGarbage.empty() &&
           EnumsAndVars::garbageCommonSpawnTime + EnumsAndVars::garbageCommonSpawnDelay < EnumsAndVars::mapPlayTimeSec)
        {
            EnumsAndVars::garbageCommonSpawnTime = EnumsAndVars::mapPlayTimeSec;

            const glm::ivec2 spawnPoint2D = m_pointsToSpawnCommonGarbage[Beryll::RandomGenerator::getInt(m_pointsToSpawnCommonGarbage.size() - 1)];
            glm::vec3 spawnPoint3D{spawnPoint2D.x, 10.0f, spawnPoint2D.y};

            Beryll::RayClosestHit rayHit = Beryll::Physics::castRayClosestHit(glm::vec3{spawnPoint2D.x, 100.0f, spawnPoint2D.y},
                                                                              glm::vec3{spawnPoint2D.x, -10.0f, spawnPoint2D.y},
                                                                              Beryll::CollisionGroups::GARBAGE,
                                                                              Beryll::CollisionGroups::GROUND);

            if(rayHit)
            {
                spawnPoint3D.y = rayHit.hitPoint.y + 10.0f;
            }

            spawnGarbage(EnumsAndVars::garbageCommonSpawnCount, GarbageType::COMMON, spawnPoint3D);
            //BR_INFO("Garbage::getCommonActiveCount() %d", Garbage::getCommonActiveCount());
        }
    }
}
