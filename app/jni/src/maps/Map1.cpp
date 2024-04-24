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

        m_improvements = Improvements(m_player, {{ImprovementType::PLAYER_MAX_SPEED, 5},
                                                            {ImprovementType::PLAYER_ACCELERATE_FASTER_ON_GROUND, 5},
                                                            {ImprovementType::PLAYER_MOVE_FASTER_THROUGH_ENEMIES, 5},
                                                            {ImprovementType::PLAYER_BETTER_CLUTCH_WITH_BUILDINGS, 5},
                                                            {ImprovementType::PLAYER_INCREASE_SIZE, 5},
                                                            {ImprovementType::PLAYER_INCREASE_MAX_HP, 5},
                                                            {ImprovementType::PLAYER_REDUCE_DAMAGE, 5},
                                                            {ImprovementType::PLAYER_TAKE_MORE_XP, 5},
                                                            {ImprovementType::PLAYER_HEAL_AT_NEW_LVL, 5},
                                                            {ImprovementType::GARBAGE_SPAWN_MORE_ON_MAP, 5},
                                                            {ImprovementType::GARBAGE_REDUCE_DAMAGE, 5}});

        m_skyBox = Beryll::Renderer::createSkyBox("skyboxes/map1");

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

        m_player = std::make_shared<Player>(playerAllBalls[0], 200);
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
            m_allGarbage.emplace_back(obj, GarbageType::COMMON, 10);
            m_allGarbage.back().disableGarbage();

            m_animatedOrDynamicObjects.push_back(obj);
            m_simpleObjForShadowMap.push_back(obj);

            obj->setDamping(EnumsAndVariables::garbageDamping, EnumsAndVariables::garbageDamping);
            obj->setGravity(EnumsAndVariables::garbageGravityDefault, false, false);

        }

//        const auto objects2 = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/prototypeMap/Garbage2.fbx",
//                                                                                       EnumsAndVariables::garbageMass,
//                                                                                       false,
//                                                                                       Beryll::CollisionFlags::DYNAMIC,
//                                                                                       Beryll::CollisionGroups::GARBAGE,
//                                                                                       Beryll::CollisionGroups::GROUND | Beryll::CollisionGroups::BUILDING |
//                                                                                       Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE |
//                                                                                       Beryll::CollisionGroups::ENEMY_ATTACK,
//                                                                                       Beryll::SceneObjectGroups::GARBAGE);
//
//        for(const auto& obj : objects2)
//        {
//            m_allGarbage.emplace_back(obj, GarbageType::COMMON, 10);
//            m_allGarbage.back().disableGarbage();
//
//            m_animatedOrDynamicObjects.push_back(obj);
//            m_simpleObjForShadowMap.push_back(obj);
//
//            obj->setDamping(EnumsAndVariables::garbageDamping, EnumsAndVariables::garbageDamping);
//            obj->setGravity(EnumsAndVariables::garbageGravityDefault, false, false);
//        }

//        const auto objects3 = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/prototypeMap/Garbage1.fbx",
//                                                                                       EnumsAndVariables::garbageMass,
//                                                                                       false,
//                                                                                       Beryll::CollisionFlags::DYNAMIC,
//                                                                                       Beryll::CollisionGroups::GARBAGE,
//                                                                                       Beryll::CollisionGroups::GROUND | Beryll::CollisionGroups::BUILDING |
//                                                                                       Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE |
//                                                                                       Beryll::CollisionGroups::ENEMY_ATTACK,
//                                                                                       Beryll::SceneObjectGroups::GARBAGE);
//
//        for(const auto& obj : objects3)
//        {
//            m_allGarbage.emplace_back(obj, GarbageType::COMMON, 10);
//            m_allGarbage.back().disableGarbage();
//
//            m_animatedOrDynamicObjects.push_back(obj);
//            m_simpleObjForShadowMap.push_back(obj);
//
//            obj->setDamping(EnumsAndVariables::garbageDamping, EnumsAndVariables::garbageDamping);
//            obj->setGravity(EnumsAndVariables::garbageGravityDefault, false, false);
//        }
    }

    void Map1::loadEnemies()
    {
        for(int i = 0; i < 150; ++i)
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

            m_animatedOrDynamicObjects.push_back(unit);
            m_allAnimatedEnemies.push_back(unit);
            m_animatedObjForShadowMap.push_back(unit);
        }

        for(int i = 0; i < 185; ++i)
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

            m_animatedOrDynamicObjects.push_back(unit3);
            m_allAnimatedEnemies.push_back(unit3);
            m_animatedObjForShadowMap.push_back(unit3);
        }

        for(int i = 0; i < 150; ++i)
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

            m_animatedOrDynamicObjects.push_back(unit2);
            m_allAnimatedEnemies.push_back(unit2);
            m_animatedObjForShadowMap.push_back(unit2);
        }

        for(int i = 0; i < 20; ++i)
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

            m_animatedOrDynamicObjects.push_back(tank);
            m_allAnimatedEnemies.push_back(tank);
            m_animatedObjForShadowMap.push_back(tank);
        }
    }

    void Map1::spawnEnemies()
    {
        // Prepare waves.
        if(m_prepareWave1 && EnumsAndVariables::mapPlayTimeSec > m_enemiesWave1Time)
        {
            m_prepareWave1 = false;

            EnumsAndVariables::enemiesMaxActiveCountOnGround = 0;

            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;
            }

            BR_INFO("prepare wave 1. Max enemies: %d", EnumsAndVariables::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave2 && EnumsAndVariables::mapPlayTimeSec > m_enemiesWave2Time)
        {
            m_prepareWave2 = false;

            EnumsAndVariables::enemiesMaxActiveCountOnGround = 0;

            int copWithPistolCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(copWithPistolCount < 50 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolCount;
                    ++EnumsAndVariables::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("prepare wave 2. Max enemies: %d", EnumsAndVariables::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave3 && EnumsAndVariables::mapPlayTimeSec > m_enemiesWave3Time)
        {
            m_prepareWave3 = false;

            EnumsAndVariables::enemiesMaxActiveCountOnGround = 0;

            int copWithPistolCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(copWithPistolCount < 100 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolCount;
                    ++EnumsAndVariables::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("prepare wave 3. Max enemies: %d", EnumsAndVariables::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave4 && EnumsAndVariables::mapPlayTimeSec > m_enemiesWave4Time)
        {
            m_prepareWave4 = false;

            EnumsAndVariables::enemiesMaxActiveCountOnGround = 0;

            int copWithPistolCount = 0;
            int copWithPistolShieldCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(copWithPistolCount < 100 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolCount;
                    ++EnumsAndVariables::enemiesMaxActiveCountOnGround;
                }
                else if(copWithPistolShieldCount < 25 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL_SHIELD)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolShieldCount;
                    ++EnumsAndVariables::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("prepare wave 4. Max enemies: %d", EnumsAndVariables::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave5 && EnumsAndVariables::mapPlayTimeSec > m_enemiesWave5Time)
        {
            m_prepareWave5 = false;

            EnumsAndVariables::enemiesMaxActiveCountOnGround = 0;

            int copWithPistolCount = 0;
            int copWithPistolShieldCount = 0;
            int sniperCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(copWithPistolCount < 125 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolCount;
                    ++EnumsAndVariables::enemiesMaxActiveCountOnGround;
                }
                else if(copWithPistolShieldCount < 75 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL_SHIELD)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolShieldCount;
                    ++EnumsAndVariables::enemiesMaxActiveCountOnGround;
                }
                else if(sniperCount < 4 && enemy->getUnitType() == UnitType::SNIPER)
                {
                    enemy->isCanBeSpawned = true;
                    ++sniperCount;
                    ++EnumsAndVariables::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("prepare wave 5. Max enemies: %d", EnumsAndVariables::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave6 && EnumsAndVariables::mapPlayTimeSec > m_enemiesWave6Time)
        {
            m_prepareWave6 = false;

            EnumsAndVariables::enemiesMaxActiveCountOnGround = 0;

            int copWithPistolCount = 0;
            int copWithPistolShieldCount = 0;
            int copWithGrenadeLauncherCount = 0;
            int sniperCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(copWithPistolCount < 150 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolCount;
                    ++EnumsAndVariables::enemiesMaxActiveCountOnGround;
                }
                else if(copWithPistolShieldCount < 75 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL_SHIELD)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolShieldCount;
                    ++EnumsAndVariables::enemiesMaxActiveCountOnGround;
                }
                else if(copWithGrenadeLauncherCount < 25 && enemy->getUnitType() == UnitType::COP_WITH_GRENADE_LAUNCHER)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithGrenadeLauncherCount;
                    ++EnumsAndVariables::enemiesMaxActiveCountOnGround;
                }
                else if(sniperCount < 8 && enemy->getUnitType() == UnitType::SNIPER)
                {
                    enemy->isCanBeSpawned = true;
                    ++sniperCount;
                    ++EnumsAndVariables::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("prepare wave 6. Max enemies: %d", EnumsAndVariables::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave7 && EnumsAndVariables::mapPlayTimeSec > m_enemiesWave7Time)
        {
            m_prepareWave7 = false;

            EnumsAndVariables::enemiesMaxActiveCountOnGround = 0;

            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;
            }

            BR_INFO("prepare wave 7. Max enemies: %d", EnumsAndVariables::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave8 && EnumsAndVariables::mapPlayTimeSec > m_enemiesWave8Time)
        {
            m_prepareWave8 = false;

            EnumsAndVariables::enemiesMaxActiveCountOnGround = 0;

            int copWithPistolCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(copWithPistolCount < 150 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolCount;
                    ++EnumsAndVariables::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("prepare wave 8. Max enemies: %d", EnumsAndVariables::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave9 && EnumsAndVariables::mapPlayTimeSec > m_enemiesWave9Time)
        {
            m_prepareWave9 = false;

            EnumsAndVariables::enemiesMaxActiveCountOnGround = 0;

            int copWithPistolCount = 0;
            int copWithPistolShieldCount = 0;
            int copWithGrenadeLauncherCount = 0;
            int sniperCount = 0;
            int tankCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(copWithPistolCount < 150 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolCount;
                    ++EnumsAndVariables::enemiesMaxActiveCountOnGround;
                }
                else if(copWithPistolShieldCount < 85 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL_SHIELD)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolShieldCount;
                    ++EnumsAndVariables::enemiesMaxActiveCountOnGround;
                }
                else if(copWithGrenadeLauncherCount < 50 && enemy->getUnitType() == UnitType::COP_WITH_GRENADE_LAUNCHER)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithGrenadeLauncherCount;
                    ++EnumsAndVariables::enemiesMaxActiveCountOnGround;
                }
                else if(sniperCount < 15 && enemy->getUnitType() == UnitType::SNIPER)
                {
                    enemy->isCanBeSpawned = true;
                    ++sniperCount;
                    ++EnumsAndVariables::enemiesMaxActiveCountOnGround;
                }
                else if(tankCount < 5 && enemy->getUnitType() == UnitType::TANK)
                {
                    enemy->isCanBeSpawned = true;
                    ++tankCount;
                    ++EnumsAndVariables::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("prepare wave 9. Max enemies: %d", EnumsAndVariables::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave10 && EnumsAndVariables::mapPlayTimeSec > m_enemiesWave10Time)
        {
            m_prepareWave10 = false;

            EnumsAndVariables::enemiesMaxActiveCountOnGround = 0;

            int copWithPistolCount = 0;
            int copWithPistolShieldCount = 0;
            int copWithGrenadeLauncherCount = 0;
            int sniperCount = 0;
            int tankCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(copWithPistolCount < 150 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolCount;
                    ++EnumsAndVariables::enemiesMaxActiveCountOnGround;
                }
                else if(copWithPistolShieldCount < 100 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL_SHIELD)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolShieldCount;
                    ++EnumsAndVariables::enemiesMaxActiveCountOnGround;
                }
                else if(copWithGrenadeLauncherCount < 100 && enemy->getUnitType() == UnitType::COP_WITH_GRENADE_LAUNCHER)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithGrenadeLauncherCount;
                    ++EnumsAndVariables::enemiesMaxActiveCountOnGround;
                }
                else if(sniperCount < 15 && enemy->getUnitType() == UnitType::SNIPER)
                {
                    enemy->isCanBeSpawned = true;
                    ++sniperCount;
                    ++EnumsAndVariables::enemiesMaxActiveCountOnGround;
                }
                else if(tankCount < 10 && enemy->getUnitType() == UnitType::TANK)
                {
                    enemy->isCanBeSpawned = true;
                    ++tankCount;
                    ++EnumsAndVariables::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("prepare wave 10. Max enemies: %d", EnumsAndVariables::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave11 && EnumsAndVariables::mapPlayTimeSec > m_enemiesWave11Time)
        {
            m_prepareWave11 = false;

            EnumsAndVariables::enemiesMaxActiveCountOnGround = 0;

            int copWithPistolCount = 0;
            int copWithPistolShieldCount = 0;
            int copWithGrenadeLauncherCount = 0;
            int sniperCount = 0;
            int tankCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(copWithPistolCount < 150 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolCount;
                    ++EnumsAndVariables::enemiesMaxActiveCountOnGround;
                }
                else if(copWithPistolShieldCount < 185 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL_SHIELD)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolShieldCount;
                    ++EnumsAndVariables::enemiesMaxActiveCountOnGround;
                }
                else if(copWithGrenadeLauncherCount < 150 && enemy->getUnitType() == UnitType::COP_WITH_GRENADE_LAUNCHER)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithGrenadeLauncherCount;
                    ++EnumsAndVariables::enemiesMaxActiveCountOnGround;
                }
                else if(sniperCount < 15 && enemy->getUnitType() == UnitType::SNIPER)
                {
                    enemy->isCanBeSpawned = true;
                    ++sniperCount;
                    ++EnumsAndVariables::enemiesMaxActiveCountOnGround;
                }
                else if(tankCount < 20 && enemy->getUnitType() == UnitType::TANK)
                {
                    enemy->isCanBeSpawned = true;
                    ++tankCount;
                    ++EnumsAndVariables::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("prepare wave 11. Max enemies: %d", EnumsAndVariables::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave12 && EnumsAndVariables::mapPlayTimeSec > m_enemiesWave12Time)
        {
            m_prepareWave12 = false;

            EnumsAndVariables::enemiesMaxActiveCountOnGround = 0;

            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;
            }

            BR_INFO("prepare wave 12. Max enemies: %d", EnumsAndVariables::enemiesMaxActiveCountOnGround);
        }

        // Spawn enemies on ground.
        int spawnedCount = 0;
        if(!m_allowedPointsToSpawnEnemies.empty())
        {
            for(const auto& enemy : m_allAnimatedEnemies)
            {
                if(BaseEnemy::getActiveCount() >= EnumsAndVariables::enemiesMaxActiveCountOnGround)
                    break;

                if(enemy->getIsEnabledUpdate() || !enemy->isCanBeSpawned)
                    continue;

                if(enemy->getUnitType() != UnitType::SNIPER)
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
                }
                else
                {
                    // Spawn snipers on buildings prepared positions.


                }
            }
        }

        if(spawnedCount > 0)
        {
            BR_INFO("spawnedCount: %d BaseEnemy::getActiveCount(): %d", spawnedCount, BaseEnemy::getActiveCount());
        }
    }
}
