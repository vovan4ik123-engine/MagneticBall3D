#include "Map4.h"
#include "EnumsAndVariables.h"
#include "enemies/MovableEnemy.h"
#include "Sounds.h"

namespace MagneticBall3D
{
    Map4::Map4(std::shared_ptr<PlayStateGUILayer> gui) : BaseMap(gui)
    {
        loadPlayer();
        m_player->getObj()->setOrigin(glm::vec3(-470.0f, 2.0f,-40.0f));
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
        std::vector<glm::vec3> walls = BeryllUtils::Common::loadMeshVerticesToVector("models3D/map4/PathEnemiesWalls.fbx");
        for(const auto& wall : walls)
        {
            m_pathFinderEnemies.addWallPosition({(int)std::roundf(wall.x), (int)std::roundf(wall.z)});
        }

        BR_INFO("Map4 pathfinder walls: %d", walls.size());

        std::vector<glm::vec3> allowedPoints = BeryllUtils::Common::loadMeshVerticesToVector("models3D/map4/PathEnemiesAllowedPositions.fbx");
        m_pathAllowedPositionsXZ.reserve(allowedPoints.size());
        for(const auto& point : allowedPoints)
        {
            m_pathAllowedPositionsXZ.push_back({(int)std::roundf(point.x), (int)std::roundf(point.z)});
        }

        BR_INFO("Map4 pathfinder allowed points: %d", m_pathAllowedPositionsXZ.size());
        m_pointsToSpawnEnemies.reserve(m_pathAllowedPositionsXZ.size());
        m_pointsToSpawnCommonGarbage.reserve(m_pathAllowedPositionsXZ.size());

        m_dirToSun = glm::normalize(glm::vec3(-1.0f, 2.5f, 0.5f));
        m_sunLightDir = -m_dirToSun;

        m_skyBox = Beryll::Renderer::createSkyBox("skyboxes/whiteClouds");

        if(EnumsAndVars::playerMagneticRadius < 40.0f)
            EnumsAndVars::playerMagneticRadius = 40.0f;
        if(EnumsAndVars::garbageMaxCountMagnetized < 70.0f)
            EnumsAndVars::garbageMaxCountMagnetized = 70.0f;
        if(EnumsAndVars::playerImpulseFactorOnGround < 1.6f)
            EnumsAndVars::playerImpulseFactorOnGround = 1.6f;
        if(EnumsAndVars::playerTorqueFactorOnGround < 1.5f)
            EnumsAndVars::playerTorqueFactorOnGround = 1.5f;
        if(EnumsAndVars::playerImpulseFactorOnBuildingRoof < 1.4f)
            EnumsAndVars::playerImpulseFactorOnBuildingRoof = 1.4f;
        if(EnumsAndVars::playerTorqueFactorOnBuildingRoof < 1.3f)
            EnumsAndVars::playerTorqueFactorOnBuildingRoof = 1.3f;
        if(EnumsAndVars::playerTorqueFactorOnBuildingWall < 11.0f)
            EnumsAndVars::playerTorqueFactorOnBuildingWall = 11.0f;

        SendStatisticsHelper::sendMapStart();

        Sounds::startBackgroundMusic(SoundType::BACKGROUND_MUSIC_2);

        //BR_INFO(" %f", );
        //BR_INFO("%s", "");
    }

    Map4::~Map4()
    {
        Sounds::stopBackgroundMusic();
    }

    void Map4::draw()
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

    void Map4::loadEnv()
    {
        const auto mainGround = std::make_shared<Beryll::SimpleCollidingObject>("models3D/map4/Ground.fbx",
                                                                                0.0f,
                                                                                false,
                                                                                Beryll::CollisionFlags::STATIC,
                                                                                Beryll::CollisionGroups::GROUND,
                                                                                Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE | Beryll::CollisionGroups::CAMERA,
                                                                                Beryll::SceneObjectGroups::GROUND);

        m_objWithNormalMap.push_back(mainGround);
        mainGround->setFriction(EnumsAndVars::staticEnvFriction);

        const auto buildings1 = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map4/Buildings.fbx",
                                                                                        0.0f,
                                                                                        false,
                                                                                        Beryll::CollisionFlags::STATIC,
                                                                                        Beryll::CollisionGroups::BUILDING,
                                                                                        Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE |
                                                                                        Beryll::CollisionGroups::RAY_FOR_ENVIRONMENT | Beryll::CollisionGroups::CAMERA,
                                                                                        Beryll::SceneObjectGroups::BUILDING);

        for(const auto& obj : buildings1)
        {
            m_staticEnv.push_back(obj);
            m_simpleObjForShadowMap.push_back(obj);
            obj->setFriction(EnumsAndVars::staticEnvFriction);
        }

        const auto buildings2 = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map4/BuildingsNormalMap.fbx",
                                                                                         0.0f,
                                                                                         false,
                                                                                         Beryll::CollisionFlags::STATIC,
                                                                                         Beryll::CollisionGroups::BUILDING,
                                                                                         Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE |
                                                                                         Beryll::CollisionGroups::RAY_FOR_ENVIRONMENT | Beryll::CollisionGroups::CAMERA,
                                                                                         Beryll::SceneObjectGroups::BUILDING);

        for(const auto& obj : buildings2)
        {
            m_objWithNormalMap.push_back(obj);
            m_simpleObjForShadowMap.push_back(obj);
            obj->setFriction(EnumsAndVars::staticEnvFriction);
        }

        const auto jumpPads = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map4/JumpPads.fbx",
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

        const auto envNoColliders = Beryll::SimpleObject::loadManyModelsFromOneFile("models3D/map4/EnvNoColliders.fbx", Beryll::SceneObjectGroups::BUILDING);

        for(const auto& obj : envNoColliders)
        {
            m_staticEnv.push_back(obj);
            m_simpleObjForShadowMap.push_back(obj);
        }
    }

    void Map4::loadGarbage()
    {
        for(int i = 0; i < 6; ++i) // 6 * 32 = 192
        {
            const auto garbageCommon = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map4/GarbageCommon_32items.fbx",
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

        for(int i = 0; i < 4; ++i) // 4 * 3 = 12
        {
            const auto garbageEnemy = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map4/GarbageEnemyMummy_3items.fbx",
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

        for(int i = 0; i < 3; ++i) // 3 * 2 = 6
        {
            const auto garbageEnemy = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map4/GarbageEnemyCamel_2items.fbx",
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

        for(int i = 0; i < 2; ++i) // 2 * 5 = 10
        {
            const auto garbageEnemy = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map4/GarbageEnemyChariot_5items.fbx",
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

    void Map4::loadEnemies()
    {
        for(int i = 0; i < 160; ++i)
        {
            auto mummy = std::make_shared<MovableEnemy>("models3D/enemies/Mummy.fbx",
                                                            0.0f,
                                                            false,
                                                            Beryll::CollisionFlags::STATIC,
                                                            Beryll::CollisionGroups::NONE,
                                                            Beryll::CollisionGroups::NONE,
                                                            Beryll::SceneObjectGroups::ENEMY);

            mummy->setCurrentAnimationByIndex(EnumsAndVars::AnimationIndexes::run, false, false, true);
            mummy->setDefaultAnimationByIndex(EnumsAndVars::AnimationIndexes::stand);
            mummy->unitType = UnitType::ENEMY_GUN1;
            mummy->attackType = AttackType::RANGE_DAMAGE_ONE;
            mummy->attackSound = SoundType::NONE;
            mummy->attackHitSound = SoundType::STONE_HIT;
            mummy->attackParticlesColor = glm::vec3{0.347f, 0.379f, 0.5586f};
            mummy->attackParticlesSize = 0.2f;
            mummy->dieSound = SoundType::POP;
            mummy->dieGarbageType = GarbageType::ENEMY_GARBAGE1;

            mummy->damage = 1.0f;
            mummy->attackDistance = 50.0f + Beryll::RandomGenerator::getFloat() * 150.0f;
            mummy->timeBetweenAttacks = 2.5f + Beryll::RandomGenerator::getFloat() * 0.5f;

            mummy->garbageAmountToDie = 10;
            mummy->reducePlayerSpeedWhenDie = 0.5f;
            mummy->experienceWhenDie = 25;
            mummy->getController().moveSpeed = 25.0f;

            m_animatedOrDynamicObjects.push_back(mummy);
            m_allAnimatedEnemies.push_back(mummy);
            m_animatedObjForShadowMap.push_back(mummy);
        }

        for(int i = 0; i < 90; ++i)
        {
            auto camel = std::make_shared<MovableEnemy>("models3D/enemies/Camel.fbx",
                                                        0.0f,
                                                        false,
                                                        Beryll::CollisionFlags::STATIC,
                                                        Beryll::CollisionGroups::NONE,
                                                        Beryll::CollisionGroups::NONE,
                                                        Beryll::SceneObjectGroups::ENEMY);

            camel->setCurrentAnimationByIndex(EnumsAndVars::AnimationIndexes::run, false, false, true);
            camel->setDefaultAnimationByIndex(EnumsAndVars::AnimationIndexes::stand);
            camel->unitType = UnitType::ENEMY_TANK;
            camel->attackType = AttackType::RANGE_DAMAGE_RADIUS;
            camel->attackSound = SoundType::NONE;
            camel->attackHitSound = SoundType::SPIT;
            camel->attackParticlesColor = glm::vec3{0.59f, 0.828f, 0.555f};
            camel->attackParticlesSize = 0.6f;
            camel->dieSound = SoundType::POP;
            camel->dieGarbageType = GarbageType::ENEMY_GARBAGE2;

            camel->damage = 0.5f;
            camel->damageRadius = 10.0f;
            camel->attackDistance = 150.0f + Beryll::RandomGenerator::getFloat() * 250.0f;
            camel->timeBetweenAttacks = 3.0f + Beryll::RandomGenerator::getFloat() * 0.5f;

            camel->garbageAmountToDie = 20;
            camel->reducePlayerSpeedWhenDie = 0.5f;
            camel->experienceWhenDie = 35;
            camel->getController().moveSpeed = 35.0f;

            m_animatedOrDynamicObjects.push_back(camel);
            m_allAnimatedEnemies.push_back(camel);
            m_animatedObjForShadowMap.push_back(camel);
        }

        for(int i = 0; i < 60; ++i)
        {
            auto chariotJavelin = std::make_shared<MovableEnemy>("models3D/enemies/ChariotJavelin.fbx",
                                                         0.0f,
                                                         false,
                                                         Beryll::CollisionFlags::STATIC,
                                                         Beryll::CollisionGroups::NONE,
                                                         Beryll::CollisionGroups::NONE,
                                                         Beryll::SceneObjectGroups::ENEMY);

            chariotJavelin->setCurrentAnimationByIndex(EnumsAndVars::AnimationIndexes::run, false, false, true);
            chariotJavelin->setDefaultAnimationByIndex(EnumsAndVars::AnimationIndexes::stand);
            chariotJavelin->unitType = UnitType::ENEMY_GUN2;
            chariotJavelin->attackType = AttackType::RANGE_DAMAGE_ONE;
            chariotJavelin->attackSound = SoundType::NONE;
            chariotJavelin->attackHitSound = SoundType::STICK_HIT;
            chariotJavelin->attackParticlesColor = glm::vec3{0.633f, 0.7967f, 0.789f};
            chariotJavelin->attackParticlesSize = 0.3f;
            chariotJavelin->dieSound = SoundType::POP;
            chariotJavelin->dieGarbageType = GarbageType::ENEMY_GARBAGE3;

            chariotJavelin->damage = 1.0f;
            chariotJavelin->attackDistance = 200.0f + Beryll::RandomGenerator::getFloat() * 200.0f;
            chariotJavelin->timeBetweenAttacks = 2.5f + Beryll::RandomGenerator::getFloat() * 0.5f;

            chariotJavelin->garbageAmountToDie = 20;
            chariotJavelin->reducePlayerSpeedWhenDie = 0.5f;
            chariotJavelin->experienceWhenDie = 50;
            chariotJavelin->getController().moveSpeed = 50.0f;

            m_animatedOrDynamicObjects.push_back(chariotJavelin);
            m_allAnimatedEnemies.push_back(chariotJavelin);
            m_animatedObjForShadowMap.push_back(chariotJavelin);
        }
    }

    void Map4::spawnEnemies()
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
            int tankCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(gun1Count < 50 && enemy->unitType == UnitType::ENEMY_GUN1)
                {
                    enemy->isCanBeSpawned = true;
                    ++gun1Count;
                }
                else if(tankCount < 20 && enemy->unitType == UnitType::ENEMY_TANK)
                {
                    enemy->isCanBeSpawned = true;
                    ++tankCount;
                }
            }

            BR_INFO("Prepare wave 4. Max enemies: %d", gun1Count + tankCount);
        }
        else if(m_prepareWave5 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave5Time)
        {
            m_prepareWave5 = false;

            int gun1Count = 0;
            int tankCount = 0;
            int gun2Count = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(gun1Count < 50 && enemy->unitType == UnitType::ENEMY_GUN1)
                {
                    enemy->isCanBeSpawned = true;
                    ++gun1Count;
                }
                else if(tankCount < 20 && enemy->unitType == UnitType::ENEMY_TANK)
                {
                    enemy->isCanBeSpawned = true;
                    ++tankCount;
                }
                else if(gun2Count < 30 && enemy->unitType == UnitType::ENEMY_GUN2)
                {
                    enemy->isCanBeSpawned = true;
                    ++gun2Count;
                }
            }

            BR_INFO("Prepare wave 5. Max enemies: %d", gun1Count + tankCount + gun2Count);
        }
        else if(m_prepareWave6 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave6Time)
        {
            m_prepareWave6 = false;

            int gun1Count = 0;
            int tankCount = 0;
            int gun2Count = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(gun1Count < 70 && enemy->unitType == UnitType::ENEMY_GUN1)
                {
                    enemy->isCanBeSpawned = true;
                    ++gun1Count;
                }
                else if(tankCount < 40 && enemy->unitType == UnitType::ENEMY_TANK)
                {
                    enemy->isCanBeSpawned = true;
                    ++tankCount;
                }
                else if(gun2Count < 30 && enemy->unitType == UnitType::ENEMY_GUN2)
                {
                    enemy->isCanBeSpawned = true;
                    ++gun2Count;
                }
            }

            BR_INFO("Prepare wave 6. Max enemies: %d", gun1Count + tankCount + gun2Count);
        }
        else if(m_prepareWave7 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave7Time)
        {
            m_prepareWave7 = false;

            int gun1Count = 0;
            int tankCount = 0;
            int gun2Count = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(gun1Count < 100 && enemy->unitType == UnitType::ENEMY_GUN1)
                {
                    enemy->isCanBeSpawned = true;
                    ++gun1Count;
                }
                else if(tankCount < 50 && enemy->unitType == UnitType::ENEMY_TANK)
                {
                    enemy->isCanBeSpawned = true;
                    ++tankCount;
                }
                else if(gun2Count < 40 && enemy->unitType == UnitType::ENEMY_GUN2)
                {
                    enemy->isCanBeSpawned = true;
                    ++gun2Count;
                }
            }

            BR_INFO("Prepare wave 7. Max enemies: %d", gun1Count + tankCount + gun2Count);
        }
        else if(m_prepareWave8 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave8Time)
        {
            m_prepareWave8 = false;

            int gun1Count = 0;
            int tankCount = 0;
            int gun2Count = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(gun1Count < 130 && enemy->unitType == UnitType::ENEMY_GUN1)
                {
                    enemy->isCanBeSpawned = true;
                    ++gun1Count;
                }
                else if(tankCount < 65 && enemy->unitType == UnitType::ENEMY_TANK)
                {
                    enemy->isCanBeSpawned = true;
                    ++tankCount;
                }
                else if(gun2Count < 55 && enemy->unitType == UnitType::ENEMY_GUN2)
                {
                    enemy->isCanBeSpawned = true;
                    ++gun2Count;
                }
            }

            BR_INFO("Prepare wave 8. Max enemies: %d", gun1Count + tankCount + gun2Count);
        }
        else if(m_prepareWave9 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave9Time)
        {
            m_prepareWave9 = false;

            int gun1Count = 0;
            int tankCount = 0;
            int gun2Count = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(gun1Count < 160 && enemy->unitType == UnitType::ENEMY_GUN1)
                {
                    enemy->isCanBeSpawned = true;
                    ++gun1Count;
                }
                else if(tankCount < 90 && enemy->unitType == UnitType::ENEMY_TANK)
                {
                    enemy->isCanBeSpawned = true;
                    ++tankCount;
                }
                else if(gun2Count < 60 && enemy->unitType == UnitType::ENEMY_GUN2)
                {
                    enemy->isCanBeSpawned = true;
                    ++gun2Count;
                }
            }

            BR_INFO("Prepare wave 9. Max enemies: %d", gun1Count + tankCount + gun2Count);
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

        // Spawn enemies.
        if(!m_pointsToSpawnEnemies.empty())
        {
            for(const auto& enemy : m_allAnimatedEnemies)
            {
                if((enemy->getIsEnabled() && glm::distance(m_player->getObj()->getOriginXZ(), enemy->getOriginXZ()) > EnumsAndVars::enemiesDisableDistance) ||
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
}
