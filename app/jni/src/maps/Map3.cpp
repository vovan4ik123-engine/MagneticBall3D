#include "Map3.h"
#include "EnumsAndVariables.h"
#include "enemies/MovableEnemy.h"
#include "enemies/StaticEnemy.h"
#include "Sounds.h"

namespace MagneticBall3D
{
    Map3::Map3(std::shared_ptr<PlayStateGUILayer> gui) : BaseMap(std::move(gui))
    {
        Beryll::LoadingScreen::showProgress(10.0f);

        // Allocate enough spase for all vectors to avoid vector reallocation.
        const int maxGarbageCount = 400;
        m_allGarbage.reserve(maxGarbageCount);
        m_allAnimatedEnemies.reserve(500);
        m_animatedOrDynamicObjects.reserve(500 + maxGarbageCount);
        m_staticEnv.reserve(300);
        m_simpleObjForShadowMap.reserve(300 + maxGarbageCount);
        m_animatedObjForShadowMap.reserve(400);

        // Specific for this map only.
        loadPlayer();
        m_player->getObj()->setOrigin(glm::vec3(-490.0f, 2.0f,-633.0f));
        m_improvements.setPlayer(m_player);
        Beryll::LoadingScreen::showProgress(20.0f);
        loadEnv();
        Beryll::LoadingScreen::showProgress(40.0f);
        loadGarbage();
        BR_ASSERT((m_allGarbage.size() < maxGarbageCount), "%s", "m_allGarbage reallocation happened. Increase maxGarbageCount.");
        Beryll::LoadingScreen::showProgress(60.0f);
        loadEnemies();
        Beryll::LoadingScreen::showProgress(80.0f);
        loadBoss();
        Beryll::LoadingScreen::showProgress(90.0f);

        loadShaders();
        m_cameraAngleOffset = glm::normalize(glm::vec3(-1.0f, 0.0f, -0.8f));
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

        m_skyBox = Beryll::Renderer::createSkyBox("skyboxes/map1");

        if(EnumsAndVars::playerMagneticRadius < 50)
            EnumsAndVars::playerMagneticRadius = 50.0f;
        if(EnumsAndVars::garbageMaxCountMagnetized < 80.0f)
            EnumsAndVars::garbageMaxCountMagnetized = 80.0f;
        if(EnumsAndVars::playerImpulseFactorOnGround < 0.12f)
            EnumsAndVars::playerImpulseFactorOnGround = 0.12f;
        if(EnumsAndVars::playerTorqueFactorOnGround < 0.11f)
            EnumsAndVars::playerTorqueFactorOnGround = 0.11f;
        if(EnumsAndVars::playerTorqueFactorOnBuildingWall < 0.38f)
            EnumsAndVars::playerTorqueFactorOnBuildingWall = 0.38f;

        EnumsAndVars::garbageCommonSpawnCount = 3;

        SendStatisticsHelper::sendMapStart();

        Beryll::LoadingScreen::showProgress(100.0f);

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
        glm::mat4 modelMatrix{1.0f};

        if(EnumsAndVars::gameOnPause || EnumsAndVars::improvementSystemOnScreen)
            m_ambientLight = 0.25f;
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
                                                                                Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE,
                                                                                Beryll::SceneObjectGroups::GROUND);

        m_objWithNormalMap.push_back(mainGround);
        mainGround->setFriction(EnumsAndVars::staticEnvFriction);

        const auto envNoCollidersNormalMap = Beryll::SimpleObject::loadManyModelsFromOneFile("models3D/map3/EnvNoCollidersNormalMap.fbx", Beryll::SceneObjectGroups::BUILDING);

        for(const auto& obj : envNoCollidersNormalMap)
        {
            m_objWithNormalMap.push_back(obj);
        }

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
    }

    void Map3::loadGarbage()
    {
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
                m_allGarbage.emplace_back(obj, GarbageType::ENEMY_MELEE, EnumsAndVars::garbageStartHP);

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
                m_allGarbage.emplace_back(obj, GarbageType::ENEMY_GUN, EnumsAndVars::garbageStartHP);

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
                m_allGarbage.emplace_back(obj, GarbageType::ENEMY_GUN_SHIELD, EnumsAndVars::garbageStartHP);

                m_animatedOrDynamicObjects.push_back(obj);
                m_simpleObjForShadowMap.push_back(obj);

                obj->setDamping(EnumsAndVars::garbageLinearDamping, EnumsAndVars::garbageAngularDamping);
                obj->setGravity(EnumsAndVars::garbageGravityDefault, false, false);
            }
        }
    }

    void Map3::loadEnemies()
    {
        for(int i = 0; i < 110; ++i)
        {
            auto janitorRake = std::make_shared<MovableEnemy>("models3D/enemies/JanitorRake.fbx",
                                                               0.0f,
                                                               false,
                                                               Beryll::CollisionFlags::STATIC,
                                                               Beryll::CollisionGroups::NONE,
                                                               Beryll::CollisionGroups::NONE,
                                                               Beryll::SceneObjectGroups::ENEMY);

            janitorRake->setCurrentAnimationByIndex(EnumsAndVars::AnimationIndexes::run, false, false);
            janitorRake->setDefaultAnimationByIndex(EnumsAndVars::AnimationIndexes::stand);
            janitorRake->unitType = UnitType::ENEMY_MELEE;
            janitorRake->attackType = AttackType::MELEE_DAMAGE_ONE;
            janitorRake->attackSound = SoundType::NONE;
            janitorRake->attackHitSound = SoundType::STICK_HIT;
            janitorRake->dieSound = SoundType::POP;
            janitorRake->dieGarbageType = GarbageType::ENEMY_MELEE;

            janitorRake->damage = 0.5f;
            janitorRake->attackDistance = 45.0f + Beryll::RandomGenerator::getFloat() * 5.0f;
            janitorRake->timeBetweenAttacks = 1.5f + Beryll::RandomGenerator::getFloat() * 0.2f;

            janitorRake->garbageAmountToDie = 10;
            janitorRake->reducePlayerSpeedWhenDie = 0.5f;
            janitorRake->experienceWhenDie = 25;
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
                                                               Beryll::SceneObjectGroups::ENEMY);

            janitorBroom->setCurrentAnimationByIndex(EnumsAndVars::AnimationIndexes::run, false, false);
            janitorBroom->setDefaultAnimationByIndex(EnumsAndVars::AnimationIndexes::stand);
            janitorBroom->unitType = UnitType::ENEMY_GUN;
            janitorBroom->attackType = AttackType::RANGE_DAMAGE_ONE;
            janitorBroom->attackSound = SoundType::NONE;
            janitorBroom->attackHitSound = SoundType::BROOM_HIT;
            janitorBroom->dieSound = SoundType::POP;
            janitorBroom->dieGarbageType = GarbageType::ENEMY_GUN;

            janitorBroom->damage = 0.5f;
            janitorBroom->attackDistance = 80.0f + Beryll::RandomGenerator::getFloat() * 100.0f;
            janitorBroom->timeBetweenAttacks = 2.0f + Beryll::RandomGenerator::getFloat() * 0.2f;

            janitorBroom->garbageAmountToDie = 10;
            janitorBroom->reducePlayerSpeedWhenDie = 0.5f;
            janitorBroom->experienceWhenDie = 25;
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
                                                            Beryll::SceneObjectGroups::ENEMY);

            copShield->setCurrentAnimationByIndex(EnumsAndVars::AnimationIndexes::run, false, false);
            copShield->setDefaultAnimationByIndex(EnumsAndVars::AnimationIndexes::stand);
            copShield->unitType = UnitType::ENEMY_GUN_SHIELD;
            copShield->attackType = AttackType::RANGE_DAMAGE_ONE;
            copShield->attackSound = SoundType::PISTOL_SHOT;
            copShield->attackHitSound = SoundType::PISTOL_HIT;
            copShield->dieSound = SoundType::POP;
            copShield->dieGarbageType = GarbageType::ENEMY_GUN_SHIELD;

            copShield->damage = 0.5f;
            copShield->attackDistance = 150.0f + Beryll::RandomGenerator::getFloat() * 50.0f;
            copShield->timeBetweenAttacks = 2.5f + Beryll::RandomGenerator::getFloat() * 0.2f;

            copShield->garbageAmountToDie = 10;
            copShield->reducePlayerSpeedWhenDie = 0.5f;
            copShield->experienceWhenDie = 30;
            copShield->getController().moveSpeed = 30.0f;

            m_animatedOrDynamicObjects.push_back(copShield);
            m_allAnimatedEnemies.push_back(copShield);
            m_animatedObjForShadowMap.push_back(copShield);
        }
    }

    void Map3::loadBoss()
    {

    }

    void Map3::spawnEnemies()
    {
        // Prepare waves.
        if(m_prepareWave1 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave1Time)
        {
            m_prepareWave1 = false;

            EnumsAndVars::enemiesMaxActiveCountOnGround = 0;

            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;
            }

            BR_INFO("Prepare wave 1. Max enemies: %d", EnumsAndVars::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave2 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave2Time)
        {
            m_prepareWave2 = false;

            EnumsAndVars::enemiesMaxActiveCountOnGround = 0;

            int meleeCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(meleeCount < 5 && enemy->unitType == UnitType::ENEMY_MELEE)
                {
                    enemy->isCanBeSpawned = true;
                    ++meleeCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("Prepare wave 2. Max enemies: %d", EnumsAndVars::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave3 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave3Time)
        {
            m_prepareWave3 = false;

            EnumsAndVars::enemiesMaxActiveCountOnGround = 0;

            int meleeCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(meleeCount < 15 && enemy->unitType == UnitType::ENEMY_MELEE)
                {
                    enemy->isCanBeSpawned = true;
                    ++meleeCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("Prepare wave 3. Max enemies: %d", EnumsAndVars::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave4 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave4Time)
        {
            m_prepareWave4 = false;

            EnumsAndVars::enemiesMaxActiveCountOnGround = 0;

            int meleeCount = 0;
            int gunCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(meleeCount < 30 && enemy->unitType == UnitType::ENEMY_MELEE)
                {
                    enemy->isCanBeSpawned = true;
                    ++meleeCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(gunCount < 10 && enemy->unitType == UnitType::ENEMY_GUN)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("Prepare wave 4. Max enemies: %d", EnumsAndVars::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave5 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave5Time)
        {
            m_prepareWave5 = false;

            EnumsAndVars::enemiesMaxActiveCountOnGround = 0;

            int meleeCount = 0;
            int gunCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(meleeCount < 45 && enemy->unitType == UnitType::ENEMY_MELEE)
                {
                    enemy->isCanBeSpawned = true;
                    ++meleeCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(gunCount < 30 && enemy->unitType == UnitType::ENEMY_GUN)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("Prepare wave 5. Max enemies: %d", EnumsAndVars::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave6 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave6Time)
        {
            m_prepareWave6 = false;

            EnumsAndVars::enemiesMaxActiveCountOnGround = 0;

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
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(gunCount < 50 && enemy->unitType == UnitType::ENEMY_GUN)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(gunShieldCount < 10 && enemy->unitType == UnitType::ENEMY_GUN_SHIELD)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunShieldCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("Prepare wave 6. Max enemies: %d", EnumsAndVars::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave7 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave7Time)
        {
            m_prepareWave7 = false;

            EnumsAndVars::enemiesMaxActiveCountOnGround = 0;

            int meleeCount = 0;
            int gunCount = 0;
            int gunShieldCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(meleeCount < 75 && enemy->unitType == UnitType::ENEMY_MELEE)
                {
                    enemy->isCanBeSpawned = true;
                    ++meleeCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(gunCount < 70 && enemy->unitType == UnitType::ENEMY_GUN)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(gunShieldCount < 40 && enemy->unitType == UnitType::ENEMY_GUN_SHIELD)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunShieldCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("Prepare wave 7. Max enemies: %d", EnumsAndVars::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave8 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave8Time)
        {
            m_prepareWave8 = false;

            EnumsAndVars::enemiesMaxActiveCountOnGround = 0;

            int meleeCount = 0;
            int gunCount = 0;
            int gunShieldCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(meleeCount < 90 && enemy->unitType == UnitType::ENEMY_MELEE)
                {
                    enemy->isCanBeSpawned = true;
                    ++meleeCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(gunCount < 85 && enemy->unitType == UnitType::ENEMY_GUN)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(gunShieldCount < 70 && enemy->unitType == UnitType::ENEMY_GUN_SHIELD)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunShieldCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("Prepare wave 8. Max enemies: %d", EnumsAndVars::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave9 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave9Time)
        {
            m_prepareWave9 = false;

            EnumsAndVars::enemiesMaxActiveCountOnGround = 0;

            int meleeCount = 0;
            int gunCount = 0;
            int gunShieldCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(meleeCount < 110 && enemy->unitType == UnitType::ENEMY_MELEE)
                {
                    enemy->isCanBeSpawned = true;
                    ++meleeCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(gunCount < 100 && enemy->unitType == UnitType::ENEMY_GUN)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(gunShieldCount < 90 && enemy->unitType == UnitType::ENEMY_GUN_SHIELD)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunShieldCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("Prepare wave 9. Max enemies: %d", EnumsAndVars::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareLastWave && EnumsAndVars::mapPlayTimeSec > m_prepareLastWaveTime)
        {
            m_prepareLastWave = false;

            EnumsAndVars::enemiesMaxActiveCountOnGround = 0;

            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;
            }

            BR_INFO("enemiesLastWavePhase(). Max enemies: %d", EnumsAndVars::enemiesMaxActiveCountOnGround);

            lastWaveToWinPhase();
        }

        // Spawn enemies.
        if(!m_pointsToSpawnEnemies.empty())
        {
            for(const auto& enemy : m_allAnimatedEnemies)
            {
                if(BaseEnemy::getActiveCount() >= EnumsAndVars::enemiesMaxActiveCountOnGround)
                    break;

                // Enemy already spawned or can not be spawned.
                if(enemy->getIsEnabledUpdate() || !enemy->isCanBeSpawned)
                    continue;

                enemy->enableEnemy();
                enemy->disableDraw();

                const glm::ivec2 spawnPoint2D = m_pointsToSpawnEnemies[Beryll::RandomGenerator::getInt(m_pointsToSpawnEnemies.size() - 1)];

                enemy->setPathArray(m_pathFinderEnemies.findPath(spawnPoint2D, m_playerClosestAllowedPos, 6), 1);

                enemy->setOrigin(enemy->getStartPointMoveFrom());
            }
        }

        //BR_INFO("BaseEnemy::getActiveCount(): %d", BaseEnemy::getActiveCount());
    }

    void Map3::startBossPhase()
    {

    }

    void Map3::handlePossPhase()
    {

    }
}
