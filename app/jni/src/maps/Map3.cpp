#include "Map3.h"
#include "EnumsAndVariables.h"
#include "enemies/MovableEnemy.h"
#include "Sounds.h"

namespace MagneticBall3D
{
    Map3::Map3(std::shared_ptr<PlayStateGUILayer> gui) : BaseMap(std::move(gui))
    {
        loadPlayer();
        m_player->getObj()->setOrigin(glm::vec3(-500.0f, 2.0f,-400.0f));
        m_improvements.setPlayer(m_player);
        loadEnv();
        loadGarbage();
        BR_ASSERT((m_allGarbage.size() < m_maxGarbageCount), "%s", "m_allGarbage reallocation happened. Increase maxGarbageCount.");
        loadEnemies();

        loadShaders();
        handleCamera();

        m_minX = -800.0f;
        m_maxX = 800.0f;
        m_minZ = -800.0f;
        m_maxZ = 800.0f;

        m_pathFinderEnemies = AStar(m_minX, m_maxX, m_minZ, m_maxZ, 20);
        std::vector<glm::vec3> walls = BeryllUtils::Common::loadMeshVerticesToVector("models3D/map3/PathEnemiesWalls.fbx");
        for(const auto& wall : walls)
        {
            m_pathFinderEnemies.addWallPosition({(int)std::roundf(wall.x), (int)std::roundf(wall.z)});
        }

        BR_INFO("Map3 pathfinder walls: %d", walls.size());

        std::vector<glm::vec3> allowedPoints = BeryllUtils::Common::loadMeshVerticesToVector("models3D/map3/PathEnemiesAllowedPositions.fbx");
        m_pathAllowedPositionsXZ.reserve(allowedPoints.size());
        for(const auto& point : allowedPoints)
        {
            m_pathAllowedPositionsXZ.push_back({(int)std::roundf(point.x), (int)std::roundf(point.z)});
        }

        BR_INFO("Map3 pathfinder allowed points: %d", m_pathAllowedPositionsXZ.size());
        m_pointsToSpawnEnemies.reserve(m_pathAllowedPositionsXZ.size());
        m_pointsToSpawnCommonGarbage.reserve(m_pathAllowedPositionsXZ.size());

        m_dirToSun = glm::normalize(glm::vec3(-1.0f, 3.0f, -1.0f));
        m_sunLightDir = -m_dirToSun;

        m_skyBox = Beryll::Renderer::createSkyBox("skyboxes/whiteClouds");

        if(EnumsAndVars::playerMagneticRadius < 70.0f)
            EnumsAndVars::playerMagneticRadius = 70.0f;
        if(EnumsAndVars::garbageMaxCountMagnetized < 80.0f)
            EnumsAndVars::garbageMaxCountMagnetized = 80.0f;
        if(EnumsAndVars::playerImpulseFactorOnGround < 1.7f)
            EnumsAndVars::playerImpulseFactorOnGround = 1.7f;
        if(EnumsAndVars::playerTorqueFactorOnGround < 1.6f)
            EnumsAndVars::playerTorqueFactorOnGround = 1.6f;
        if(EnumsAndVars::playerImpulseFactorOnBuildingRoof < 1.5f)
            EnumsAndVars::playerImpulseFactorOnBuildingRoof = 1.5f;
        if(EnumsAndVars::playerTorqueFactorOnBuildingRoof < 1.4f)
            EnumsAndVars::playerTorqueFactorOnBuildingRoof = 1.4f;
        if(EnumsAndVars::playerTorqueFactorOnBuildingWall < 10.0f)
            EnumsAndVars::playerTorqueFactorOnBuildingWall = 10.0f;

        SendStatisticsHelper::sendMapStart();

        Sounds::startBackgroundMusic(SoundType::BACKGROUND_MUSIC_1);

        //BR_INFO(" %f", );
        //BR_INFO("%s", "");
    }

    Map3::~Map3()
    {
        Sounds::stopBackgroundMusic();
    }

    void Map3::draw()
    {
        //BR_INFO("%s", "Scene draw call.");
        m_improvements.draw();

        // 1. Draw into shadow map.
        glm::vec3 sunPos = m_player->getObj()->getOrigin() +
                           (Beryll::Camera::getCameraFrontDirectionXZ() * 200.0f) +
                           (m_dirToSun * (600.0f - m_player->getObj()->getOrigin().y)); // sunPos.y is 600 max.
        updateSunPosition(sunPos, m_shadowsCubeWidth, m_shadowsCubeHeight, m_shadowsCubeDepth);

        Beryll::Renderer::disableFaceCulling();
        m_shadowMap->drawIntoShadowMap(m_simpleObjForShadowMap, m_animatedObjForShadowMap, m_sunLightVPMatrix);
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
        m_simpleObjSunLightShadows->set1Float("specularLightStrength", 1.1f);
        m_simpleObjSunLightShadows->set1Float("alphaTransparency", 1.0f);

        modelMatrix = m_player->getObj()->getModelMatrix();
        m_simpleObjSunLightShadows->setMatrix4x4Float("MVPLightMatrix", m_sunLightVPMatrix * modelMatrix);
        m_simpleObjSunLightShadows->setMatrix4x4Float("modelMatrix", modelMatrix);
        m_simpleObjSunLightShadows->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
        Beryll::Renderer::drawObject(m_player->getObj(), modelMatrix, m_simpleObjSunLightShadows);

        m_simpleObjSunLightShadows->set1Float("sunLightStrength", 1.0f);
        m_simpleObjSunLightShadows->set1Float("specularLightStrength", 1.1f);
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

    void Map3::loadEnv()
    {
        // Ground + some env no colliders has normal map in material.
        const auto mainGround = std::make_shared<Beryll::SimpleCollidingObject>("models3D/map3/Ground.fbx",
                                                                                0.0f,
                                                                                false,
                                                                                Beryll::CollisionFlags::STATIC,
                                                                                Beryll::CollisionGroups::GROUND,
                                                                                Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE | Beryll::CollisionGroups::CAMERA,
                                                                                Beryll::SceneObjectGroups::GROUND);

        m_objWithNormalMap.push_back(mainGround);
        mainGround->setFriction(EnumsAndVars::staticEnvFriction);

        const auto buildings = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map3/Buildings.fbx",
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

        const auto jumpPads = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map3/JumpPads.fbx",
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

        const auto envNoColliders1 = Beryll::SimpleObject::loadManyModelsFromOneFile("models3D/map3/EnvNoColliders.fbx", Beryll::SceneObjectGroups::BUILDING);

        for(const auto& obj : envNoColliders1)
        {
            m_staticEnv.push_back(obj);
            m_simpleObjForShadowMap.push_back(obj);
        }

        const auto envNoCollidersNormalMap = Beryll::SimpleObject::loadManyModelsFromOneFile("models3D/map3/EnvNoCollidersNormalMap.fbx", Beryll::SceneObjectGroups::BUILDING);

        for(const auto& obj : envNoCollidersNormalMap)
        {
            m_objWithNormalMap.push_back(obj);
        }
    }

    void Map3::loadGarbage()
    {
        m_idOfFirstGarbage = BeryllUtils::Common::getLastGeneratedID() + 1;

        for(int i = 0; i < 6; ++i) // 6 * 31 = 186
        {
            const auto garbageCommon = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map3/GarbageCommon_31items.fbx",
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

        for(int i = 0; i < 4; ++i) // 4 * 4 = 16
        {
            const auto garbageEnemy = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map3/GarbageEnemyJanitorRake_4items.fbx",
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
            const auto garbageEnemy = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map3/GarbageEnemyJanitorBroom_4items.fbx",
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

        for(int i = 0; i < 4; ++i) // 4 * 4 = 16
        {
            const auto garbageEnemy = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map3/GarbageEnemyGunShield_4items.fbx",
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

    void Map3::loadEnemies()
    {
        m_idOfFirstEnemy = BeryllUtils::Common::getLastGeneratedID() + 1;

        for(int i = 0; i < 110; ++i)
        {
            auto janitorRake = std::make_shared<MovableEnemy>("models3D/enemies/JanitorRake.fbx",
                                                               0.0f,
                                                               false,
                                                               Beryll::CollisionFlags::STATIC,
                                                               Beryll::CollisionGroups::NONE,
                                                               Beryll::CollisionGroups::NONE,
                                                               Beryll::SceneObjectGroups::ENEMY_SIZE_1,
                                                               13.0f);

            janitorRake->setCurrentAnimationByIndex(EnumsAndVars::AnimationIndexes::run, false, false, true);
            janitorRake->setDefaultAnimationByIndex(EnumsAndVars::AnimationIndexes::stand);
            janitorRake->unitType = UnitType::ENEMY_MELEE;
            janitorRake->attackType = AttackType::MELEE_DAMAGE_ONE;
            janitorRake->attackSound = SoundType::NONE;
            janitorRake->attackHitSound = SoundType::STICK_HIT;
            janitorRake->dieGarbageType = GarbageType::ENEMY_GARBAGE1;

            janitorRake->damage = 15.0f;
            janitorRake->attackDistance = 45.0f + Beryll::RandomGenerator::getFloat() * 10.0f;
            janitorRake->timeBetweenAttacks = 1.5f + Beryll::RandomGenerator::getFloat() * 0.5f;

            janitorRake->reducePlayerSpeedWhenTakeSmashDamage = 0.0f;
            janitorRake->experienceWhenDie = 35;
            janitorRake->getController().moveSpeed = 40.0f;

            m_animatedOrDynamicObjects.push_back(janitorRake);
            m_allAnimatedEnemies.push_back(janitorRake);
            m_animatedObjForShadowMap.push_back(janitorRake);
        }

        for(int i = 0; i < 100; ++i)
        {
            auto janitorBroom = std::make_shared<MovableEnemy>("models3D/enemies/JanitorBroom.fbx",
                                                               0.0f,
                                                               false,
                                                               Beryll::CollisionFlags::STATIC,
                                                               Beryll::CollisionGroups::NONE,
                                                               Beryll::CollisionGroups::NONE,
                                                               Beryll::SceneObjectGroups::ENEMY_SIZE_1,
                                                               13.0f);

            janitorBroom->setCurrentAnimationByIndex(EnumsAndVars::AnimationIndexes::run, false, false, true);
            janitorBroom->setDefaultAnimationByIndex(EnumsAndVars::AnimationIndexes::stand);
            janitorBroom->unitType = UnitType::ENEMY_GUN1;
            janitorBroom->attackType = AttackType::RANGE_DAMAGE_ONE;
            janitorBroom->attackSound = SoundType::NONE;
            janitorBroom->attackHitSound = SoundType::BROOM_HIT;
            janitorBroom->attackParticlesColor = glm::vec3{0.4258f, 0.84f, 0.68f};
            janitorBroom->attackParticlesSize = 0.3f;
            janitorBroom->dieGarbageType = GarbageType::ENEMY_GARBAGE2;

            janitorBroom->damage = 1.0f;
            janitorBroom->attackDistance = 80.0f + Beryll::RandomGenerator::getFloat() * 120.0f;
            janitorBroom->timeBetweenAttacks = 2.0f + Beryll::RandomGenerator::getFloat() * 0.5f;

            janitorBroom->reducePlayerSpeedWhenTakeSmashDamage = 0.0f;
            janitorBroom->experienceWhenDie = 35;
            janitorBroom->getController().moveSpeed = 35.0f;

            m_animatedOrDynamicObjects.push_back(janitorBroom);
            m_allAnimatedEnemies.push_back(janitorBroom);
            m_animatedObjForShadowMap.push_back(janitorBroom);
        }

        for(int i = 0; i < 90; ++i)
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
            copShield->dieGarbageType = GarbageType::ENEMY_GARBAGE3;

            copShield->damage = 1.0f;
            copShield->attackDistance = 150.0f + Beryll::RandomGenerator::getFloat() * 200.0f;
            copShield->timeBetweenAttacks = 2.5f + Beryll::RandomGenerator::getFloat() * 0.5f;

            copShield->reducePlayerSpeedWhenTakeSmashDamage = 0.0f;
            copShield->experienceWhenDie = 40;
            copShield->getController().moveSpeed = 30.0f;

            m_animatedOrDynamicObjects.push_back(copShield);
            m_allAnimatedEnemies.push_back(copShield);
            m_animatedObjForShadowMap.push_back(copShield);
        }
    }

    void Map3::spawnEnemies()
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

            int meleeCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(meleeCount < 5 && enemy->unitType == UnitType::ENEMY_MELEE)
                {
                    enemy->isCanBeSpawned = true;
                    ++meleeCount;
                }
            }

            BR_INFO("Prepare wave 2. Max enemies: %d", meleeCount);
        }
        else if(m_prepareWave3 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave3Time)
        {
            m_prepareWave3 = false;

            int meleeCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(meleeCount < 15 && enemy->unitType == UnitType::ENEMY_MELEE)
                {
                    enemy->isCanBeSpawned = true;
                    ++meleeCount;
                }
            }

            BR_INFO("Prepare wave 3. Max enemies: %d", meleeCount);
        }
        else if(m_prepareWave4 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave4Time)
        {
            m_prepareWave4 = false;

            int meleeCount = 0;
            int gunCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(meleeCount < 30 && enemy->unitType == UnitType::ENEMY_MELEE)
                {
                    enemy->isCanBeSpawned = true;
                    ++meleeCount;
                }
                else if(gunCount < 10 && enemy->unitType == UnitType::ENEMY_GUN1)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunCount;
                }
            }

            BR_INFO("Prepare wave 4. Max enemies: %d", meleeCount + gunCount);
        }
        else if(m_prepareWave5 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave5Time)
        {
            m_prepareWave5 = false;

            int meleeCount = 0;
            int gunCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(meleeCount < 40 && enemy->unitType == UnitType::ENEMY_MELEE)
                {
                    enemy->isCanBeSpawned = true;
                    ++meleeCount;
                }
                else if(gunCount < 30 && enemy->unitType == UnitType::ENEMY_GUN1)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunCount;
                }
            }

            BR_INFO("Prepare wave 5. Max enemies: %d", meleeCount + gunCount);
        }
        else if(m_prepareWave6 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave6Time)
        {
            m_prepareWave6 = false;

            int meleeCount = 0;
            int gunCount = 0;
            int gunShieldCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(meleeCount < 50 && enemy->unitType == UnitType::ENEMY_MELEE)
                {
                    enemy->isCanBeSpawned = true;
                    ++meleeCount;
                }
                else if(gunCount < 50 && enemy->unitType == UnitType::ENEMY_GUN1)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunCount;
                }
                else if(gunShieldCount < 10 && enemy->unitType == UnitType::ENEMY_GUN_SHIELD)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunShieldCount;
                }
            }

            BR_INFO("Prepare wave 6. Max enemies: %d", meleeCount + gunCount + gunShieldCount);
        }
        else if(m_prepareWave7 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave7Time)
        {
            m_prepareWave7 = false;

            int meleeCount = 0;
            int gunCount = 0;
            int gunShieldCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(meleeCount < 60 && enemy->unitType == UnitType::ENEMY_MELEE)
                {
                    enemy->isCanBeSpawned = true;
                    ++meleeCount;
                }
                else if(gunCount < 60 && enemy->unitType == UnitType::ENEMY_GUN1)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunCount;
                }
                else if(gunShieldCount < 30 && enemy->unitType == UnitType::ENEMY_GUN_SHIELD)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunShieldCount;
                }
            }

            BR_INFO("Prepare wave 7. Max enemies: %d", meleeCount + gunCount + gunShieldCount);
        }
        else if(m_prepareWave8 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave8Time)
        {
            m_prepareWave8 = false;

            int meleeCount = 0;
            int gunCount = 0;
            int gunShieldCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(meleeCount < 70 && enemy->unitType == UnitType::ENEMY_MELEE)
                {
                    enemy->isCanBeSpawned = true;
                    ++meleeCount;
                }
                else if(gunCount < 70 && enemy->unitType == UnitType::ENEMY_GUN1)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunCount;
                }
                else if(gunShieldCount < 50 && enemy->unitType == UnitType::ENEMY_GUN_SHIELD)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunShieldCount;
                }
            }

            BR_INFO("Prepare wave 8. Max enemies: %d", meleeCount + gunCount + gunShieldCount);
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
        //int reSpawnedCount = 0;
        if(!m_pointsToSpawnEnemies.empty())
        {
            for(const auto& enemy : m_allAnimatedEnemies)
            {
                if((enemy->getIsEnabled() && glm::distance(m_player->getObj()->getOriginXZ(), enemy->getOriginXZ()) > EnumsAndVars::enemiesRespawnDistance) ||
                   (!enemy->getIsEnabledUpdate() && enemy->isCanBeSpawned))
                {
                    //++reSpawnedCount;
                    enemy->enableEnemy();
                    enemy->disableDraw();

                    const glm::ivec2 spawnPoint2D = m_pointsToSpawnEnemies[Beryll::RandomGenerator::getInt(m_pointsToSpawnEnemies.size() - 1)];
                    enemy->setPathArray(m_pathFinderEnemies.findPath(spawnPoint2D, m_playerClosestAllowedPos, 6), 1);
                    m_pathFinderEnemies.addBlockedPosition(enemy->getCurrentPointToMove2DInt());
                    enemy->setOrigin(enemy->getStartPointMoveFrom());
                }
            }
        }
        //BR_INFO("BaseEnemy::getActiveCount(): %d reSpawnedCount %d", BaseEnemy::getActiveCount(), reSpawnedCount);
    }
}
