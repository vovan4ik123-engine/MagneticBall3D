#include "Map0Tutorial.h"
#include "enemies/StaticEnemy.h"
#include "Sounds.h"

namespace MagneticBall3D
{
    Map0Tutorial::Map0Tutorial(std::shared_ptr<PlayStateGUILayer> gui) : BaseMap(std::move(gui))
    {
        loadPlayer();
        m_player->getObj()->setOrigin(glm::vec3(95.0f, 2.0f,0.0f));
        m_improvements.setPlayer(m_player);
        loadEnv();
        loadGarbage();
        BR_ASSERT((m_allGarbage.size() < m_maxGarbageCount), "%s", "m_allGarbage reallocation happened. Increase maxGarbageCount.");
        loadEnemies();

        loadShaders();
        handleCamera();

        m_dirToSun = glm::normalize(glm::vec3(-0.5f, 2.0f, 0.4f));
        m_sunLightDir = -m_dirToSun;

        m_skyBox = Beryll::Renderer::createSkyBox("skyboxes/whiteClouds");

        if(EnumsAndVars::playerMagneticRadius < 60.0f)
            EnumsAndVars::playerMagneticRadius = 60.0f;
        if(EnumsAndVars::garbageMaxCountMagnetized < 85.0f)
            EnumsAndVars::garbageMaxCountMagnetized = 85.0f;
        if(EnumsAndVars::playerImpulseFactorOnGround < 1.7f)
            EnumsAndVars::playerImpulseFactorOnGround = 1.7f;
        if(EnumsAndVars::playerTorqueFactorOnGround < 1.6f)
            EnumsAndVars::playerTorqueFactorOnGround = 1.6f;
        if(EnumsAndVars::playerImpulseFactorOnBuildingRoof < 1.5f)
            EnumsAndVars::playerImpulseFactorOnBuildingRoof = 1.5f;
        if(EnumsAndVars::playerTorqueFactorOnBuildingRoof < 1.4f)
            EnumsAndVars::playerTorqueFactorOnBuildingRoof = 1.4f;
        if(EnumsAndVars::playerTorqueFactorOnBuildingWall < 12.0f)
            EnumsAndVars::playerTorqueFactorOnBuildingWall = 12.0f;

        m_gui->disableMapPlayTimer();

        SendStatisticsHelper::sendMapStart();
    }

    Map0Tutorial::~Map0Tutorial()
    {
        
    }

    void Map0Tutorial::updateBeforePhysics()
    {
        if(EnumsAndVars::gameOnPause)
        {
            m_gui->tutorialMoveShow = false;
            return;
        }

        EnumsAndVars::mapPlayTimeSec += Beryll::TimeStep::getTimeStepSec();

        Sounds::update();

        handlePlayerWin();

        handleControls();

        if(m_player->getObj()->getOrigin().x < 85.0f)
        {
            m_player->getObj()->resetVelocities();
            m_player->getObj()->applyCentralImpulse(glm::vec3(1.0f, 0.0f, 0.0f) * 300.0f);
        }
        else if(m_player->getObj()->getOrigin().z > 58.0f)
        {
            m_player->getObj()->resetVelocities();
            m_player->getObj()->applyCentralImpulse(glm::vec3(1.0f, 1.0f, -0.8f) * 150.0f);
        }
        else if(m_player->getObj()->getOrigin().z < -58.0f)
        {
            m_player->getObj()->resetVelocities();
            m_player->getObj()->applyCentralImpulse(glm::vec3(1.0f, 1.0f, 0.8f) * 150.0f);
        }
    }

    void Map0Tutorial::updateAfterPhysics()
    {
        if(EnumsAndVars::gameOnPause)
            return;

        for(const std::shared_ptr<Beryll::SceneObject>& so : m_animatedOrDynamicObjects)
        {
            if(so->getIsEnabledUpdate())
            {
                so->updateAfterPhysics();
            }
        }

        m_player->update();
        updateEnemiesAndTheirsAttacks();
        updateAndMagnetizeGarbage();
        killEnemies();
        handleCamera();
        updateGUI();

        // Tutorial progress.
        if(m_player->getObj()->getOrigin().x > 1300.0f)
            EnumsAndVars::mapPlayerWin = true;
        else if(m_player->getObj()->getOrigin().x > 1200.0f)
            SendStatisticsHelper::sendMap0_1200mPassed();
        else if(m_player->getObj()->getOrigin().x > 1050.0f)
            SendStatisticsHelper::sendMap0_onBuilding();
        else if(m_player->getObj()->getOrigin().x > 800.0f)
            SendStatisticsHelper::sendMap0_800mPassed();
        else if(m_player->getObj()->getOrigin().x > 600.0f)
            SendStatisticsHelper::sendMap0_600mPassed();
        else if(m_player->getObj()->getOrigin().x > 400.0f)
            SendStatisticsHelper::sendMap0_400mPassed();
        else if(m_player->getObj()->getOrigin().x > 200.0f)
            SendStatisticsHelper::sendMap0_200mPassed();
        else if(m_player->getObj()->getOrigin().x > 100.0f)
            SendStatisticsHelper::sendMap0_100mPassed();

        // Tutorial tips on screen.
        if(m_player->getMoveSpeed() < 2.0f)
            m_gui->tutorialMoveShow = true;
        else
            m_gui->tutorialMoveShow = false;
    }

    void Map0Tutorial::draw()
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
            m_ambientLight = 0.15f;
        else
            m_ambientLight = 0.7f;

        m_animatedObjSunLightShadows->bind();
        m_animatedObjSunLightShadows->set3Float("sunLightDir", m_sunLightDir);
        m_animatedObjSunLightShadows->set3Float("cameraPos", Beryll::Camera::getCameraPos());
        m_animatedObjSunLightShadows->set1Float("ambientLight", m_ambientLight);
        m_animatedObjSunLightShadows->set1Float("specularLightStrength", 0.8f);

        for(const auto& animObj : m_allAnimatedEnemies)
        {
            if(animObj->getIsEnabledDraw())
            {
                modelMatrix = animObj->getModelMatrix();
                m_animatedObjSunLightShadows->setMatrix4x4Float("MVPLightMatrix", m_sunLightVPMatrix * modelMatrix);
                m_animatedObjSunLightShadows->setMatrix4x4Float("modelMatrix", modelMatrix);
                m_animatedObjSunLightShadows->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
                Beryll::Renderer::drawObject(animObj, modelMatrix, m_animatedObjSunLightShadows);
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
        m_simpleObjSunLightShadowsNormals->set1Float("ambientLight", m_ambientLight);
        m_simpleObjSunLightShadowsNormals->set1Float("specularLightStrength", 0.5f);

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

    void Map0Tutorial::loadEnv()
    {
        const auto ground = std::make_shared<Beryll::SimpleCollidingObject>("models3D/map0Tutorial/Ground.fbx",
                                                                            0.0f,
                                                                            false,
                                                                            Beryll::CollisionFlags::STATIC,
                                                                            Beryll::CollisionGroups::GROUND,
                                                                            Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE | Beryll::CollisionGroups::CAMERA,
                                                                            Beryll::SceneObjectGroups::GROUND);

        m_objWithNormalMap.push_back(ground);
        ground->setFriction(EnumsAndVars::staticEnvFriction);

        const auto walls = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map0Tutorial/Buildings.fbx",
                                                                                       0.0f,
                                                                                       false,
                                                                                       Beryll::CollisionFlags::STATIC,
                                                                                       Beryll::CollisionGroups::BUILDING,
                                                                                       Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE |
                                                                                       Beryll::CollisionGroups::RAY_FOR_ENVIRONMENT | Beryll::CollisionGroups::CAMERA,
                                                                                       Beryll::SceneObjectGroups::BUILDING);

        for(const auto& obj : walls)
        {
            m_objWithNormalMap.push_back(obj);
            m_simpleObjForShadowMap.push_back(obj);
            obj->setFriction(EnumsAndVars::staticEnvFriction);
        }

        const auto envNoColliders1 = Beryll::SimpleObject::loadManyModelsFromOneFile("models3D/map0Tutorial/EnvNoColliders.fbx", Beryll::SceneObjectGroups::BUILDING);

        for(const auto& obj : envNoColliders1)
        {
            m_staticEnv.push_back(obj);
            m_simpleObjForShadowMap.push_back(obj);
        }
    }

    void Map0Tutorial::loadGarbage()
    {
        for(int i = 0; i < 2; ++i) // 2 * 32 = 64
        {
            const auto garbageCommon = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map0Tutorial/GarbageCommon_32items.fbx",
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
                m_allGarbage.back().enableGarbage();

                m_animatedOrDynamicObjects.push_back(obj);
                m_simpleObjForShadowMap.push_back(obj);

                obj->setDamping(EnumsAndVars::garbageLinearDamping, EnumsAndVars::garbageAngularDamping);
                obj->setGravity(EnumsAndVars::garbageGravityDefault, false, false);
                obj->setOrigin(glm::vec3(Beryll::RandomGenerator::getInt(300) + 200,
                                            Beryll::RandomGenerator::getInt(40) + 10,
                                            Beryll::RandomGenerator::getInt(100) - 50));
            }
        }

        for(int i = 0; i < 8; ++i) // 8 * 4 = 32
        {
            const auto garbageEnemy = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map0Tutorial/GarbageEnemyJanitorRake_4items.fbx",
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

        for(int i = 0; i < 7; ++i) // 7 * 4 = 28
        {
            const auto garbageEnemy = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map0Tutorial/GarbageEnemyGunShield_4items.fbx",
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

        BR_INFO("Garbage::getCommonActiveCount() %d", Garbage::getCommonActiveCount());
    }

    void Map0Tutorial::loadEnemies()
    {
        for(int i = 0; i < 70; ++i)
        {
            auto janitorRake = std::make_shared<StaticEnemy>("models3D/enemies/JanitorRake.fbx",
                                                             0.0f,
                                                             false,
                                                             Beryll::CollisionFlags::STATIC,
                                                             Beryll::CollisionGroups::NONE,
                                                             Beryll::CollisionGroups::NONE,
                                                             Beryll::SceneObjectGroups::ENEMY);

            janitorRake->setCurrentAnimationByIndex(EnumsAndVars::AnimationIndexes::stand, false, false, true);
            janitorRake->setDefaultAnimationByIndex(EnumsAndVars::AnimationIndexes::stand);
            janitorRake->unitType = UnitType::ENEMY_MELEE;
            janitorRake->attackType = AttackType::MELEE_DAMAGE_ONE;
            janitorRake->attackSound = SoundType::NONE;
            janitorRake->attackHitSound = SoundType::STICK_HIT;
            janitorRake->dieSound = SoundType::POP;
            janitorRake->dieGarbageType = GarbageType::ENEMY_GARBAGE1;

            janitorRake->damage = 0.0f;
            janitorRake->attackDistance = 50.0f;
            janitorRake->timeBetweenAttacks = 1.5f + Beryll::RandomGenerator::getFloat() * 0.2f;

            janitorRake->garbageAmountToDie = 10;
            janitorRake->reducePlayerSpeedWhenDie = 0.0f;
            janitorRake->experienceWhenDie = 0;

            m_animatedOrDynamicObjects.push_back(janitorRake);
            m_allAnimatedEnemies.push_back(janitorRake);
            m_animatedObjForShadowMap.push_back(janitorRake);

            janitorRake->setOrigin(glm::vec3(Beryll::RandomGenerator::getInt(200) + 550,
                                             janitorRake->getFromOriginToBottom(),
                                             Beryll::RandomGenerator::getInt(100) - 50));

            janitorRake->enableEnemy();
            janitorRake->rotateToDirection(glm::vec3(-1.0f, 0.0f, 0.0f), true);
        }

        for(int i = 0; i < 30; ++i)
        {
            auto copShield = std::make_shared<StaticEnemy>("models3D/enemies/CopWithPistolShield.fbx",
                                                            0.0f,
                                                            false,
                                                            Beryll::CollisionFlags::STATIC,
                                                            Beryll::CollisionGroups::NONE,
                                                            Beryll::CollisionGroups::NONE,
                                                            Beryll::SceneObjectGroups::ENEMY);

            copShield->setCurrentAnimationByIndex(EnumsAndVars::AnimationIndexes::stand, false, false, true);
            copShield->setDefaultAnimationByIndex(EnumsAndVars::AnimationIndexes::stand);
            copShield->unitType = UnitType::ENEMY_GUN_SHIELD;
            copShield->attackType = AttackType::RANGE_DAMAGE_ONE;
            copShield->attackSound = SoundType::PISTOL_SHOT;
            copShield->attackHitSound = SoundType::PISTOL_HIT;
            copShield->attackParticlesColor = glm::vec3{0.9f, 0.9f, 0.0f};
            copShield->attackParticlesSize = 0.3f;
            copShield->dieSound = SoundType::POP;
            copShield->dieGarbageType = GarbageType::ENEMY_GARBAGE2;

            copShield->damage = 0.0f;
            copShield->attackDistance = 100.0f;
            copShield->timeBetweenAttacks = 2.0f + Beryll::RandomGenerator::getFloat() * 0.2f;

            copShield->garbageAmountToDie = 10;
            copShield->reducePlayerSpeedWhenDie = 0.0f;
            copShield->experienceWhenDie = 0;

            m_animatedOrDynamicObjects.push_back(copShield);
            m_allAnimatedEnemies.push_back(copShield);
            m_animatedObjForShadowMap.push_back(copShield);

            copShield->setOrigin(glm::vec3(Beryll::RandomGenerator::getInt(150) + 750,
                                           copShield->getFromOriginToBottom(),
                                           Beryll::RandomGenerator::getInt(100) - 50));

            copShield->enableEnemy();
            copShield->rotateToDirection(glm::vec3(-1.0f, 0.0f, 0.0f), true);
        }
    }
}
