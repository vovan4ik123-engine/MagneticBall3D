#include "Map2.h"
#include "EnumsAndVariables.h"
#include "enemies/MovableEnemy.h"
#include "enemies/StaticEnemy.h"
#include "Sounds.h"

namespace MagneticBall3D
{
    Map2::Map2(std::shared_ptr<PlayStateGUILayer> gui) : BaseMap(std::move(gui))
    {
        loadPlayer();
        m_player->getObj()->setOrigin(glm::vec3(-120.0f, 2.0f,450.0f));
        m_improvements.setPlayer(m_player);
        Beryll::LoadingScreen::showProgress(20.0f);
        loadEnv();
        Beryll::LoadingScreen::showProgress(40.0f);
        loadGarbage();
        BR_ASSERT((m_allGarbage.size() < m_maxGarbageCount), "%s", "m_allGarbage reallocation happened. Increase maxGarbageCount.");
        Beryll::LoadingScreen::showProgress(60.0f);
        loadEnemies();
        Beryll::LoadingScreen::showProgress(80.0f);
        loadBoss();
        Beryll::LoadingScreen::showProgress(90.0f);

        StaticEnemy::staticPositions = std::vector<StaticPosition>{{glm::vec3(-409.0f, 155.5f, -460.0f)},
                                                                   {glm::vec3(500.5f, 130.0f, 372.0f)}
        };
        StaticEnemy::spawnDelay = 40.0f;

        loadShaders();
        m_cameraAngleOffset = glm::normalize(glm::vec3(0.5f, 0.0f, 1.0f));
        handleCamera();

        m_minX = -800.0f;
        m_maxX = 800.0f;
        m_minZ = -800.0f;
        m_maxZ = 800.0f;

        m_pathFinderEnemies = AStar(m_minX, m_maxX, m_minZ, m_maxZ, 20);
        std::vector<glm::vec3> walls = BeryllUtils::Common::loadMeshVerticesToVector("models3D/map2/PathEnemiesWalls.fbx");
        for(const auto& wall : walls)
        {
            m_pathFinderEnemies.addWallPosition({(int)std::roundf(wall.x), (int)std::roundf(wall.z)});
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

        m_dirToSun = glm::normalize(glm::vec3(-1.0f, 1.7f, 1.0f));
        m_sunLightDir = -m_dirToSun;

        m_skyBox = Beryll::Renderer::createSkyBox("skyboxes/whiteClouds");

        SendStatisticsHelper::sendMapStart();

        Beryll::LoadingScreen::showProgress(100.0f);

        Sounds::startBackgroundMusic(SoundType::BACKGROUND_MUSIC_2);

        //BR_INFO(" %f", );
        //BR_INFO("%s", "");
    }

    Map2::~Map2()
    {
        Sounds::stopBackgroundMusic();
    }

    void Map2::draw()
    {
        //BR_INFO("%s", "Scene draw call.");
        m_improvements.draw();

        // 1. Draw into shadow map.
        glm::vec3 sunPos = m_player->getObj()->getOrigin() +
                           (Beryll::Camera::getCameraFrontDirectionXZ() * 200.0f) +
                           (m_dirToSun * (600.0f - m_player->getObj()->getOrigin().y)); // sunPos.y is 600 max.
        updateSunPosition(sunPos, m_shadowsCubeWidth, m_shadowsCubeHeight, m_shadowsCubeDepth);

        Beryll::Renderer::disableFaceCulling();
        if(BaseEnemy::getActiveCount() < 300)
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
        m_simpleObjSunLightShadowsNormals->set1Float("specularLightStrength", 0.2f);

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
        mainGround->setFriction(EnumsAndVars::staticEnvFriction);

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
            m_simpleObjForShadowMap.push_back(obj);
            obj->setFriction(EnumsAndVars::staticEnvFriction);
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
            obj->setFriction(EnumsAndVars::staticEnvFriction);
        }

        const auto jumpPads = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map2/JumpPads.fbx",
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

        const auto envNoColliders1 = Beryll::SimpleObject::loadManyModelsFromOneFile("models3D/map2/EnvNoColliders.fbx", Beryll::SceneObjectGroups::BUILDING);

        for(const auto& obj : envNoColliders1)
        {
            m_staticEnv.push_back(obj);
            m_simpleObjForShadowMap.push_back(obj);
        }
    }

    void Map2::loadGarbage()
    {
        for(int i = 0; i < 6; ++i) // 6 * 32 = 192
        {
            const auto garbageCommon = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map2/GarbageCommon_32items.fbx",
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
            const auto garbageEnemy = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map2/GarbageEnemyGun_4items.fbx",
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

        for(int i = 0; i < 5; ++i) // 5 * 4 = 20
        {
            const auto garbageEnemy = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map2/GarbageEnemyGunShield_4items.fbx",
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

        for(int i = 0; i < 2; ++i) // 2 * 2 = 4
        {
            const auto garbageEnemy = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map2/GarbageEnemySniper_2items.fbx",
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
                m_allGarbage.emplace_back(obj, GarbageType::ENEMY_SNIPER, EnumsAndVars::garbageStartHP);

                m_animatedOrDynamicObjects.push_back(obj);
                m_simpleObjForShadowMap.push_back(obj);

                obj->setDamping(EnumsAndVars::garbageLinearDamping, EnumsAndVars::garbageAngularDamping);
                obj->setGravity(EnumsAndVars::garbageGravityDefault, false, false);
            }
        }

        for(int i = 0; i < 7; ++i) // 7 * 3 = 21
        {
            const auto garbageEnemy = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map2/GarbageEnemyMagnet_3items.fbx",
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
                m_allGarbage.emplace_back(obj, GarbageType::ENEMY_MAGNET, EnumsAndVars::garbageStartHP);

                m_animatedOrDynamicObjects.push_back(obj);
                m_simpleObjForShadowMap.push_back(obj);

                obj->setDamping(EnumsAndVars::garbageLinearDamping, EnumsAndVars::garbageAngularDamping);
                obj->setGravity(EnumsAndVars::garbageGravityDefault, false, false);
            }
        }

        for(int i = 0; i < 4; ++i) // 4 * 4 = 16
        {
            const auto garbageEnemy = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map2/GarbageEnemyRocket_4items.fbx",
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
                m_allGarbage.emplace_back(obj, GarbageType::ENEMY_ROCKET, EnumsAndVars::garbageStartHP);

                m_animatedOrDynamicObjects.push_back(obj);
                m_simpleObjForShadowMap.push_back(obj);

                obj->setDamping(EnumsAndVars::garbageLinearDamping, EnumsAndVars::garbageAngularDamping);
                obj->setGravity(EnumsAndVars::garbageGravityDefault, false, false);
            }
        }

        BR_INFO("Garbage::getCommonActiveCount() %d", Garbage::getCommonActiveCount());
    }

    void Map2::loadEnemies()
    {
        for(int i = 0; i < 150; ++i)
        {
            auto rat = std::make_shared<MovableEnemy>("models3D/enemies/RatWithMagnet.fbx",
                                                            0.0f,
                                                            false,
                                                            Beryll::CollisionFlags::STATIC,
                                                            Beryll::CollisionGroups::NONE,
                                                            Beryll::CollisionGroups::NONE,
                                                            Beryll::SceneObjectGroups::ENEMY);

            rat->setCurrentAnimationByIndex(EnumsAndVars::AnimationIndexes::run, false, false);
            rat->setDefaultAnimationByIndex(EnumsAndVars::AnimationIndexes::stand);
            rat->unitType = UnitType::ENEMY_MAGNET;
            rat->attackType = AttackType::MAGNETIZE_GARBAGE;
            rat->attackSound = SoundType::NONE;
            rat->attackHitSound = SoundType::NONE;
            rat->dieSound = SoundType::POP;
            rat->dieGarbageType = GarbageType::ENEMY_MAGNET;
            rat->castRayToFindYPos = true;

            rat->attackDistance = 80.0f + Beryll::RandomGenerator::getFloat() * 50.0f;
            rat->timeBetweenAttacks = 2.5f + Beryll::RandomGenerator::getFloat();

            rat->garbageAmountToDie = 4;
            rat->reducePlayerSpeedWhenDie = 5.0f;
            rat->experienceWhenDie = 25;
            rat->getController().moveSpeed = 55.0f;

            m_animatedOrDynamicObjects.push_back(rat);
            m_allAnimatedEnemies.push_back(rat);
            m_animatedObjForShadowMap.push_back(rat);
        }

        for(int i = 0; i < 100; ++i)
        {
            auto guard = std::make_shared<MovableEnemy>("models3D/enemies/JunkyardGuard.fbx",
                                                        0.0f,
                                                        false,
                                                        Beryll::CollisionFlags::STATIC,
                                                        Beryll::CollisionGroups::NONE,
                                                        Beryll::CollisionGroups::NONE,
                                                        Beryll::SceneObjectGroups::ENEMY);

            guard->setCurrentAnimationByIndex(EnumsAndVars::AnimationIndexes::run, false, false);
            guard->setDefaultAnimationByIndex(EnumsAndVars::AnimationIndexes::stand);
            guard->unitType = UnitType::ENEMY_GUN;
            guard->attackType = AttackType::RANGE_DAMAGE_ONE;
            guard->attackSound = SoundType::PISTOL_SHOT;
            guard->attackHitSound = SoundType::PISTOL_HIT;
            guard->dieSound = SoundType::POP;
            guard->dieGarbageType = GarbageType::ENEMY_GUN;
            guard->castRayToFindYPos = true;

            guard->damage = 5.0f;
            guard->attackDistance = 100.0f + Beryll::RandomGenerator::getFloat() * 100.0f;
            guard->timeBetweenAttacks = 1.5f + Beryll::RandomGenerator::getFloat() * 0.2f;

            guard->garbageAmountToDie = 12;
            guard->reducePlayerSpeedWhenDie = 10.0f;
            guard->experienceWhenDie = 30;
            guard->getController().moveSpeed = 40.0f;

            m_animatedOrDynamicObjects.push_back(guard);
            m_allAnimatedEnemies.push_back(guard);
            m_animatedObjForShadowMap.push_back(guard);
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
            copShield->attackSound = SoundType::PISTOL_SHOT;
            copShield->attackHitSound = SoundType::PISTOL_HIT;
            copShield->dieSound = SoundType::POP;
            copShield->dieGarbageType = GarbageType::ENEMY_GUN_SHIELD;
            copShield->castRayToFindYPos = true;

            copShield->damage = 5.0f;
            copShield->attackDistance = 70.0f + Beryll::RandomGenerator::getFloat() * 50.0f;
            copShield->timeBetweenAttacks = 1.5f + Beryll::RandomGenerator::getFloat() * 0.2f;

            copShield->garbageAmountToDie = 10;
            copShield->reducePlayerSpeedWhenDie = 10.0f;
            copShield->experienceWhenDie = 30;
            copShield->getController().moveSpeed = 45.0f;

            m_animatedOrDynamicObjects.push_back(copShield);
            m_allAnimatedEnemies.push_back(copShield);
            m_animatedObjForShadowMap.push_back(copShield);
        }

        for(int i = 0; i < 2; ++i)
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
            sniper->attackSound = SoundType::RIFLE_SHOT;
            sniper->attackHitSound = SoundType::PISTOL_HIT;
            sniper->dieSound = SoundType::POP;
            sniper->dieGarbageType = GarbageType::ENEMY_SNIPER;

            sniper->damage = 20.0f;
            sniper->attackDistance = 2500.0f;
            sniper->timeBetweenAttacks = 3.0f + Beryll::RandomGenerator::getFloat() * 0.2f;

            sniper->garbageAmountToDie = 10;
            sniper->reducePlayerSpeedWhenDie = 0.0f;
            sniper->experienceWhenDie = 30;

            m_animatedOrDynamicObjects.push_back(sniper);
            m_allAnimatedEnemies.push_back(sniper);
            m_animatedObjForShadowMap.push_back(sniper);
        }

        for(int i = 0; i < 20; ++i)
        {
            auto junkyardBusRocket = std::make_shared<MovableEnemy>("models3D/enemies/JunkyardBus.fbx",
                                                       0.0f,
                                                       false,
                                                       Beryll::CollisionFlags::STATIC,
                                                       Beryll::CollisionGroups::NONE,
                                                       Beryll::CollisionGroups::NONE,
                                                       Beryll::SceneObjectGroups::ENEMY);

            junkyardBusRocket->setCurrentAnimationByIndex(EnumsAndVars::AnimationIndexes::run, false, false);
            junkyardBusRocket->setDefaultAnimationByIndex(EnumsAndVars::AnimationIndexes::stand);
            junkyardBusRocket->unitType = UnitType::ENEMY_ROCKET;
            junkyardBusRocket->attackType = AttackType::RANGE_DAMAGE_RADIUS;
            junkyardBusRocket->attackSound = SoundType::BIG_ROCKET_LAUNCH;
            junkyardBusRocket->attackHitSound = SoundType::NONE;
            junkyardBusRocket->dieSound = SoundType::POP;
            junkyardBusRocket->dieGarbageType = GarbageType::ENEMY_ROCKET;
            junkyardBusRocket->castRayToFindYPos = true;

            junkyardBusRocket->damage = 2.0f;
            junkyardBusRocket->attackDistance = 200.0f + Beryll::RandomGenerator::getFloat() * 300.0f;
            junkyardBusRocket->damageRadius = 6.0f;
            junkyardBusRocket->timeBetweenAttacks = 2.5f + Beryll::RandomGenerator::getFloat() * 0.2f;

            junkyardBusRocket->garbageAmountToDie = 25;
            junkyardBusRocket->reducePlayerSpeedWhenDie = 20.0f;
            junkyardBusRocket->experienceWhenDie = 200;
            junkyardBusRocket->getController().moveSpeed = 50.0f;

            m_animatedOrDynamicObjects.push_back(junkyardBusRocket);
            m_allAnimatedEnemies.push_back(junkyardBusRocket);
            m_animatedObjForShadowMap.push_back(junkyardBusRocket);
        }
    }

    void Map2::loadBoss()
    {

    }

    void Map2::spawnEnemies()
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

            int ratCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(ratCount < 50 && enemy->unitType == UnitType::ENEMY_MAGNET)
                {
                    enemy->isCanBeSpawned = true;
                    ++ratCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("Prepare wave 2. Max enemies: %d", EnumsAndVars::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave3 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave3Time)
        {
            m_prepareWave3 = false;

            EnumsAndVars::enemiesMaxActiveCountOnGround = 0;

            int ratCount = 0;
            int gunCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(ratCount < 70 && enemy->unitType == UnitType::ENEMY_MAGNET)
                {
                    enemy->isCanBeSpawned = true;
                    ++ratCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(gunCount < 20 && enemy->unitType == UnitType::ENEMY_GUN)
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

            int ratCount = 0;
            int gunCount = 0;
            int gunShieldCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(ratCount < 90 && enemy->unitType == UnitType::ENEMY_MAGNET)
                {
                    enemy->isCanBeSpawned = true;
                    ++ratCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(gunCount < 30 && enemy->unitType == UnitType::ENEMY_GUN)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(gunShieldCount < 20 && enemy->unitType == UnitType::ENEMY_GUN_SHIELD)
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

            int ratCount = 0;
            int gunCount = 0;
            int gunShieldCount = 0;
            int sniperCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(ratCount < 110 && enemy->unitType == UnitType::ENEMY_MAGNET)
                {
                    enemy->isCanBeSpawned = true;
                    ++ratCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(gunCount < 40 && enemy->unitType == UnitType::ENEMY_GUN)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(gunShieldCount < 30 && enemy->unitType == UnitType::ENEMY_GUN_SHIELD)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunShieldCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(sniperCount < 2 && enemy->unitType == UnitType::ENEMY_SNIPER)
                {
                    enemy->isCanBeSpawned = true;
                    ++sniperCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("Prepare wave 5. Max enemies: %d", EnumsAndVars::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave6 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave6Time)
        {
            m_prepareWave6 = false;

            EnumsAndVars::enemiesMaxActiveCountOnGround = 0;

            int ratCount = 0;
            int gunCount = 0;
            int gunShieldCount = 0;
            int sniperCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(ratCount < 130 && enemy->unitType == UnitType::ENEMY_MAGNET)
                {
                    enemy->isCanBeSpawned = true;
                    ++ratCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(gunCount < 50 && enemy->unitType == UnitType::ENEMY_GUN)
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
                else if(sniperCount < 2 && enemy->unitType == UnitType::ENEMY_SNIPER)
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

            int ratCount = 0;
            int gunCount = 0;
            int gunShieldCount = 0;
            int sniperCount = 0;
            int rocketCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(ratCount < 150 && enemy->unitType == UnitType::ENEMY_MAGNET)
                {
                    enemy->isCanBeSpawned = true;
                    ++ratCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(gunCount < 60 && enemy->unitType == UnitType::ENEMY_GUN)
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
                else if(sniperCount < 2 && enemy->unitType == UnitType::ENEMY_SNIPER)
                {
                    enemy->isCanBeSpawned = true;
                    ++sniperCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(rocketCount < 4 && enemy->unitType == UnitType::ENEMY_ROCKET)
                {
                    enemy->isCanBeSpawned = true;
                    ++rocketCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("Prepare wave 7. Max enemies: %d", EnumsAndVars::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave8 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave8Time)
        {
            m_prepareWave8 = false;

            EnumsAndVars::enemiesMaxActiveCountOnGround = 0;

            int ratCount = 0;
            int gunCount = 0;
            int gunShieldCount = 0;
            int sniperCount = 0;
            int rocketCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(ratCount < 150 && enemy->unitType == UnitType::ENEMY_MAGNET)
                {
                    enemy->isCanBeSpawned = true;
                    ++ratCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(gunCount < 70 && enemy->unitType == UnitType::ENEMY_GUN)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(gunShieldCount < 60 && enemy->unitType == UnitType::ENEMY_GUN_SHIELD)
                {
                    enemy->isCanBeSpawned = true;
                    ++gunShieldCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(sniperCount < 2 && enemy->unitType == UnitType::ENEMY_SNIPER)
                {
                    enemy->isCanBeSpawned = true;
                    ++sniperCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(rocketCount < 6 && enemy->unitType == UnitType::ENEMY_ROCKET)
                {
                    enemy->isCanBeSpawned = true;
                    ++rocketCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("Prepare wave 8. Max enemies: %d", EnumsAndVars::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave9 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave9Time)
        {
            m_prepareWave9 = false;

            EnumsAndVars::enemiesMaxActiveCountOnGround = 0;

            int ratCount = 0;
            int gunCount = 0;
            int gunShieldCount = 0;
            int sniperCount = 0;
            int rocketCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(ratCount < 150 && enemy->unitType == UnitType::ENEMY_MAGNET)
                {
                    enemy->isCanBeSpawned = true;
                    ++ratCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(gunCount < 80 && enemy->unitType == UnitType::ENEMY_GUN)
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
                else if(sniperCount < 2 && enemy->unitType == UnitType::ENEMY_SNIPER)
                {
                    enemy->isCanBeSpawned = true;
                    ++sniperCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(rocketCount < 8 && enemy->unitType == UnitType::ENEMY_ROCKET)
                {
                    enemy->isCanBeSpawned = true;
                    ++rocketCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("Prepare wave 9. Max enemies: %d", EnumsAndVars::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave10 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave10Time)
        {
            m_prepareWave10 = false;

            EnumsAndVars::enemiesMaxActiveCountOnGround = 0;

            int ratCount = 0;
            int gunCount = 0;
            int gunShieldCount = 0;
            int sniperCount = 0;
            int rocketCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(ratCount < 150 && enemy->unitType == UnitType::ENEMY_MAGNET)
                {
                    enemy->isCanBeSpawned = true;
                    ++ratCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(gunCount < 90 && enemy->unitType == UnitType::ENEMY_GUN)
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
                else if(sniperCount < 2 && enemy->unitType == UnitType::ENEMY_SNIPER)
                {
                    enemy->isCanBeSpawned = true;
                    ++sniperCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(rocketCount < 10 && enemy->unitType == UnitType::ENEMY_ROCKET)
                {
                    enemy->isCanBeSpawned = true;
                    ++rocketCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("Prepare wave 10. Max enemies: %d", EnumsAndVars::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave11 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave11Time)
        {
            m_prepareWave11 = false;

            EnumsAndVars::enemiesMaxActiveCountOnGround = 0;

            int ratCount = 0;
            int gunCount = 0;
            int gunShieldCount = 0;
            int sniperCount = 0;
            int rocketCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(ratCount < 150 && enemy->unitType == UnitType::ENEMY_MAGNET)
                {
                    enemy->isCanBeSpawned = true;
                    ++ratCount;
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
                else if(sniperCount < 2 && enemy->unitType == UnitType::ENEMY_SNIPER)
                {
                    enemy->isCanBeSpawned = true;
                    ++sniperCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(rocketCount < 15 && enemy->unitType == UnitType::ENEMY_ROCKET)
                {
                    enemy->isCanBeSpawned = true;
                    ++rocketCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("Prepare wave 11. Max enemies: %d", EnumsAndVars::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave12 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave12Time)
        {
            m_prepareWave12 = false;

            EnumsAndVars::enemiesMaxActiveCountOnGround = 0;

            int ratCount = 0;
            int gunCount = 0;
            int gunShieldCount = 0;
            int sniperCount = 0;
            int rocketCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(ratCount < 150 && enemy->unitType == UnitType::ENEMY_MAGNET)
                {
                    enemy->isCanBeSpawned = true;
                    ++ratCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(gunCount < 100 && enemy->unitType == UnitType::ENEMY_GUN)
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
                else if(sniperCount < 2 && enemy->unitType == UnitType::ENEMY_SNIPER)
                {
                    enemy->isCanBeSpawned = true;
                    ++sniperCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(rocketCount < 20 && enemy->unitType == UnitType::ENEMY_ROCKET)
                {
                    enemy->isCanBeSpawned = true;
                    ++rocketCount;
                    ++EnumsAndVars::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("Prepare wave 12. Max enemies: %d", EnumsAndVars::enemiesMaxActiveCountOnGround);
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

                if(enemy->unitType != UnitType::ENEMY_SNIPER)
                {
                    enemy->enableEnemy();
                    enemy->disableDraw();

                    const glm::ivec2 spawnPoint2D = m_pointsToSpawnEnemies[Beryll::RandomGenerator::getInt(m_pointsToSpawnEnemies.size() - 1)];

                    enemy->setPathArray(m_pathFinderEnemies.findPath(spawnPoint2D, m_playerClosestAllowedPos, 6), 1);

                    enemy->setOrigin(enemy->getStartPointMoveFrom());
                }
                // Spawn sniper.
                else if(StaticEnemy::spawnTime + StaticEnemy::spawnDelay < EnumsAndVars::mapPlayTimeSec)
                {
                    for(StaticPosition& sniperPos : StaticEnemy::staticPositions)
                    {
                        // Check position in free + not closest than 150m.
                        if(sniperPos.isFreePosition &&
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
    }

    void Map2::startBossPhase()
    {
        //EnumsAndVars::bossPhase = true;
        //EnumsAndVars::enemiesLastWavePhase = false;
    }

    void Map2::handlePossPhase()
    {

        //EnumsAndVars::mapPlayerWin = true;
    }

    void Map2::spawnCommonGarbage()
    {
        if(!m_pointsToSpawnCommonGarbage.empty() &&
           EnumsAndVars::garbageCommonSpawnTime + EnumsAndVars::garbageCommonSpawnDelay < EnumsAndVars::mapPlayTimeSec)
        {
            EnumsAndVars::garbageCommonSpawnTime = EnumsAndVars::mapPlayTimeSec;

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

            spawnGarbage(EnumsAndVars::garbageCommonSpawnCount, GarbageType::COMMON, spawnPoint3D);
            //BR_INFO("Garbage::getCommonActiveCount() %d", Garbage::getCommonActiveCount());
        }
    }
}