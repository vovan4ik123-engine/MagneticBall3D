#include "Map1.h"
#include "EnumsAndVariables.h"
#include "enemies/CopWithPistol.h"
#include "enemies/CopWithPistolShield.h"
#include "enemies/CopWithGrenadeLauncher.h"
#include "enemies/Tank.h"

namespace MagneticBall3D
{
    Map1::Map1(std::shared_ptr<PlayStateGUILayer> gui) : BaseMap(std::move(gui))
    {
        // Specific for this map only.
        loadPlayer();
        loadEnv();
        //loadGarbage();
        loadEnemies();

        // Defined in base class. Common for all maps.
        loadShaders();
        handleCamera();

        m_minX = -810.0f;
        m_maxX = 810.0f;
        m_minZ = -810.0f;
        m_maxZ = 810.0f;
        m_pathFinder = AStar(m_minX, m_maxX, m_minZ, m_maxZ, 30);
//        std::vector<glm::vec3> walls = BeryllUtils::Common::loadMeshVerticesToVector("models3D/map1/PathWalls.fbx");
//        for(const auto& wall : walls)
//        {
//            m_pathFinder.addWallPosition({(int)std::roundf(wall.x), (int)std::roundf(wall.z)});
//        }
//
//        BR_INFO("Map1 pathfinder walls.size() %d", walls.size());

        std::vector<glm::vec3> allowedPoints = BeryllUtils::Common::loadMeshVerticesToVector("models3D/map1/PathAllowedPositions.fbx");
        m_pathAllowedPositionsXZ.reserve(allowedPoints.size());
        for(const auto& point : allowedPoints)
        {
            m_pathAllowedPositionsXZ.push_back({(int)std::roundf(point.x), (int)std::roundf(point.z)});
        }

        BR_INFO("Map1 pathfinder m_pathGridXZ.size() %d", m_pathAllowedPositionsXZ.size());
        m_allowedPointsToSpawnEnemies.reserve(m_pathAllowedPositionsXZ.size());
        m_allowedPointsToSpawnGarbage.reserve(m_pathAllowedPositionsXZ.size());

        m_dirToSun = glm::normalize(glm::vec3(-1.0f, 3.5f, -0.8f));
        m_sunLightDir = -m_dirToSun;
        m_sunDistance = 600.0f;

        m_improvements = Improvements(m_player, {{ImprovementType::PLAYER_SIZE, 5}});

        m_skyBox = Beryll::Renderer::createSkyBox("skyboxes/map1");

        m_mapStartTimeSec = Beryll::TimeStep::getSecFromStart();

        //BR_INFO(" %f", );
        //BR_INFO("%s", "");
    }

    Map1::~Map1()
    {

    }

    void Map1::loadPlayer()
    {
        auto playerAllBalls = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/player/Player6Models.fbx",
                                                                                EnumsAndVariables::playerMass,
                                                                                true,
                                                                                Beryll::CollisionFlags::DYNAMIC,
                                                                                Beryll::CollisionGroups::PLAYER,
                                                                                Beryll::CollisionGroups::GROUND | Beryll::CollisionGroups::BUILDING |
                                                                                Beryll::CollisionGroups::GARBAGE | Beryll::CollisionGroups::ENEMY_ATTACK |
                                                                                Beryll::CollisionGroups::JUMPPAD,
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

            m_animatedOrDynamicObjects.push_back(item);
            m_simpleObjForShadowMap.push_back(item);
        }

        m_player = std::make_shared<Player>(playerAllBalls[0], 1000);
        m_player->setAllModels(playerAllBalls);

        //m_player->setOrigin(glm::vec3(-140.0f, 5.0f,-140.0f));
        m_player->getObj()->setGravity(EnumsAndVariables::playerGravityOnGround);
        m_player->getObj()->setFriction(EnumsAndVariables::playerFriction);
        m_player->getObj()->setDamping(EnumsAndVariables::playerLinearDamping, EnumsAndVariables::playerAngularDamping);
    }

    void Map1::loadEnv()
    {
        const auto ground = std::make_shared<Beryll::SimpleCollidingObject>("models3D/map1/Ground.fbx",
                                                                            0.0f,
                                                                            false,
                                                                            Beryll::CollisionFlags::STATIC,
                                                                            Beryll::CollisionGroups::GROUND,
                                                                            Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE,
                                                                            Beryll::SceneObjectGroups::GROUND);

        m_staticEnv.push_back(ground);
        ground->setFriction(EnumsAndVariables::staticEnvFriction);

//        const auto objects1 = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map1/Buildings.fbx",
//                                                                                       0.0f,
//                                                                                       false,
//                                                                                       Beryll::CollisionFlags::STATIC,
//                                                                                       Beryll::CollisionGroups::BUILDING,
//                                                                                       Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE |
//                                                                                       Beryll::CollisionGroups::RAY_FOR_BUILDING_CHECK  | Beryll::CollisionGroups::ENEMY_ATTACK,
//                                                                                       Beryll::SceneObjectGroups::BUILDING);
//
//        for(const auto& obj : objects1)
//        {
//            m_staticEnv.push_back(obj);
//            m_simpleObjForShadowMap.push_back(obj);
//            obj->setFriction(EnumsAndVariables::staticEnvFriction);
//        }

        const auto envColliders1 = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map1/EnvColliders.fbx",
                                                                                            0.0f,
                                                                                            false,
                                                                                            Beryll::CollisionFlags::STATIC,
                                                                                            Beryll::CollisionGroups::BUILDING,
                                                                                            Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE |
                                                                                            Beryll::CollisionGroups::RAY_FOR_BUILDING_CHECK  | Beryll::CollisionGroups::ENEMY_ATTACK,
                                                                                            Beryll::SceneObjectGroups::BUILDING);

        for(const auto& obj : envColliders1)
        {
            m_staticEnv.push_back(obj);
            m_simpleObjForShadowMap.push_back(obj);
            obj->setFriction(EnumsAndVariables::staticEnvFriction);
        }

        const auto jumpPads = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map1/JumpPads.fbx",
                                                                                       0.0f,
                                                                                       false,
                                                                                       Beryll::CollisionFlags::STATIC,
                                                                                       Beryll::CollisionGroups::JUMPPAD,
                                                                                       Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE,
                                                                                       Beryll::SceneObjectGroups::JUMPPAD);

        for(const auto& obj : jumpPads)
        {
            m_staticEnv.push_back(obj);
        }

        const auto envNoColliders1 = Beryll::SimpleObject::loadManyModelsFromOneFile("models3D/map1/EnvNoColliders.fbx", Beryll::SceneObjectGroups::BUILDING);

        for(const auto& obj : envNoColliders1)
        {
            m_staticEnv.push_back(obj);
        }
    }

    void Map1::loadGarbage()
    {
        const auto objects1 = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/prototypeMap/Garbage3.fbx",
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
            m_allGarbage.emplace_back(obj, GarbageType::COMMON, 5);
            m_allGarbage.back().disableGarbage();

            m_animatedOrDynamicObjects.push_back(obj);
            m_simpleObjForShadowMap.push_back(obj);

            obj->setDamping(EnumsAndVariables::garbageDamping, EnumsAndVariables::garbageDamping);
            obj->setGravity(EnumsAndVariables::garbageGravityDefault, false, false);

        }

        const auto objects2 = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/prototypeMap/Garbage2.fbx",
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
            m_allGarbage.emplace_back(obj, GarbageType::COMMON, 5);
            m_allGarbage.back().disableGarbage();

            m_animatedOrDynamicObjects.push_back(obj);
            m_simpleObjForShadowMap.push_back(obj);

            obj->setDamping(EnumsAndVariables::garbageDamping, EnumsAndVariables::garbageDamping);
            obj->setGravity(EnumsAndVariables::garbageGravityDefault, false, false);
        }

        const auto objects3 = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/prototypeMap/Garbage1.fbx",
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
            m_allGarbage.emplace_back(obj, GarbageType::COMMON, 5);
            m_allGarbage.back().disableGarbage();

            m_animatedOrDynamicObjects.push_back(obj);
            m_simpleObjForShadowMap.push_back(obj);

            obj->setDamping(EnumsAndVariables::garbageDamping, EnumsAndVariables::garbageDamping);
            obj->setGravity(EnumsAndVariables::garbageGravityDefault, false, false);
        }
    }

    void Map1::loadEnemies()
    {
        for(int i = 0; i < 200; ++i)
        {
            auto unit = std::make_shared<CopWithPistol>("models3D/enemies/CopWithPistol.fbx",
                                                        0.0f,
                                                        false,
                                                        Beryll::CollisionFlags::STATIC,
                                                        Beryll::CollisionGroups::NONE,
                                                        Beryll::CollisionGroups::NONE,
                                                        Beryll::SceneObjectGroups::ENEMY);

            unit->setCurrentAnimationByIndex(EnumsAndVariables::AnimationIndexes::run, false, false);
            unit->setDefaultAnimationByIndex(EnumsAndVariables::AnimationIndexes::stand);
            unit->getController().moveSpeed = 25.0f;

            m_animatedOrDynamicObjects.push_back(unit);
            m_allAnimatedEnemies.push_back(unit);
            m_animatedObjForShadowMap.push_back(unit);
        }

        for(int i = 0; i < 200; ++i)
        {
            auto unit3 = std::make_shared<CopWithPistolShield>("models3D/enemies/CopWithPistolShield.fbx",
                                                               0.0f,
                                                               false,
                                                               Beryll::CollisionFlags::STATIC,
                                                               Beryll::CollisionGroups::NONE,
                                                               Beryll::CollisionGroups::NONE,
                                                               Beryll::SceneObjectGroups::ENEMY);

            unit3->setCurrentAnimationByIndex(EnumsAndVariables::AnimationIndexes::run, false, false);
            unit3->setDefaultAnimationByIndex(EnumsAndVariables::AnimationIndexes::stand);
            unit3->getController().moveSpeed = 17.0f;

            m_animatedOrDynamicObjects.push_back(unit3);
            m_allAnimatedEnemies.push_back(unit3);
            m_animatedObjForShadowMap.push_back(unit3);
        }

        for(int i = 0; i < 200; ++i)
        {
            auto unit2 = std::make_shared<CopWithGrenadeLauncher>("models3D/enemies/CopWithGrenadeLauncher.fbx",
                                                                  0.0f,
                                                                  false,
                                                                  Beryll::CollisionFlags::STATIC,
                                                                  Beryll::CollisionGroups::NONE,
                                                                  Beryll::CollisionGroups::NONE,
                                                                  Beryll::SceneObjectGroups::ENEMY);

            unit2->setCurrentAnimationByIndex(EnumsAndVariables::AnimationIndexes::run, false, false);
            unit2->setDefaultAnimationByIndex(EnumsAndVariables::AnimationIndexes::stand);
            unit2->getController().moveSpeed = 20.0f;

            m_animatedOrDynamicObjects.push_back(unit2);
            m_allAnimatedEnemies.push_back(unit2);
            m_animatedObjForShadowMap.push_back(unit2);
        }

        for(int i = 0; i < 200; ++i)
        {
            auto tank = std::make_shared<Tank>("models3D/enemies/Tank.fbx",
                                                                  0.0f,
                                                                  false,
                                                                  Beryll::CollisionFlags::STATIC,
                                                                  Beryll::CollisionGroups::NONE,
                                                                  Beryll::CollisionGroups::NONE,
                                                                  Beryll::SceneObjectGroups::ENEMY);

            tank->setCurrentAnimationByIndex(EnumsAndVariables::AnimationIndexes::run, false, false);
            tank->setDefaultAnimationByIndex(EnumsAndVariables::AnimationIndexes::stand);
            tank->getController().moveSpeed = 20.0f;

            m_animatedOrDynamicObjects.push_back(tank);
            m_allAnimatedEnemies.push_back(tank);
            m_animatedObjForShadowMap.push_back(tank);
        }
    }

    void Map1::spawnEnemies()
    {
        // Prepare waves.
        if(m_prepareWave1 && m_mapPlayTimeSec > m_enemiesWave1Time)
        {
            BR_INFO("%s", "prepare wave 1");
            m_prepareWave1 = false;

            // Reset spawn for all.
            for(auto& enemy : m_allAnimatedEnemies)
                enemy->isCanBeSpawned = false;

            EnumsAndVariables::maxActiveEnemiesCountOnGround = 0;

            int copWithPistolCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                if(copWithPistolCount < 50 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolCount;
                    ++EnumsAndVariables::maxActiveEnemiesCountOnGround;
                }
            }
        }
        else if(m_prepareWave2 && m_mapPlayTimeSec > m_enemiesWave2Time)
        {
            BR_INFO("%s", "prepare wave 2");
            m_prepareWave2 = false;

            // Reset spawn for all.
            for(auto& enemy : m_allAnimatedEnemies)
                enemy->isCanBeSpawned = false;

            EnumsAndVariables::maxActiveEnemiesCountOnGround = 0;

            int copWithPistolCount = 0;
            int copWithPistolShieldCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                if(copWithPistolCount < 100 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolCount;
                    ++EnumsAndVariables::maxActiveEnemiesCountOnGround;
                }
                else if(copWithPistolShieldCount < 50 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL_SHIELD)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolShieldCount;
                    ++EnumsAndVariables::maxActiveEnemiesCountOnGround;
                }
            }
        }
        else if(m_prepareWave3 && m_mapPlayTimeSec > m_enemiesWave3Time)
        {
            BR_INFO("%s", "prepare wave 3");
            m_prepareWave3 = false;

            // Reset spawn for all.
            for(auto& enemy : m_allAnimatedEnemies)
                enemy->isCanBeSpawned = false;

            EnumsAndVariables::maxActiveEnemiesCountOnGround = 0;

            int copWithPistolCount = 0;
            int copWithPistolShieldCount = 0;
            int copWithGrenadeLauncherCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                if(copWithPistolCount < 100 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolCount;
                    ++EnumsAndVariables::maxActiveEnemiesCountOnGround;
                }
                else if(copWithPistolShieldCount < 100 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL_SHIELD)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolShieldCount;
                    ++EnumsAndVariables::maxActiveEnemiesCountOnGround;
                }
                else if(copWithGrenadeLauncherCount < 50 && enemy->getUnitType() == UnitType::COP_WITH_GRENADE_LAUNCHER)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithGrenadeLauncherCount;
                    ++EnumsAndVariables::maxActiveEnemiesCountOnGround;
                }
            }
        }
        else if(m_prepareWave4 && m_mapPlayTimeSec > m_enemiesWave4Time)
        {
            BR_INFO("%s", "prepare wave 4");
            m_prepareWave4 = false;

            // Reset spawn for all.
            for(auto& enemy : m_allAnimatedEnemies)
                enemy->isCanBeSpawned = false;

            EnumsAndVariables::maxActiveEnemiesCountOnGround = 0;

            int copWithPistolCount = 0;
            int copWithPistolShieldCount = 0;
            int copWithGrenadeLauncherCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                if(copWithPistolCount < 150 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolCount;
                    ++EnumsAndVariables::maxActiveEnemiesCountOnGround;
                }
                else if(copWithPistolShieldCount < 150 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL_SHIELD)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolShieldCount;
                    ++EnumsAndVariables::maxActiveEnemiesCountOnGround;
                }
                else if(copWithGrenadeLauncherCount < 150 && enemy->getUnitType() == UnitType::COP_WITH_GRENADE_LAUNCHER)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithGrenadeLauncherCount;
                    ++EnumsAndVariables::maxActiveEnemiesCountOnGround;
                }
            }
        }
        else if(m_prepareWave5 && m_mapPlayTimeSec > m_enemiesWave5Time)
        {
            BR_INFO("%s", "prepare wave 5");
            m_prepareWave5 = false;

            // Reset spawn for all.
            for(auto& enemy : m_allAnimatedEnemies)
                enemy->isCanBeSpawned = false;

            EnumsAndVariables::maxActiveEnemiesCountOnGround = 0;

            int tankCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                if(tankCount < 200 && enemy->getUnitType() == UnitType::TANK)
                {
                    enemy->isCanBeSpawned = true;
                    ++tankCount;
                    ++EnumsAndVariables::maxActiveEnemiesCountOnGround;
                }
            }
        }

        //BR_INFO("maxActiveEnemiesCountOnGround: %d BaseEnemy::getActiveCount(): %d", EnumsAndVariables::maxActiveEnemiesCountOnGround, BaseEnemy::getActiveCount());

        // Spawn enemies on ground.
        int countToSpawn = EnumsAndVariables::maxActiveEnemiesCountOnGround - BaseEnemy::getActiveCount();
        int spawnedCount = 0;
        if(countToSpawn > 0 && !m_allowedPointsToSpawnEnemies.empty())
        {
            for(const auto& enemy : m_allAnimatedEnemies)
            {
                if(!enemy->getIsEnabledUpdate() && enemy->isCanBeSpawned)
                {
                    enemy->enableEnemy();
                    enemy->disableDraw();

                    const glm::ivec2& spawnPoint2D = m_allowedPointsToSpawnEnemies[Beryll::RandomGenerator::getInt(m_allowedPointsToSpawnEnemies.size() - 1)];
                    glm::vec3 spawnPoint3D{spawnPoint2D.x,
                                           enemy->getFromOriginToBottom(),
                                           spawnPoint2D.y};
                    enemy->setOrigin(spawnPoint3D);

                    enemy->pathArray = m_pathFinder.findPath(spawnPoint2D, m_playerClosestAllowedPos, 6);
                    if(enemy->pathArray.size() > 1)
                        enemy->indexInPathArray = 1;
                    else
                        enemy->indexInPathArray = 0;

                    enemy->currentPointToMove2DIntegers = enemy->pathArray[enemy->indexInPathArray];
                    enemy->currentPointToMove3DFloats = glm::vec3(enemy->currentPointToMove2DIntegers.x,
                                                                  enemy->getFromOriginToBottom(),
                                                                  enemy->currentPointToMove2DIntegers.y);

                    ++spawnedCount;

                    if(spawnedCount >= countToSpawn)
                        break;
                }
            }
        }

        // Spawn snipers on buildings.
    }
}
