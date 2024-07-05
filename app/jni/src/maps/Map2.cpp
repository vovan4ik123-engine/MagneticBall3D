#include "Map2.h"
#include "EnumsAndVariables.h"
#include "enemies/MovableEnemy.h"
#include "enemies/StaticEnemy.h"
#include "Sounds.h"

namespace MagneticBall3D
{
    Map2::Map2(std::shared_ptr<PlayStateGUILayer> gui) : BaseMap(std::move(gui))
    {
        Beryll::LoadingScreen::showProgress(10.0f);

        // Allocate enough spase for all vectors to avoid vector reallocation.
        const int maxGarbageCount = 350;
        m_allGarbage.reserve(maxGarbageCount);
        m_allAnimatedEnemies.reserve(500);
        m_animatedOrDynamicObjects.reserve(500 + maxGarbageCount);
        m_staticEnv.reserve(300);
        m_simpleObjForShadowMap.reserve(300 + maxGarbageCount);
        m_animatedObjForShadowMap.reserve(400);

        // Specific for this map only.
        loadPlayer();
        m_player->getObj()->setOrigin(glm::vec3(-770.0f, 2.0f,-520.0f));
        m_improvements.setPlayer(m_player);
        Beryll::LoadingScreen::showProgress(20.0f);
        loadEnv();
        Beryll::LoadingScreen::showProgress(40.0f);
        //loadGarbage();
        BR_ASSERT((m_allGarbage.size() < maxGarbageCount), "%s", "m_allGarbage reallocation happened. Increase maxGarbageCount.");
        Beryll::LoadingScreen::showProgress(60.0f);
        loadEnemies();
        Beryll::LoadingScreen::showProgress(80.0f);
        loadBoss();
        Beryll::LoadingScreen::showProgress(90.0f);

        loadShaders();
        m_cameraOffset = glm::normalize(glm::vec3(-1.0f, 0.0f, -0.5f));
        handleCamera();

        m_minX = -800.0f;
        m_maxX = 800.0f;
        m_minZ = -800.0f;
        m_maxZ = 800.0f;
        m_pathFinder = AStar(m_minX, m_maxX, m_minZ, m_maxZ, 20);
        std::vector<glm::vec3> walls = BeryllUtils::Common::loadMeshVerticesToVector("models3D/map2/PathEnemiesWalls.fbx");
        for(const auto& wall : walls)
        {
            m_pathFinder.addWallPosition({(int)std::roundf(wall.x), (int)std::roundf(wall.z)});
        }

        BR_INFO("Map2 pathfinder walls: %d", walls.size());

        std::vector<glm::vec3> allowedPoints = BeryllUtils::Common::loadMeshVerticesToVector("models3D/map2/PathEnemiesAllowedPositions.fbx");
        m_pathAllowedPositionsXZ.reserve(allowedPoints.size());
        for(const auto& point : allowedPoints)
        {
            m_pathAllowedPositionsXZ.push_back({(int)std::roundf(point.x), (int)std::roundf(point.z)});
        }

        BR_INFO("Map2 pathfinder allowed points: %d", m_pathAllowedPositionsXZ.size());
        m_pointsToSpawnEnemies.reserve(m_pathAllowedPositionsXZ.size());
        m_pointsToSpawnCommonGarbage.reserve(m_pathAllowedPositionsXZ.size());

        m_pathFinderBoss = AStar(m_minX, m_maxX, m_minZ, m_maxZ, 40);
        std::vector<glm::vec3> wallsBoss = BeryllUtils::Common::loadMeshVerticesToVector("models3D/map1/PathBossWalls.fbx");
        for(const auto& wall : wallsBoss)
        {
            m_pathFinderBoss.addWallPosition({(int)std::roundf(wall.x), (int)std::roundf(wall.z)});
        }

        BR_INFO("Map2 pathfinder wallsBoss: %d", wallsBoss.size());

        std::vector<glm::vec3> allowedPointsBoss = BeryllUtils::Common::loadMeshVerticesToVector("models3D/map1/PathBossAllowedPositions.fbx");
        m_pathAllowedPositionsXZBoss.reserve(allowedPointsBoss.size());
        for(const auto& point : allowedPointsBoss)
        {
            m_pathAllowedPositionsXZBoss.push_back({(int)std::roundf(point.x), (int)std::roundf(point.z)});
        }

        BR_INFO("Map2 pathfinder allowed points boss: %d", m_pathAllowedPositionsXZBoss.size());

        m_dirToSun = glm::normalize(glm::vec3(-1.0f, 1.0f, 1.0f));
        m_sunLightDir = -m_dirToSun;

        m_skyBox = Beryll::Renderer::createSkyBox("skyboxes/map1");

        EnAndVars::garbageCommonSpawnCount = 4;

        //SendStatisticsHelper::sendMapStart();

        Beryll::LoadingScreen::showProgress(100.0f);

        //Sounds::startBackgroundMusic(SoundType::BACKGROUND_MUSIC_1);

        //BR_INFO(" %f", );
        //BR_INFO("%s", "");

    }

    Map2::~Map2()
    {

    }

    void Map2::draw()
    {
        //BR_INFO("%s", "scene draw call");
        // 1. Draw into shadow map.
        glm::vec3 sunPos = m_player->getObj()->getOrigin() +
                           (Beryll::Camera::getCameraFrontDirectionXZ() * 200.0f) +
                           (m_dirToSun * (600.0f - m_player->getObj()->getOrigin().y)); // sunPos.y is 600 max.
        updateSunPosition(sunPos, m_shadowsCubeWidth, m_shadowsCubeHeight, m_shadowsCubeDepth);

        Beryll::Renderer::disableFaceCulling();
        if(BaseEnemy::getActiveCount() < 150)
            m_shadowMap->drawIntoShadowMap(m_simpleObjForShadowMap, m_animatedObjForShadowMap, m_sunLightVPMatrix);
        else
            m_shadowMap->drawIntoShadowMap(m_simpleObjForShadowMap, {}, m_sunLightVPMatrix);
        Beryll::Renderer::enableFaceCulling();

        // 2. Draw scene.
        glm::mat4 modelMatrix{1.0f};

        if(EnAndVars::gameOnPause || EnAndVars::improvementSystemOnScreen)
            m_ambientLight = 0.25f;
        else
            m_ambientLight = 0.7f;

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

//        if(m_boss->getIsEnabledDraw())
//        {
//            modelMatrix = m_boss->getModelMatrix();
//            m_animatedObjSunLight->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
//            Beryll::Renderer::drawObject(m_boss, modelMatrix, m_animatedObjSunLight);
//
//            // Sync HP bar rotations with camera direction.
//            m_bossHpBar.addToRotation(glm::rotation(m_bossHpBar.getFaceDirXYZ(), Beryll::Camera::getCameraFrontDirectionXZ()));
//            m_bossHpBar.addToRotation(glm::rotation(m_bossHpBar.getUpDirXYZ(), Beryll::Camera::getCameraUp()));
//            glm::vec3 bossHPOrigin = m_boss->getOrigin();
//            bossHPOrigin.y += 41.0f;
//            m_bossHpBar.setOrigin(bossHPOrigin);
//            m_bossHpBar.progress = 1.0f - float(m_boss->getCurrentHP()) / float(m_boss->getMaxHP());
//
//            m_bossHpBar.draw();
//        }

        m_simpleObjSunLightShadowsNormals->bind();
        m_simpleObjSunLightShadowsNormals->set3Float("sunLightDir", m_sunLightDir);
        m_simpleObjSunLightShadowsNormals->set3Float("cameraPos", Beryll::Camera::getCameraPos());
        m_simpleObjSunLightShadowsNormals->set1Float("ambientLight", m_gui->sliderAmbient->getValue());
        m_simpleObjSunLightShadowsNormals->set1Float("specularLightStrength", m_gui->sliderSpecularPower->getValue());

        for(const auto& normalMapObj : m_objWithNormalMap)
        {
            modelMatrix = normalMapObj->getModelMatrix();
            m_simpleObjSunLightShadowsNormals->setMatrix4x4Float("MVPLightMatrix", m_sunLightVPMatrix * modelMatrix);
            m_simpleObjSunLightShadowsNormals->setMatrix4x4Float("modelMatrix", modelMatrix);
            m_simpleObjSunLightShadowsNormals->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
            Beryll::Renderer::drawObject(normalMapObj, modelMatrix, m_simpleObjSunLightShadowsNormals);
        }

        m_simpleObjSunLightShadows->bind();
        m_simpleObjSunLightShadows->set3Float("sunLightDir", m_sunLightDir);
        m_simpleObjSunLightShadows->set3Float("cameraPos", Beryll::Camera::getCameraPos());
        m_simpleObjSunLightShadows->set1Float("ambientLight", m_ambientLight);
        m_simpleObjSunLightShadows->set1Float("specularLightStrength", 1.4f);
        m_simpleObjSunLightShadows->set1Float("alphaTransparency", 1.0f);

        modelMatrix = m_player->getObj()->getModelMatrix();
        m_simpleObjSunLightShadows->setMatrix4x4Float("MVPLightMatrix", m_sunLightVPMatrix * modelMatrix);
        m_simpleObjSunLightShadows->setMatrix4x4Float("modelMatrix", modelMatrix);
        m_simpleObjSunLightShadows->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
        Beryll::Renderer::drawObject(m_player->getObj(), modelMatrix, m_simpleObjSunLightShadows);

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

        // If player is on building roof this building should be semitransparent.
        std::shared_ptr<Beryll::BaseSimpleObject> semiTransparentBuilding = nullptr;

        m_simpleObjSunLightShadows->set1Float("specularLightStrength", 1.5f);

        for(const auto& staticObj : m_staticEnv)
        {
            if(staticObj->getIsEnabledDraw())
            {
//                if(m_player->getIsOnBuilding() && m_player->getBuildingNormalAngle() < 0.17f && // Building normal < 10 degrees with BeryllConstants::worldUp.
//                   staticObj->getID() == m_player->getBuildingCollisionID())
//                {
//                    // Store this building and draw it last.
//                    semiTransparentBuilding = staticObj;
//                }
//                else
                {
                    modelMatrix = staticObj->getModelMatrix();
                    m_simpleObjSunLightShadows->setMatrix4x4Float("MVPLightMatrix", m_sunLightVPMatrix * modelMatrix);
                    m_simpleObjSunLightShadows->setMatrix4x4Float("modelMatrix", modelMatrix);
                    m_simpleObjSunLightShadows->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
                    Beryll::Renderer::drawObject(staticObj, modelMatrix, m_simpleObjSunLightShadows);
                }
            }
        }

//        if(semiTransparentBuilding)
//        {
//            modelMatrix = semiTransparentBuilding->getModelMatrix();
//            m_simpleObjSunLightShadows->set1Float("alphaTransparency", 0.5f);
//            m_simpleObjSunLightShadows->set1Float("specularLightStrength", 0.0f);
//            m_simpleObjSunLightShadows->setMatrix4x4Float("MVPLightMatrix", m_sunLightVPMatrix * modelMatrix);
//            m_simpleObjSunLightShadows->setMatrix4x4Float("modelMatrix", modelMatrix);
//            m_simpleObjSunLightShadows->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
//            Beryll::Renderer::drawObject(semiTransparentBuilding, modelMatrix, m_simpleObjSunLightShadows);
//        }

        m_skyBox->draw();
        Beryll::ParticleSystem::draw();
    }

    void Map2::loadEnv()
    {
        // Main ground has normal map in material. Other ground pieces dont.
        const auto mainGround = std::make_shared<Beryll::SimpleCollidingObject>("models3D/map2/MainGround.fbx",
                                                                                0.0f,
                                                                                false,
                                                                                Beryll::CollisionFlags::STATIC,
                                                                                Beryll::CollisionGroups::GROUND,
                                                                                Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE |
                                                                                Beryll::CollisionGroups::MOVABLE_ENEMY,
                                                                                Beryll::SceneObjectGroups::GROUND);

        m_objWithNormalMap.push_back(mainGround);
        mainGround->setFriction(EnAndVars::staticEnvFriction);

        const auto otherGrounds = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map2/OtherGrounds.fbx",
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
            obj->setFriction(EnAndVars::staticEnvFriction);
        }

        const auto buildings = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map2/Buildings.fbx",
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
            obj->setFriction(EnAndVars::staticEnvFriction);
        }

        const auto envNoColliders1 = Beryll::SimpleObject::loadManyModelsFromOneFile("models3D/map2/EnvNoColliders.fbx", Beryll::SceneObjectGroups::BUILDING);

        for(const auto& obj : envNoColliders1)
        {
            m_staticEnv.push_back(obj);
        }
    }

    void Map2::loadGarbage()
    {
        for(int i = 0; i < 6; ++i) // 6 * 34 = 204
        {
            const auto garbageCommon = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map1/GarbageCommon.fbx",
                                                                                                EnAndVars::garbageMass,
                                                                                                false,
                                                                                                Beryll::CollisionFlags::DYNAMIC,
                                                                                                Beryll::CollisionGroups::GARBAGE,
                                                                                                Beryll::CollisionGroups::GROUND | Beryll::CollisionGroups::BUILDING |
                                                                                                Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE |
                                                                                                Beryll::CollisionGroups::ENEMY_ATTACK,
                                                                                                Beryll::SceneObjectGroups::GARBAGE);

            for(const auto& obj : garbageCommon)
            {
                m_allGarbage.emplace_back(obj, GarbageType::COMMON, EnAndVars::garbageStartHP);

                m_animatedOrDynamicObjects.push_back(obj);
                m_simpleObjForShadowMap.push_back(obj);

                obj->setDamping(EnAndVars::garbageLinearDamping, EnAndVars::garbageAngularDamping);
                obj->setGravity(EnAndVars::garbageGravityDefault, false, false);
            }
        }

        BR_INFO("Garbage::getCommonActiveCount() %d", Garbage::getCommonActiveCount());
    }

    void Map2::loadEnemies()
    {
        for(int i = 0; i < 20; ++i)
        {
            auto rat = std::make_shared<MovableEnemy>("models3D/enemies/RatWithMagnet.fbx",
                                                            0.0f,
                                                            false,
                                                            Beryll::CollisionFlags::STATIC,
                                                            Beryll::CollisionGroups::NONE,
                                                            Beryll::CollisionGroups::NONE,
                                                            Beryll::SceneObjectGroups::ENEMY);

            rat->setCurrentAnimationByIndex(EnAndVars::AnimationIndexes::run, false, false);
            rat->setDefaultAnimationByIndex(EnAndVars::AnimationIndexes::stand);
            rat->unitType = UnitType::RAT_WITH_MAGNET;
            rat->attackType = AttackType::STEAL_GARBAGE;
            rat->castRayToFindYPos = true;
            rat->isCanBeSpawned = true;

            rat->attackDistance = 70.0f + Beryll::RandomGenerator::getFloat() * 20.0f;
            rat->timeBetweenAttacks = 2.0f + Beryll::RandomGenerator::getFloat() * 0.1f;

            rat->garbageAmountToDie = 10;
            rat->reducePlayerSpeedWhenDie = 5.0f;
            rat->experienceWhenDie = 25;
            rat->getController().moveSpeed = 40.0f;

            m_animatedOrDynamicObjects.push_back(rat);
            m_allAnimatedEnemies.push_back(rat);
            m_animatedObjForShadowMap.push_back(rat);
        }

        for(int i = 0; i < 0; ++i)
        {
            auto guard = std::make_shared<MovableEnemy>("models3D/enemies/JunkyardGuard.fbx",
                                                        0.0f,
                                                        false,
                                                        Beryll::CollisionFlags::STATIC,
                                                        Beryll::CollisionGroups::NONE,
                                                        Beryll::CollisionGroups::NONE,
                                                        Beryll::SceneObjectGroups::ENEMY);

            guard->setCurrentAnimationByIndex(EnAndVars::AnimationIndexes::run, false, false);
            guard->setDefaultAnimationByIndex(EnAndVars::AnimationIndexes::stand);
            guard->unitType = UnitType::STAND_GUN;
            guard->attackType = AttackType::RANGE_DAMAGE_ONE;
            guard->castRayToFindYPos = true;
            guard->isCanBeSpawned = true;

            guard->damage = 1.0f;
            guard->attackDistance = 100.0f + Beryll::RandomGenerator::getFloat() * 100.0f;
            guard->timeBetweenAttacks = 1.5f + Beryll::RandomGenerator::getFloat() * 0.1f;

            guard->garbageAmountToDie = 10;
            guard->reducePlayerSpeedWhenDie = 8.0f;
            guard->experienceWhenDie = 25;
            guard->getController().moveSpeed = 25.0f;

            m_animatedOrDynamicObjects.push_back(guard);
            m_allAnimatedEnemies.push_back(guard);
            m_animatedObjForShadowMap.push_back(guard);
        }
    }

    void Map2::loadBoss()
    {

    }

    void Map2::spawnEnemies()
    {
        EnAndVars::enemiesMaxActiveCountOnGround = 30;

        if(!m_pointsToSpawnEnemies.empty())
        {
            for (const auto &enemy: m_allAnimatedEnemies)
            {
                if (BaseEnemy::getActiveCount() >= EnAndVars::enemiesMaxActiveCountOnGround)
                    break;

                if (enemy->getIsEnabledUpdate() || !enemy->isCanBeSpawned)
                    continue;

                enemy->enableEnemy();
                enemy->disableDraw();

                BR_INFO("%s", "spawn rat");

                const glm::ivec2 spawnPoint2D = m_pointsToSpawnEnemies[Beryll::RandomGenerator::getInt(m_pointsToSpawnEnemies.size() - 1)];

                enemy->setPathArray(m_pathFinder.findPath(spawnPoint2D, m_playerClosestAllowedPos, 6), 1);

                enemy->setOrigin(enemy->getStartPointMoveFrom());
            }
        }
    }

    void Map2::startBossPhase()
    {

    }

    void Map2::handlePossPhase()
    {

    }

    void Map2::spawnCommonGarbage()
    {
        if(!m_pointsToSpawnCommonGarbage.empty() &&
           EnAndVars::garbageCommonSpawnTime + EnAndVars::garbageCommonSpawnDelay < EnAndVars::mapPlayTimeSec)
        {
            EnAndVars::garbageCommonSpawnTime = EnAndVars::mapPlayTimeSec;

            const glm::ivec2 spawnPoint2D = m_pointsToSpawnCommonGarbage[Beryll::RandomGenerator::getInt(m_pointsToSpawnCommonGarbage.size() - 1)];
            glm::vec3 spawnPoint3D{spawnPoint2D.x, 10.0f, spawnPoint2D.y};

            Beryll::RayClosestHit rayHit = Beryll::Physics::castRayClosestHit(glm::vec3{spawnPoint2D.x, 70.0f, spawnPoint2D.y},
                                                                              glm::vec3{spawnPoint2D.x, -10.0f, spawnPoint2D.y},
                                                                              Beryll::CollisionGroups::GARBAGE,
                                                                              Beryll::CollisionGroups::GROUND);

            if(rayHit)
            {
                spawnPoint3D.y = rayHit.hitPoint.y + 10.0f;
            }

            spawnGarbage(EnAndVars::garbageCommonSpawnCount, GarbageType::COMMON, spawnPoint3D);
            //BR_INFO("Garbage::getCommonActiveCount() %d", Garbage::getCommonActiveCount());
        }
    }
}