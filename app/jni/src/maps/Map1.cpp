#include "Map1.h"
#include "EnumsAndVariables.h"
#include "enemies/MovableEnemy.h"
#include "enemies/StaticEnemy.h"
#include "bosses/TankWithCommander.h"
#include "Sounds.h"

namespace MagneticBall3D
{
    Map1::Map1(std::shared_ptr<PlayStateGUILayer> gui) : BaseMap(std::move(gui))
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
        m_player->getObj()->setOrigin(glm::vec3(-770.0f, 2.0f,0.0f));
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

        StaticEnemy::staticPositions = std::vector<StaticPosition>{{glm::vec3(65.0f, 252.0f, -445.0f)},
                                                                   {glm::vec3(65.0f, 214.0f, -264.0f)},
                                                                   {glm::vec3(56.5f, 250.0f, -129.5f)},
                                                                   {glm::vec3(71.0f, 250.0f, -60.0f)},
                                                                   {glm::vec3(129.0f, 250.0f, -55.5f)},
                                                                   {glm::vec3(260.5f, 215.0f, -61.0f)},
                                                                   {glm::vec3(320.0f, 215.0f, -61.0f)},
                                                                   {glm::vec3(458.5f, 242.0f, -62.0f)},
                                                                   {glm::vec3(639.0f, 255.0f, -58.0f)},
                                                                   {glm::vec3(56.0f, 305.5f, 435.0f)},
                                                                   {glm::vec3(56.0f, 305.5f, 484.0f)},
                                                                   {glm::vec3(145.0f, 305.5f, 396.5f)}
        };

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
        m_pointsToSpawnEnemies.reserve(m_pathAllowedPositionsXZ.size());
        m_pointsToSpawnCommonGarbage.reserve(m_pathAllowedPositionsXZ.size());

        m_pathFinderBoss = AStar(m_minX, m_maxX, m_minZ, m_maxZ, 40);
        std::vector<glm::vec3> wallsBoss = BeryllUtils::Common::loadMeshVerticesToVector("models3D/map1/PathBossWalls.fbx");
        for(const auto& wall : wallsBoss)
        {
            m_pathFinderBoss.addWallPosition({(int)std::roundf(wall.x), (int)std::roundf(wall.z)});
        }

        BR_INFO("Map1 pathfinder wallsBoss: %d", wallsBoss.size());

        std::vector<glm::vec3> allowedPointsBoss = BeryllUtils::Common::loadMeshVerticesToVector("models3D/map1/PathBossAllowedPositions.fbx");
        m_pathAllowedPositionsXZBoss.reserve(allowedPointsBoss.size());
        for(const auto& point : allowedPointsBoss)
        {
            m_pathAllowedPositionsXZBoss.push_back({(int)std::roundf(point.x), (int)std::roundf(point.z)});
        }

        BR_INFO("Map1 pathfinder allowed points boss: %d", m_pathAllowedPositionsXZBoss.size());

        m_dirToSun = glm::normalize(glm::vec3(-1.0f, 3.5f, -0.8f));
        m_sunLightDir = -m_dirToSun;

        m_skyBox = Beryll::Renderer::createSkyBox("skyboxes/map1");

        EnumsAndVars::garbageCommonSpawnCount = 5;

        SendStatisticsHelper::sendMapStart();

        Beryll::LoadingScreen::showProgress(100.0f);

        Sounds::startBackgroundMusic(SoundType::BACKGROUND_MUSIC_2);

        //BR_INFO(" %f", );
        //BR_INFO("%s", "");
    }

    Map1::~Map1()
    {
        Sounds::stopBackgroundMusic();
    }

    void Map1::draw()
    {
        //BR_INFO("%s", "Scene draw call.");
        m_improvements.draw();

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

        if(EnumsAndVars::gameOnPause || EnumsAndVars::improvementSystemOnScreen)
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

        if(m_boss->getIsEnabledDraw())
        {
            modelMatrix = m_boss->getModelMatrix();
            m_animatedObjSunLight->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
            Beryll::Renderer::drawObject(m_boss, modelMatrix, m_animatedObjSunLight);

            // Sync HP bar rotations with camera direction.
            m_bossHpBar.addToRotation(glm::rotation(m_bossHpBar.getFaceDirXYZ(), Beryll::Camera::getCameraFrontDirectionXZ()));
            m_bossHpBar.addToRotation(glm::rotation(m_bossHpBar.getUpDirXYZ(), Beryll::Camera::getCameraUp()));
            glm::vec3 bossHPOrigin = m_boss->getOrigin();
            bossHPOrigin.y += 41.0f;
            m_bossHpBar.setOrigin(bossHPOrigin);
            m_bossHpBar.progress = 1.0f - float(m_boss->getCurrentHP()) / float(m_boss->getMaxHP());

            m_bossHpBar.draw();
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

        m_simpleObjSunLightShadows->set1Float("specularLightStrength", 1.0f);

        for(const auto& staticObj : m_staticEnv)
        {
            if(staticObj->getIsEnabledDraw())
            {
                if(m_player->getIsOnBuilding() && m_player->getBuildingNormalAngle() < 0.17f && // Building normal < 10 degrees with BeryllConstants::worldUp.
                   staticObj->getID() == m_player->getBuildingCollisionID())
                {
                    // Store this building and draw it last.
                    semiTransparentBuilding = staticObj;
                }
                else
                {
                    modelMatrix = staticObj->getModelMatrix();
                    m_simpleObjSunLightShadows->setMatrix4x4Float("MVPLightMatrix", m_sunLightVPMatrix * modelMatrix);
                    m_simpleObjSunLightShadows->setMatrix4x4Float("modelMatrix", modelMatrix);
                    m_simpleObjSunLightShadows->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
                    Beryll::Renderer::drawObject(staticObj, modelMatrix, m_simpleObjSunLightShadows);
                }
            }
        }

        if(semiTransparentBuilding)
        {
            modelMatrix = semiTransparentBuilding->getModelMatrix();
            m_simpleObjSunLightShadows->set1Float("alphaTransparency", 0.5f);
            m_simpleObjSunLightShadows->set1Float("specularLightStrength", 0.0f);
            m_simpleObjSunLightShadows->setMatrix4x4Float("MVPLightMatrix", m_sunLightVPMatrix * modelMatrix);
            m_simpleObjSunLightShadows->setMatrix4x4Float("modelMatrix", modelMatrix);
            m_simpleObjSunLightShadows->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
            Beryll::Renderer::drawObject(semiTransparentBuilding, modelMatrix, m_simpleObjSunLightShadows);
        }

        m_skyBox->draw();
        Beryll::ParticleSystem::draw();
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
        ground->setFriction(EnumsAndVars::staticEnvFriction);

        const auto buildings = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map1/Buildings.fbx",
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
        for(int i = 0; i < 6; ++i) // 6 * 34 = 204
        {
            const auto garbageCommon = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map1/GarbageCommon.fbx",
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

        for(int i = 0; i < 5; ++i) // 5 * 4 = 20
        {
            const auto garbageCopPistol = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map1/GarbageCopPistol_4items.fbx",
                                                                                                   EnumsAndVars::garbageMass,
                                                                                                   false,
                                                                                                   Beryll::CollisionFlags::DYNAMIC,
                                                                                                   Beryll::CollisionGroups::GARBAGE,
                                                                                                Beryll::CollisionGroups::GROUND | Beryll::CollisionGroups::BUILDING |
                                                                                                Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE |
                                                                                                Beryll::CollisionGroups::ENEMY_ATTACK,
                                                                                                   Beryll::SceneObjectGroups::GARBAGE);

            for(const auto& obj : garbageCopPistol)
            {
                m_allGarbage.emplace_back(obj, GarbageType::ENEMY_GUN, EnumsAndVars::garbageStartHP);

                m_animatedOrDynamicObjects.push_back(obj);
                m_simpleObjForShadowMap.push_back(obj);

                obj->setDamping(EnumsAndVars::garbageLinearDamping, EnumsAndVars::garbageAngularDamping);
                obj->setGravity(EnumsAndVars::garbageGravityDefault, false, false);
            }
        }

        for(int i = 0; i < 5; ++i) // 5 * 5 = 25
        {
            const auto garbageCopShield = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map1/GarbageCopShield_5items.fbx",
                                                                                                   EnumsAndVars::garbageMass,
                                                                                                   false,
                                                                                                   Beryll::CollisionFlags::DYNAMIC,
                                                                                                   Beryll::CollisionGroups::GARBAGE,
                                                                                                   Beryll::CollisionGroups::GROUND | Beryll::CollisionGroups::BUILDING |
                                                                                                   Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE |
                                                                                                   Beryll::CollisionGroups::ENEMY_ATTACK,
                                                                                                   Beryll::SceneObjectGroups::GARBAGE);

            for(const auto& obj : garbageCopShield)
            {
                m_allGarbage.emplace_back(obj, GarbageType::ENEMY_GUN_SHIELD, EnumsAndVars::garbageStartHP);

                m_animatedOrDynamicObjects.push_back(obj);
                m_simpleObjForShadowMap.push_back(obj);

                obj->setDamping(EnumsAndVars::garbageLinearDamping, EnumsAndVars::garbageAngularDamping);
                obj->setGravity(EnumsAndVars::garbageGravityDefault, false, false);
            }
        }

        for(int i = 0; i < 5; ++i) // 5 * 4 = 20
        {
            const auto garbageCopGrenade = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map1/GarbageCopGrenade_3items.fbx",
                                                                                                    EnumsAndVars::garbageMass,
                                                                                                    false,
                                                                                                    Beryll::CollisionFlags::DYNAMIC,
                                                                                                    Beryll::CollisionGroups::GARBAGE,
                                                                                                   Beryll::CollisionGroups::GROUND | Beryll::CollisionGroups::BUILDING |
                                                                                                   Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE |
                                                                                                   Beryll::CollisionGroups::ENEMY_ATTACK,
                                                                                                    Beryll::SceneObjectGroups::GARBAGE);

            for(const auto& obj : garbageCopGrenade)
            {
                m_allGarbage.emplace_back(obj, GarbageType::ENEMY_GRENADE_LAUNCHER, EnumsAndVars::garbageStartHP);

                m_animatedOrDynamicObjects.push_back(obj);
                m_simpleObjForShadowMap.push_back(obj);

                obj->setDamping(EnumsAndVars::garbageLinearDamping, EnumsAndVars::garbageAngularDamping);
                obj->setGravity(EnumsAndVars::garbageGravityDefault, false, false);
            }
        }

        for(int i = 0; i < 5; ++i) // 5 * 2 = 10
        {
            const auto garbageCopSniper = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map1/GarbageCopSniper_2items.fbx",
                                                                                                   EnumsAndVars::garbageMass,
                                                                                                   false,
                                                                                                   Beryll::CollisionFlags::DYNAMIC,
                                                                                                   Beryll::CollisionGroups::GARBAGE,
                                                                                                   Beryll::CollisionGroups::GROUND | Beryll::CollisionGroups::BUILDING |
                                                                                                   Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE |
                                                                                                   Beryll::CollisionGroups::ENEMY_ATTACK,
                                                                                                   Beryll::SceneObjectGroups::GARBAGE);

            for(const auto& obj : garbageCopSniper)
            {
                m_allGarbage.emplace_back(obj, GarbageType::ENEMY_SNIPER, EnumsAndVars::garbageStartHP);

                m_animatedOrDynamicObjects.push_back(obj);
                m_simpleObjForShadowMap.push_back(obj);

                obj->setDamping(EnumsAndVars::garbageLinearDamping, EnumsAndVars::garbageAngularDamping);
                obj->setGravity(EnumsAndVars::garbageGravityDefault, false, false);
            }
        }

        for(int i = 0; i < 5; ++i) // 5 * 3 = 15
        {
            const auto garbageTank = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map1/GarbageTank_3items.fbx",
                                                                                              EnumsAndVars::garbageMass,
                                                                                              false,
                                                                                              Beryll::CollisionFlags::DYNAMIC,
                                                                                              Beryll::CollisionGroups::GARBAGE,
                                                                                                   Beryll::CollisionGroups::GROUND | Beryll::CollisionGroups::BUILDING |
                                                                                                   Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE |
                                                                                                   Beryll::CollisionGroups::ENEMY_ATTACK,
                                                                                              Beryll::SceneObjectGroups::GARBAGE);

            for(const auto& obj : garbageTank)
            {
                m_allGarbage.emplace_back(obj, GarbageType::ENEMY_TANK, EnumsAndVars::garbageStartHP);

                m_animatedOrDynamicObjects.push_back(obj);
                m_simpleObjForShadowMap.push_back(obj);

                obj->setDamping(EnumsAndVars::garbageLinearDamping, EnumsAndVars::garbageAngularDamping);
                obj->setGravity(EnumsAndVars::garbageGravityDefault, false, false);
            }
        }

        BR_INFO("Garbage::getCommonActiveCount() %d", Garbage::getCommonActiveCount());
    }

    void Map1::loadEnemies()
    {
        for(int i = 0; i < 110; ++i)
        {
            auto copPistol = std::make_shared<MovableEnemy>("models3D/enemies/CopWithPistol.fbx",
                                                            0.0f,
                                                            false,
                                                            Beryll::CollisionFlags::STATIC,
                                                            Beryll::CollisionGroups::NONE,
                                                            Beryll::CollisionGroups::NONE,
                                                            Beryll::SceneObjectGroups::ENEMY);

            copPistol->setCurrentAnimationByIndex(EnumsAndVars::AnimationIndexes::run, false, false);
            copPistol->setDefaultAnimationByIndex(EnumsAndVars::AnimationIndexes::stand);
            copPistol->unitType = UnitType::ENEMY_GUN;
            copPistol->attackType = AttackType::RANGE_DAMAGE_ONE;

            copPistol->damage = 5.0f;
            copPistol->attackDistance = 100.0f + Beryll::RandomGenerator::getFloat() * 100.0f;
            copPistol->timeBetweenAttacks = 1.5f + Beryll::RandomGenerator::getFloat() * 0.1f;

            copPistol->garbageAmountToDie = 10;
            copPistol->reducePlayerSpeedWhenDie = 6.0f;
            copPistol->experienceWhenDie = 25;
            copPistol->getController().moveSpeed = 25.0f;

            m_animatedOrDynamicObjects.push_back(copPistol);
            m_allAnimatedEnemies.push_back(copPistol);
            m_animatedObjForShadowMap.push_back(copPistol);
        }

        for(int i = 0; i < 100; ++i)
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

            copShield->damage = 5.0f;
            copShield->attackDistance = 70.0f + Beryll::RandomGenerator::getFloat() * 50.0f;
            copShield->timeBetweenAttacks = 1.5f + Beryll::RandomGenerator::getFloat() * 0.1f;

            copShield->garbageAmountToDie = 10;
            copShield->reducePlayerSpeedWhenDie = 10.0f;
            copShield->experienceWhenDie = 30;
            copShield->getController().moveSpeed = 20.0f;

            m_animatedOrDynamicObjects.push_back(copShield);
            m_allAnimatedEnemies.push_back(copShield);
            m_animatedObjForShadowMap.push_back(copShield);
        }

        for(int i = 0; i < 10; ++i)
        {
            auto sniper = std::make_shared<StaticEnemy>("models3D/enemies/Sniper.fbx",
                                                        0.0f,
                                                        false,
                                                        Beryll::CollisionFlags::STATIC,
                                                        Beryll::CollisionGroups::NONE,
                                                        Beryll::CollisionGroups::NONE,
                                                        Beryll::SceneObjectGroups::ENEMY);

            sniper->setCurrentAnimationByIndex(EnumsAndVars::AnimationIndexes::stand, false, false);
            sniper->setDefaultAnimationByIndex(EnumsAndVars::AnimationIndexes::stand);
            sniper->unitType = UnitType::ENEMY_SNIPER;
            sniper->attackType = AttackType::RANGE_DAMAGE_ONE;

            sniper->damage = 15.0f;
            sniper->attackDistance = 2500.0f;
            sniper->timeBetweenAttacks = 3.0f + Beryll::RandomGenerator::getFloat() * 0.1f;

            sniper->garbageAmountToDie = 10;
            sniper->reducePlayerSpeedWhenDie = 0.0f;
            sniper->experienceWhenDie = 30;

            m_animatedOrDynamicObjects.push_back(sniper);
            m_allAnimatedEnemies.push_back(sniper);
            m_animatedObjForShadowMap.push_back(sniper);
        }

        for(int i = 0; i < 70; ++i)
        {
            auto copGrenade = std::make_shared<MovableEnemy>("models3D/enemies/CopWithGrenadeLauncher.fbx",
                                                             0.0f,
                                                             false,
                                                             Beryll::CollisionFlags::STATIC,
                                                             Beryll::CollisionGroups::NONE,
                                                             Beryll::CollisionGroups::NONE,
                                                             Beryll::SceneObjectGroups::ENEMY);

            copGrenade->setCurrentAnimationByIndex(EnumsAndVars::AnimationIndexes::run, false, false);
            copGrenade->setDefaultAnimationByIndex(EnumsAndVars::AnimationIndexes::stand);
            copGrenade->unitType = UnitType::ENEMY_GRENADE_LAUNCHER;
            copGrenade->attackType = AttackType::RANGE_DAMAGE_RADIUS;

            copGrenade->damage = 1.0f;
            copGrenade->attackDistance = 130.0f + Beryll::RandomGenerator::getFloat() * 120.0f;
            copGrenade->damageRadius = 3.0f;
            copGrenade->timeBetweenAttacks = 2.0f + Beryll::RandomGenerator::getFloat() * 0.1f;

            copGrenade->garbageAmountToDie = 10;
            copGrenade->reducePlayerSpeedWhenDie = 8.0f;
            copGrenade->experienceWhenDie = 50;
            copGrenade->getController().moveSpeed = 20.0f;

            m_animatedOrDynamicObjects.push_back(copGrenade);
            m_allAnimatedEnemies.push_back(copGrenade);
            m_animatedObjForShadowMap.push_back(copGrenade);
        }

        for(int i = 0; i < 20; ++i)
        {
            auto tank = std::make_shared<MovableEnemy>("models3D/enemies/Tank.fbx",
                                                       0.0f,
                                                       false,
                                                       Beryll::CollisionFlags::STATIC,
                                                       Beryll::CollisionGroups::NONE,
                                                       Beryll::CollisionGroups::NONE,
                                                       Beryll::SceneObjectGroups::ENEMY);

            tank->setCurrentAnimationByIndex(EnumsAndVars::AnimationIndexes::run, false, false);
            tank->setDefaultAnimationByIndex(EnumsAndVars::AnimationIndexes::stand);
            tank->unitType = UnitType::ENEMY_TANK;
            tank->attackType = AttackType::RANGE_DAMAGE_RADIUS;

            tank->damage = 2.0f;
            tank->attackDistance = 200.0f + Beryll::RandomGenerator::getFloat() * 300.0f;
            tank->damageRadius = 6.0f;
            tank->timeBetweenAttacks = 2.5f + Beryll::RandomGenerator::getFloat() * 0.1f;

            tank->garbageAmountToDie = 25;
            tank->reducePlayerSpeedWhenDie = 20.0f;
            tank->experienceWhenDie = 200;
            tank->getController().moveSpeed = 15.0f;

            m_animatedOrDynamicObjects.push_back(tank);
            m_allAnimatedEnemies.push_back(tank);
            m_animatedObjForShadowMap.push_back(tank);
        }
    }

    void Map1::loadBoss()
    {
        m_boss = std::make_shared<TankWithCommander>("models3D/bosses/BossTankWithCommander.fbx",
                                                     0.0f,
                                                     false,
                                                     Beryll::CollisionFlags::STATIC,
                                                     Beryll::CollisionGroups::BOSS,
                                                     Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::CAMERA,
                                                     Beryll::SceneObjectGroups::BOSS,
                                                     100.0f);

        m_boss->setCurrentAnimationByIndex(EnumsAndVars::AnimationIndexes::run, false, false);
        m_boss->setDefaultAnimationByIndex(EnumsAndVars::AnimationIndexes::stand);

        m_animatedOrDynamicObjects.push_back(m_boss);
    }

    void Map1::spawnEnemies()
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

            int gunCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(gunCount < 15 && enemy->unitType == UnitType::ENEMY_GUN)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("Prepare wave 2. Max enemies: %d", EnumsAndVars::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave3 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave3Time)
        {
            m_prepareWave3 = false;

            EnumsAndVars::enemiesMaxActiveCountOnGround = 0;

            int gunCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(gunCount < 50 && enemy->unitType == UnitType::ENEMY_GUN)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("Prepare wave 3. Max enemies: %d", EnumsAndVars::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave4 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave4Time)
        {
            m_prepareWave4 = false;

            EnumsAndVars::enemiesMaxActiveCountOnGround = 0;

            int gunCount = 0;
            int gunShieldCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(gunCount < 50 && enemy->unitType == UnitType::ENEMY_GUN)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(gunShieldCount < 25 && enemy->unitType == UnitType::ENEMY_GUN_SHIELD)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunShieldCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("Prepare wave 4. Max enemies: %d", EnumsAndVars::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave5 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave5Time)
        {
            m_prepareWave5 = false;

            EnumsAndVars::enemiesMaxActiveCountOnGround = 0;

            int gunCount = 0;
            int gunShieldCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(gunCount < 60 && enemy->unitType == UnitType::ENEMY_GUN)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(gunShieldCount < 50 && enemy->unitType == UnitType::ENEMY_GUN_SHIELD)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunShieldCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("Prepare wave 5. Max enemies: %d", EnumsAndVars::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave6 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave6Time)
        {
            m_prepareWave6 = false;

            EnumsAndVars::enemiesMaxActiveCountOnGround = 0;

            int gunCount = 0;
            int gunShieldCount = 0;
            int sniperCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(gunCount < 70 && enemy->unitType == UnitType::ENEMY_GUN)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(gunShieldCount < 66 && enemy->unitType == UnitType::ENEMY_GUN_SHIELD)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunShieldCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(sniperCount < 4 && enemy->unitType == UnitType::ENEMY_SNIPER)
                {
                    enemy->isCanBeSpawned = true;
                    ++sniperCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("Prepare wave 6. Max enemies: %d", EnumsAndVars::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave7 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave7Time)
        {
            m_prepareWave7 = false;

            EnumsAndVars::enemiesMaxActiveCountOnGround = 0;

            int gunCount = 0;
            int gunShieldCount = 0;
            int sniperCount = 0;
            int grenadeLauncherCount = 0;
            int tankCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(gunCount < 80 && enemy->unitType == UnitType::ENEMY_GUN)
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
                else if(sniperCount < 5 && enemy->unitType == UnitType::ENEMY_SNIPER)
                {
                    enemy->isCanBeSpawned = true;
                    ++sniperCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(grenadeLauncherCount < 20 && enemy->unitType == UnitType::ENEMY_GRENADE_LAUNCHER)
                {
                    enemy->isCanBeSpawned = true;
                    ++grenadeLauncherCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(tankCount < 1 && enemy->unitType == UnitType::ENEMY_TANK)
                {
                    enemy->isCanBeSpawned = true;
                    ++tankCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("Prepare wave 7. Max enemies: %d", EnumsAndVars::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave8 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave8Time)
        {
            m_prepareWave8 = false;

            EnumsAndVars::enemiesMaxActiveCountOnGround = 0;

            int gunCount = 0;
            int gunShieldCount = 0;
            int sniperCount = 0;
            int grenadeLauncherCount = 0;
            int tankCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(gunCount < 90 && enemy->unitType == UnitType::ENEMY_GUN)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(gunShieldCount < 80 && enemy->unitType == UnitType::ENEMY_GUN_SHIELD)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunShieldCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(sniperCount < 6 && enemy->unitType == UnitType::ENEMY_SNIPER)
                {
                    enemy->isCanBeSpawned = true;
                    ++sniperCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(grenadeLauncherCount < 30 && enemy->unitType == UnitType::ENEMY_GRENADE_LAUNCHER)
                {
                    enemy->isCanBeSpawned = true;
                    ++grenadeLauncherCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(tankCount < 4 && enemy->unitType == UnitType::ENEMY_TANK)
                {
                    enemy->isCanBeSpawned = true;
                    ++tankCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("Prepare wave 8. Max enemies: %d", EnumsAndVars::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave9 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave9Time)
        {
            m_prepareWave9 = false;

            EnumsAndVars::enemiesMaxActiveCountOnGround = 0;

            int gunCount = 0;
            int gunShieldCount = 0;
            int sniperCount = 0;
            int grenadeLauncherCount = 0;
            int tankCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(gunCount < 100 && enemy->unitType == UnitType::ENEMY_GUN)
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
                else if(sniperCount < 7 && enemy->unitType == UnitType::ENEMY_SNIPER)
                {
                    enemy->isCanBeSpawned = true;
                    ++sniperCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(grenadeLauncherCount < 40 && enemy->unitType == UnitType::ENEMY_GRENADE_LAUNCHER)
                {
                    enemy->isCanBeSpawned = true;
                    ++grenadeLauncherCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(tankCount < 8 && enemy->unitType == UnitType::ENEMY_TANK)
                {
                    enemy->isCanBeSpawned = true;
                    ++tankCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("Prepare wave 9. Max enemies: %d", EnumsAndVars::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave10 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave10Time)
        {
            m_prepareWave10 = false;

            EnumsAndVars::enemiesMaxActiveCountOnGround = 0;

            int gunCount = 0;
            int gunShieldCount = 0;
            int sniperCount = 0;
            int grenadeLauncherCount = 0;
            int tankCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(gunCount < 110 && enemy->unitType == UnitType::ENEMY_GUN)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(gunShieldCount < 100 && enemy->unitType == UnitType::ENEMY_GUN_SHIELD)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunShieldCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(sniperCount < 8 && enemy->unitType == UnitType::ENEMY_SNIPER)
                {
                    enemy->isCanBeSpawned = true;
                    ++sniperCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(grenadeLauncherCount < 50 && enemy->unitType == UnitType::ENEMY_GRENADE_LAUNCHER)
                {
                    enemy->isCanBeSpawned = true;
                    ++grenadeLauncherCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(tankCount < 12 && enemy->unitType == UnitType::ENEMY_TANK)
                {
                    enemy->isCanBeSpawned = true;
                    ++tankCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("Prepare wave 10. Max enemies: %d", EnumsAndVars::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave11 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave11Time)
        {
            m_prepareWave11 = false;

            EnumsAndVars::enemiesMaxActiveCountOnGround = 0;

            int gunCount = 0;
            int gunShieldCount = 0;
            int sniperCount = 0;
            int grenadeLauncherCount = 0;
            int tankCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(gunCount < 110 && enemy->unitType == UnitType::ENEMY_GUN)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(gunShieldCount < 100 && enemy->unitType == UnitType::ENEMY_GUN_SHIELD)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunShieldCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(sniperCount < 9 && enemy->unitType == UnitType::ENEMY_SNIPER)
                {
                    enemy->isCanBeSpawned = true;
                    ++sniperCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(grenadeLauncherCount < 60 && enemy->unitType == UnitType::ENEMY_GRENADE_LAUNCHER)
                {
                    enemy->isCanBeSpawned = true;
                    ++grenadeLauncherCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(tankCount < 16 && enemy->unitType == UnitType::ENEMY_TANK)
                {
                    enemy->isCanBeSpawned = true;
                    ++tankCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("Prepare wave 11. Max enemies: %d", EnumsAndVars::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave12 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave12Time)
        {
            m_prepareWave12 = false;

            EnumsAndVars::enemiesMaxActiveCountOnGround = 0;

            int gunCount = 0;
            int gunShieldCount = 0;
            int sniperCount = 0;
            int grenadeLauncherCount = 0;
            int tankCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(gunCount < 110 && enemy->unitType == UnitType::ENEMY_GUN)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(gunShieldCount < 100 && enemy->unitType == UnitType::ENEMY_GUN_SHIELD)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunShieldCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(sniperCount < 10 && enemy->unitType == UnitType::ENEMY_SNIPER)
                {
                    enemy->isCanBeSpawned = true;
                    ++sniperCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(grenadeLauncherCount < 70 && enemy->unitType == UnitType::ENEMY_GRENADE_LAUNCHER)
                {
                    enemy->isCanBeSpawned = true;
                    ++grenadeLauncherCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(tankCount < 20 && enemy->unitType == UnitType::ENEMY_TANK)
                {
                    enemy->isCanBeSpawned = true;
                    ++tankCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("Prepare wave 12. Max enemies: %d", EnumsAndVars::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareToBoss && EnumsAndVars::mapPlayTimeSec > m_prepareToBossTime)
        {
            m_prepareToBoss = false;

            EnumsAndVars::enemiesMaxActiveCountOnGround = 0;

            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;
            }

            prepareToBossPhase();

            BR_INFO("prepareToBossPhase(). Max enemies: %d", EnumsAndVars::enemiesMaxActiveCountOnGround);
        }

        bool sortSnipersPositions = true;

        // Spawn enemies.
        if(!m_pointsToSpawnEnemies.empty())
        {
            for(const auto& enemy : m_allAnimatedEnemies)
            {
                if(BaseEnemy::getActiveCount() >= EnumsAndVars::enemiesMaxActiveCountOnGround)
                    break;

                if(enemy->getIsEnabledUpdate() || !enemy->isCanBeSpawned)
                    continue;

                if(enemy->unitType != UnitType::ENEMY_SNIPER)
                {
                    enemy->enableEnemy();
                    enemy->disableDraw();

                    const glm::ivec2 spawnPoint2D = m_pointsToSpawnEnemies[Beryll::RandomGenerator::getInt(m_pointsToSpawnEnemies.size() - 1)];

                    enemy->setPathArray(m_pathFinder.findPath(spawnPoint2D, m_playerClosestAllowedPos, 6), 1);

                    enemy->setOrigin(enemy->getStartPointMoveFrom());
                }
                // Spawn sniper.
                else if(StaticEnemy::spawnTime + StaticEnemy::spawnDelay < EnumsAndVars::mapPlayTimeSec)
                {
                    if(sortSnipersPositions)
                    {
                        sortSnipersPositions = false;

                        // Sort by distance from player.
                        std::sort(StaticEnemy::staticPositions.begin(), StaticEnemy::staticPositions.end(),
                                  [&](const StaticPosition& pos1, const StaticPosition& pos2)
                                  { return glm::distance(m_player->getObj()->getOrigin(), pos1.position) < glm::distance(m_player->getObj()->getOrigin(), pos2.position); });
                    }

                    for(StaticPosition& sniperPos : StaticEnemy::staticPositions)
                    {
                        Beryll::RayClosestHit rayBuildingHit = Beryll::Physics::castRayClosestHit(m_player->getObj()->getOrigin(),
                                                                                                  sniperPos.position,
                                                                                                  Beryll::CollisionGroups::RAY_FOR_ENVIRONMENT,
                                                                                                  Beryll::CollisionGroups::BUILDING);

                        // Check position in free + not closest than 150m + sniper see player(rayBuildingHit does not hit).
                        if(sniperPos.isFreePosition && !rayBuildingHit &&
                           glm::distance(m_player->getObj()->getOrigin(), sniperPos.position) > 150.0f)
                        {
                            sniperPos.isFreePosition = false;

                            enemy->enableEnemy();
                            enemy->disableDraw();
                            enemy->setOrigin(sniperPos.position);
                            //BR_INFO("Spawn sniper. Y = %f", sniperPos.position.y);

                            StaticEnemy::spawnTime = EnumsAndVars::mapPlayTimeSec;
                            break;
                        }
                    }
                }
            }
        }

        //BR_INFO("BaseEnemy::getActiveCount(): %d", BaseEnemy::getActiveCount());
    }

    void Map1::startBossPhase()
    {
        BR_INFO("%s", "startBossPhase()");

        m_gui->showMenuBossTankWithCommander();
        EnumsAndVars::bossPhase = true;
        EnumsAndVars::prepareToBossPhase = false;

        // Enable boss.
        m_boss->enableBoss();

        std::vector<glm::ivec2> pointsToSpawnBoss;
        pointsToSpawnBoss.reserve(200);
        glm::vec2 playerPosXZ{m_player->getObj()->getOrigin().x, m_player->getObj()->getOrigin().z};
        float distanceToClosestPoint = std::numeric_limits<float>::max();
        float distanceToCurrent = 0.0f;
        glm::ivec2 playerClosestAllowedPos;

        for(const glm::ivec2& point : m_pathAllowedPositionsXZBoss)
        {
            distanceToCurrent = glm::distance(playerPosXZ, glm::vec2(float(point.x), float(point.y)));

            if(distanceToCurrent < distanceToClosestPoint)
            {
                distanceToClosestPoint = distanceToCurrent;
                playerClosestAllowedPos = point;
            }

            if(glm::distance(playerPosXZ, glm::vec2(float(point.x), float(point.y))) > 500.0f)
            {
                pointsToSpawnBoss.push_back(point);
            }
        }

        BR_INFO("pointsToSpawnBoss.size() %d", pointsToSpawnBoss.size());
        BR_ASSERT((pointsToSpawnBoss.empty() == false), "%s", "pointsToSpawnBoss.empty()");

        const glm::ivec2 spawnPoint2D = pointsToSpawnBoss[Beryll::RandomGenerator::getInt(pointsToSpawnBoss.size() - 1)];
        glm::vec3 spawnPoint3D{spawnPoint2D.x,
                               m_boss->getFromOriginToBottom(),
                               spawnPoint2D.y};
        m_boss->setOrigin(spawnPoint3D);

        m_boss->pathArray = m_pathFinderBoss.findPath(spawnPoint2D, playerClosestAllowedPos, 10);
        if(m_boss->pathArray.size() > 1)
            m_boss->pathArrayIndexToMove = 1;
        else
            m_boss->pathArrayIndexToMove = 0;

        m_boss->currentPointToMove2DIntegers = m_boss->pathArray[m_boss->pathArrayIndexToMove];
        m_boss->currentPointToMove3DFloats = glm::vec3(m_boss->currentPointToMove2DIntegers.x,
                                                       m_boss->getFromOriginToBottom(),
                                                       m_boss->currentPointToMove2DIntegers.y);
    }

    void Map1::handlePossPhase()
    {
        // Handle boss specific to this map: Tank with commander.
        // 1. Update boss.
        glm::vec2 playerPosXZ{m_player->getObj()->getOrigin().x, m_player->getObj()->getOrigin().z};
        float distanceToClosestPoint = std::numeric_limits<float>::max();
        float distanceToCurrent = 0.0f;
        glm::ivec2 playerClosestAllowedPos;

        for(const glm::ivec2& point : m_pathAllowedPositionsXZBoss)
        {
            distanceToCurrent = glm::distance(playerPosXZ, glm::vec2(float(point.x), float(point.y)));

            if(distanceToCurrent < distanceToClosestPoint)
            {
                distanceToClosestPoint = distanceToCurrent;
                playerClosestAllowedPos = point;
            }
        }
        m_boss->pathArray = m_pathFinderBoss.findPath(m_boss->currentPointToMove2DIntegers, playerClosestAllowedPos, 10);
        m_boss->pathArrayIndexToMove = 0;
        m_boss->currentPointToMove2DIntegers = m_boss->pathArray[m_boss->pathArrayIndexToMove];
        m_boss->currentPointToMove3DFloats = glm::vec3(m_boss->currentPointToMove2DIntegers.x,
                                                       m_boss->getFromOriginToBottom(),
                                                       m_boss->currentPointToMove2DIntegers.y);

        m_boss->update(m_player->getObj()->getOrigin());

        // 2. Boss attack.
        if(m_boss->bossState == BossState::CAN_ATTACK)
        {
            BR_INFO("%s", "BossState::CAN_ATTACK");

            glm::vec3 target = m_player->getObj()->getOrigin();
            target.y += m_player->getObj()->getFromOriginToTop() * 0.8f;
            Beryll::RayClosestHit rayAttack = Beryll::Physics::castRayClosestHit(m_boss->getOrigin(),
                                                                                 target,
                                                                                 Beryll::CollisionGroups::ENEMY_ATTACK,
                                                                                 Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE);

            if(rayAttack)
            {
                m_boss->attack(m_player->getObj()->getOrigin());

                // Spam particles.
                glm::vec3 from = m_boss->getOrigin(); // Calculate particles start point.
                from.y += m_boss->getFromOriginToTop() * 0.8f;
                from += m_boss->getFaceDirXZ() * 40.0f;
                emitParticlesLine(from, rayAttack.hitPoint, 2.0f, 2.0f,
                                  glm::vec4(1.0f, 0.5f, 0.0f, 1.0f), glm::vec4(1.0f, 0.5f, 0.0f, 0.7f), 0.6f);

                emitParticlesExplosion(rayAttack.hitPoint, 6, 4.0f, 4.0f,
                                       glm::vec4(1.0f, 0.5f, 0.0f, 1.0f), glm::vec4(1.0f, 0.5f, 0.0f, 0.7f), 1.2f);

                Sounds::playSoundEffect(SoundType::GRENADE_LAUNCHER_SHOT);

                if(rayAttack.hittedCollGroup == Beryll::CollisionGroups::PLAYER)
                {
                    m_player->takeDamage(100.0f);
                }
                else if(rayAttack.hittedCollGroup == Beryll::CollisionGroups::GARBAGE)
                {
                    for(auto& wrapper : m_allGarbage)
                    {
                        if(wrapper.isMagnetized)
                        {
                            float distance = glm::distance(rayAttack.hitPoint, wrapper.obj->getOrigin());

                            if(distance < 8.0f)
                                wrapper.takeDamage(25.0f);

                            if(distance < 20.0f)
                                wrapper.obj->applyCentralImpulse(glm::normalize(wrapper.obj->getOrigin() - rayAttack.hitPoint) * 4.0f);
                        }
                    }
                }
            }
        }

        // 3. Player attack boss.
        if(Beryll::Physics::getIsCollisionWithGroup(m_player->getObj()->getID(), Beryll::CollisionGroups::BOSS))
        {
            BR_INFO("%s", "Player collision with boss.");

            if(m_player->getObj()->getOrigin().y > m_boss->getOrigin().y + m_boss->getFromOriginToTop())
            {
                BR_INFO("%s", "Damage boss.");
                m_boss->takeDamage(m_boss->getMaxHP() * 0.35f); // Boss should die after 3 times damaged.

                // Move player away from boss.
                glm::vec3 moveAwayDir = Beryll::Camera::getCameraFrontDirectionXZ();
                moveAwayDir.y = 0.8f;
                m_player->getObj()->resetVelocities();
                m_player->getObj()->applyCentralImpulse(glm::normalize(moveAwayDir) * 1000.0f);
            }
            else
            {
                BR_INFO("%s", "Player is on boss side.");
                glm::vec3 moveAwayDir = m_player->getObj()->getOrigin() - m_boss->getOrigin();
                moveAwayDir.y = 0.0f;
                m_player->getObj()->applyCentralImpulse(glm::normalize(moveAwayDir) * 30.0f);
            }
        }

        if(m_boss->getIsDie())
            EnumsAndVars::mapPlayerWin = true;
    }
}
