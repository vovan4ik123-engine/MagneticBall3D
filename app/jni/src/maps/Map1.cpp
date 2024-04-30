#include "Map1.h"
#include "EnumsAndVariables.h"
#include "enemies/CopWithPistol.h"
#include "enemies/CopWithPistolShield.h"
#include "enemies/CopWithGrenadeLauncher.h"
#include "enemies/Tank.h"
#include "enemies/Sniper.h"

namespace MagneticBall3D
{
    Map1::Map1(std::shared_ptr<PlayStateGUILayer> gui) : BaseMap(std::move(gui))
    {
        // Specific for this map only.
        loadPlayer();
        loadEnv();
        loadGarbage();
        loadEnemies();

        Sniper::sniperPositions = std::vector<SniperPosAndRange>{{glm::vec3(79.0f, 260.0f, -644.0f), 2500.0f},
                                                                 {glm::vec3(68.0f, 257.0f, -448.0f), 2500.0f},
                                                                 {glm::vec3(65.0f, 219.0f, -265.0f), 2500.0f},
                                                                 {glm::vec3(62.0f, 255.0f, -122.0f), 2500.0f},
                                                                 {glm::vec3(77.0f, 255.0f, -53.0f), 2500.0f},
                                                                 {glm::vec3(127.0f, 255.0f, -55.0f), 2500.0f},
                                                                 {glm::vec3(270.0f, 220.0f, -62.5f), 2500.0f},
                                                                 {glm::vec3(313.0f, 220.0f, -63.0f), 2500.0f},
                                                                 {glm::vec3(460.0f, 248.0f, -59.0f), 2500.0f},
                                                                 {glm::vec3(644.5f, 260.5f, -64.0f), 2500.0f},
                                                                 {glm::vec3(64.0f, 309.5f, 429.0f), 2500.0f},
                                                                 {glm::vec3(64.0f, 309.5f, 490.0f), 2500.0f},
                                                                 {glm::vec3(176.0f, 309.5f, 403.5f), 2500.0f}
        };

        // Defined in base class. Common for all maps.
        loadShaders();
        handleCamera();

        m_minX = -800.0f;
        m_maxX = 800.0f;
        m_minZ = -800.0f;
        m_maxZ = 800.0f;
        m_pathFinder = AStar(m_minX, m_maxX, m_minZ, m_maxZ, 20);
        std::vector<glm::vec3> walls = BeryllUtils::Common::loadMeshVerticesToVector("models3D/map1/PathEnemiesWalls.fbx");
        for(const auto& wall : walls)
        {
            m_pathFinder.addWallPosition({(int)std::roundf(wall.x), (int)std::roundf(wall.z)});
        }

        BR_INFO("Map1 pathfinder walls: %d", walls.size());

        std::vector<glm::vec3> allowedPoints = BeryllUtils::Common::loadMeshVerticesToVector("models3D/map1/PathEnemiesAllowedPositions.fbx");
        m_pathAllowedPositionsXZ.reserve(allowedPoints.size());
        for(const auto& point : allowedPoints)
        {
            m_pathAllowedPositionsXZ.push_back({(int)std::roundf(point.x), (int)std::roundf(point.z)});
        }

        BR_INFO("Map1 pathfinder allowed points: %d", m_pathAllowedPositionsXZ.size());
        m_allowedPointsToSpawnEnemies.reserve(m_pathAllowedPositionsXZ.size());
        m_allowedPointsToSpawnGarbage.reserve(m_pathAllowedPositionsXZ.size());

        m_dirToSun = glm::normalize(glm::vec3(-1.0f, 3.5f, -0.8f));
        m_sunLightDir = -m_dirToSun;
        m_sunDistance = 600.0f;

        m_improvements = Improvements(m_player, {{ImprovementType::PLAYER_MAX_SPEED,                    5},
                                                            {ImprovementType::PLAYER_ACCELERATE_FASTER,            5},
                                                            {ImprovementType::PLAYER_MOVE_FASTER_THROUGH_ENEMIES,  5},
                                                            {ImprovementType::PLAYER_BETTER_CLUTCH_WITH_BUILDINGS, 5},
                                                            {ImprovementType::PLAYER_INCREASE_SIZE,                5},
                                                            {ImprovementType::PLAYER_INCREASE_MAX_HP,              5},
                                                            {ImprovementType::PLAYER_REDUCE_DAMAGE,                5},
                                                            {ImprovementType::PLAYER_TAKE_MORE_XP,                 5},
                                                            {ImprovementType::PLAYER_HEAL_AT_NEW_LVL,              5},
                                                            {ImprovementType::GARBAGE_SPAWN_MORE_ON_MAP,           5},
                                                            {ImprovementType::GARBAGE_REDUCE_DAMAGE,               5}
                                                            });

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

        m_player = std::make_shared<Player>(playerAllBalls[0], 500);
        m_player->setAllModels(playerAllBalls);

        m_player->getObj()->setOrigin(glm::vec3(-770.0f, 2.0f,0.0f));
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

        const auto objects1 = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map1/Buildings.fbx",
                                                                                       0.0f,
                                                                                       false,
                                                                                       Beryll::CollisionFlags::STATIC,
                                                                                       Beryll::CollisionGroups::BUILDING,
                                                                                       Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE |
                                                                                       Beryll::CollisionGroups::RAY_FOR_BUILDING_CHECK  | Beryll::CollisionGroups::ENEMY_ATTACK,
                                                                                       Beryll::SceneObjectGroups::BUILDING);

        for(const auto& obj : objects1)
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
        for(int i = 0; i < 20; ++i)
        {
            const auto garbageCommon = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map1/GarbageCommon.fbx",
                                                                                           EnumsAndVariables::garbageMass,
                                                                                           false,
                                                                                           Beryll::CollisionFlags::DYNAMIC,
                                                                                           Beryll::CollisionGroups::GARBAGE,
                                                                                           Beryll::CollisionGroups::GROUND | Beryll::CollisionGroups::BUILDING |
                                                                                           Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE |
                                                                                           Beryll::CollisionGroups::ENEMY_ATTACK,
                                                                                           Beryll::SceneObjectGroups::GARBAGE);

            for(const auto& obj : garbageCommon)
            {
                m_allGarbage.emplace_back(obj, GarbageType::COMMON, 30);
                m_allGarbage.back().disableGarbage();

                m_animatedOrDynamicObjects.push_back(obj);
                m_simpleObjForShadowMap.push_back(obj);

                obj->setDamping(EnumsAndVariables::garbageLinearDamping, EnumsAndVariables::garbageAngularDamping);
                obj->setGravity(EnumsAndVariables::garbageGravityDefault, false, false);

            }
        }
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

                if(copWithPistolCount < 100 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL)
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

                if(copWithPistolCount < 100 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL)
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
                else if(copWithGrenadeLauncherCount < 50 && enemy->getUnitType() == UnitType::COP_WITH_GRENADE_LAUNCHER)
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

                if(copWithPistolCount < 100 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL)
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

                if(copWithPistolCount < 100 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL)
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
                else if(copWithGrenadeLauncherCount < 85 && enemy->getUnitType() == UnitType::COP_WITH_GRENADE_LAUNCHER)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithGrenadeLauncherCount;
                    ++EnumsAndVariables::enemiesMaxActiveCountOnGround;
                }
                else if(sniperCount < 13 && enemy->getUnitType() == UnitType::SNIPER)
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

                if(copWithPistolCount < 100 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL)
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
                else if(copWithGrenadeLauncherCount < 150 && enemy->getUnitType() == UnitType::COP_WITH_GRENADE_LAUNCHER)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithGrenadeLauncherCount;
                    ++EnumsAndVariables::enemiesMaxActiveCountOnGround;
                }
                else if(sniperCount < 13 && enemy->getUnitType() == UnitType::SNIPER)
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

                if(copWithPistolCount < 100 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolCount;
                    ++EnumsAndVariables::enemiesMaxActiveCountOnGround;
                }
                else if(copWithPistolShieldCount < 150 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL_SHIELD)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolShieldCount;
                    ++EnumsAndVariables::enemiesMaxActiveCountOnGround;
                }
                else if(copWithGrenadeLauncherCount < 235 && enemy->getUnitType() == UnitType::COP_WITH_GRENADE_LAUNCHER)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithGrenadeLauncherCount;
                    ++EnumsAndVariables::enemiesMaxActiveCountOnGround;
                }
                else if(sniperCount < 13 && enemy->getUnitType() == UnitType::SNIPER)
                {
                    enemy->isCanBeSpawned = true;
                    ++sniperCount;
                    ++EnumsAndVariables::enemiesMaxActiveCountOnGround;
                }
                else if(tankCount < 30 && enemy->getUnitType() == UnitType::TANK)
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

        bool sortSnipersPositions = true;

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
                    if(sortSnipersPositions)
                    {
                        sortSnipersPositions = false;

                        // Sort by distance from player.
                        std::sort(Sniper::sniperPositions.begin(), Sniper::sniperPositions.end(),
                                  [&](const SniperPosAndRange& pos1, const SniperPosAndRange& pos2)
                                  { return glm::distance(m_player->getObj()->getOrigin(), pos1.position) < glm::distance(m_player->getObj()->getOrigin(), pos2.position); });

                        for(const SniperPosAndRange& posAndRange : Sniper::sniperPositions)
                        {
                            BR_INFO("Distance from player to sniper: %d", glm::distance(m_player->getObj()->getOrigin(), posAndRange.position));
                        }
                    }

                    for(SniperPosAndRange& posAndRange : Sniper::sniperPositions)
                    {
                        Beryll::RayClosestHit rayBuildingHit = Beryll::Physics::castRayClosestHit(m_player->getObj()->getOrigin(),
                                                                                                  posAndRange.position,
                                                                                                  Beryll::CollisionGroups::RAY_FOR_BUILDING_CHECK,
                                                                                                  Beryll::CollisionGroups::BUILDING);

                        // Check position in free + not closest than 150m + sniper see player(rayBuildingHit does not hit).
                        if(posAndRange.isFreePosition &&
                           glm::distance(m_player->getObj()->getOrigin(), posAndRange.position) > 150.0f &&
                           !rayBuildingHit)
                        {
                            posAndRange.isFreePosition = false;

                            enemy->enableEnemy();
                            enemy->disableDraw();
                            enemy->setOrigin(posAndRange.position);

                            break;
                        }
                    }
                }
            }
        }

        if(spawnedCount > 0)
        {
            BR_INFO("spawnedCount: %d BaseEnemy::getActiveCount(): %d", spawnedCount, BaseEnemy::getActiveCount());
        }
    }
}
