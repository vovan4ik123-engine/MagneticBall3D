#include "Map1.h"
#include "EnumsAndVariables.h"
#include "enemies/CopWithPistol.h"
#include "enemies/CopWithGrenadeLauncher.h"

namespace MagneticBall3D
{
    Map1::Map1(std::shared_ptr<PlayStateGUILayer> gui) : BaseMap(std::move(gui))
    {
        // Specific for this map only.
        loadPlayerAndStaticEnv();
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
        std::vector<glm::vec3> walls = BeryllUtils::Common::loadMeshVerticesToVector("models3D/map1/PathWalls.fbx");
        for(const auto& wall : walls)
        {
            m_pathFinder.addWallPosition({(int)std::roundf(wall.x), (int)std::roundf(wall.z)});
        }

        BR_INFO("Map1 pathfinder walls.size() %d", walls.size());

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
    }

    Map1::~Map1()
    {

    }

    void Map1::loadPlayerAndStaticEnv()
    {
        const auto playerBall = std::make_shared<Beryll::SimpleCollidingObject>("models3D/player/Player.fbx",
                                                                                EnumsAndVariables::playerMass,
                                                                                true,
                                                                                Beryll::CollisionFlags::DYNAMIC,
                                                                                Beryll::CollisionGroups::PLAYER,
                                                                                Beryll::CollisionGroups::GROUND | Beryll::CollisionGroups::BUILDING |
                                                                                Beryll::CollisionGroups::GARBAGE | Beryll::CollisionGroups::ENEMY_ATTACK,
                                                                                Beryll::SceneObjectGroups::PLAYER);

        m_allSceneObjects.push_back(playerBall);
        m_simpleObjForShadowMap.push_back(playerBall);
        m_player = std::make_shared<Player>(playerBall, 1000);

        //m_player->setOrigin(glm::vec3(-140.0f, 5.0f,-140.0f));
        m_player->getObj()->setGravity(EnumsAndVariables::playerGravityOnGround);
        m_player->getObj()->setFriction(EnumsAndVariables::playerFriction);
        m_player->getObj()->setDamping(EnumsAndVariables::playerDamping, EnumsAndVariables::playerDamping);

        // Static env.
        const auto ground = std::make_shared<Beryll::SimpleCollidingObject>("models3D/map1/Ground.fbx",
                                                                            0.0f,
                                                                            false,
                                                                            Beryll::CollisionFlags::STATIC,
                                                                            Beryll::CollisionGroups::GROUND,
                                                                            Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE,
                                                                            Beryll::SceneObjectGroups::GROUND);

        m_allSceneObjects.push_back(ground);
        m_allStaticEnv.push_back(ground);
        m_simpleObjForShadowMap.push_back(ground);
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
            m_allSceneObjects.push_back(obj);
            m_allStaticEnv.push_back(obj);
            m_simpleObjForShadowMap.push_back(obj);
            obj->setFriction(EnumsAndVariables::staticEnvFriction);
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
            m_allGarbageWrappers.emplace_back(obj, GarbageType::DEFAULT, 5);
            m_allGarbageWrappers.back().disableGarbage();

            m_allSceneObjects.push_back(obj);
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
            m_allGarbageWrappers.emplace_back(obj, GarbageType::DEFAULT, 5);
            m_allGarbageWrappers.back().disableGarbage();

            m_allSceneObjects.push_back(obj);
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
            m_allGarbageWrappers.emplace_back(obj, GarbageType::DEFAULT, 5);
            m_allGarbageWrappers.back().disableGarbage();

            m_allSceneObjects.push_back(obj);
            m_simpleObjForShadowMap.push_back(obj);

            obj->setDamping(EnumsAndVariables::garbageDamping, EnumsAndVariables::garbageDamping);
            obj->setGravity(EnumsAndVariables::garbageGravityDefault, false, false);
        }
    }

    void Map1::loadEnemies()
    {
        for(int x = 0; x < 300; ++x)
        {
            auto unit = std::make_shared<CopWithPistol>("models3D/enemies/CopWithPistol.fbx",
                                                        0.0f,
                                                        false,
                                                        Beryll::CollisionFlags::STATIC,
                                                        Beryll::CollisionGroups::NONE,
                                                        Beryll::CollisionGroups::NONE,
                                                        Beryll::SceneObjectGroups::ENEMY);

            unit->setCurrentAnimationByIndex(EnumsAndVariables::AnimationIndexes::RUN, false, false);
            unit->setDefaultAnimationByIndex(EnumsAndVariables::AnimationIndexes::STAND);
            unit->getController().moveSpeed = 20.0f;

            m_allSceneObjects.push_back(unit);
            m_allAnimatedEnemies.push_back(unit);
            m_animatedObjForShadowMap.push_back(unit);

            auto unit2 = std::make_shared<CopWithGrenadeLauncher>("models3D/enemies/CopWithGrenadeLauncher.fbx",
                                                        0.0f,
                                                        false,
                                                        Beryll::CollisionFlags::STATIC,
                                                        Beryll::CollisionGroups::NONE,
                                                        Beryll::CollisionGroups::NONE,
                                                        Beryll::SceneObjectGroups::ENEMY);

            unit2->setCurrentAnimationByIndex(EnumsAndVariables::AnimationIndexes::RUN, false, false);
            unit2->setDefaultAnimationByIndex(EnumsAndVariables::AnimationIndexes::STAND);
            unit2->getController().moveSpeed = 20.0f;

            m_allSceneObjects.push_back(unit2);
            m_allAnimatedEnemies.push_back(unit2);
            m_animatedObjForShadowMap.push_back(unit2);
        }
    }
}
