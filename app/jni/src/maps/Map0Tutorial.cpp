#include "Map0Tutorial.h"
#include "enemies/StaticEnemy.h"
#include "Sounds.h"

namespace MagneticBall3D
{
    Map0Tutorial::Map0Tutorial(std::shared_ptr<PlayStateGUILayer> gui) : BaseMap(std::move(gui))
    {
        Beryll::LoadingScreen::showProgress(10.0f);

        // Allocate enough spase for all vectors to avoid vector reallocation.
        const int maxGarbageCount = 200;
        m_allGarbage.reserve(maxGarbageCount);
        m_allAnimatedEnemies.reserve(100);
        m_animatedOrDynamicObjects.reserve(100 + maxGarbageCount);
        m_staticEnv.reserve(100);
        m_simpleObjForShadowMap.reserve(100 + maxGarbageCount);
        m_animatedObjForShadowMap.reserve(100);

        // Specific for this map only.
        loadPlayer();
        m_player->getObj()->setOrigin(glm::vec3(0.0f, 2.0f,0.0f));
        Beryll::LoadingScreen::showProgress(20.0f);
        loadEnv();
        Beryll::LoadingScreen::showProgress(40.0f);
        loadGarbage();
        BR_ASSERT((m_allGarbage.size() < maxGarbageCount), "%s", "m_allGarbage reallocation happened. Increase maxGarbageCount.");
        Beryll::LoadingScreen::showProgress(60.0f);
        loadEnemies();
        Beryll::LoadingScreen::showProgress(80.0f);

        loadShaders();
        handleCamera();

        m_dirToSun = glm::normalize(glm::vec3(-0.5f, 3.5f, -1.0f));
        m_sunLightDir = -m_dirToSun;

        m_improvements = Improvements(m_player, {});
        m_skyBox = Beryll::Renderer::createSkyBox("skyboxes/map1");

        EnAndVars::playerMagneticRadius = 50.0f;
        EnAndVars::garbageMaxCountMagnetized = 85.0f;

        m_gui->disableMapPlayTimer();

        SendStatisticsHelper::sendMapStart();

        Beryll::LoadingScreen::showProgress(100.0f);

        Sounds::startBackgroundMusic(SoundType::BACKGROUND_MUSIC_1);
    }

    Map0Tutorial::~Map0Tutorial()
    {
        Sounds::stopBackgroundMusic();
    }

    void Map0Tutorial::updateBeforePhysics()
    {
        if(EnAndVars::gameOnPause)
        {
            m_gui->tutorialSwipeEnabled = false;
            m_gui->tutorialHowToSwipeEnabled = false;
            m_gui->tutorialSwipeOnWallEnabled = false;
            return;
        }

        EnAndVars::mapPlayTimeSec += Beryll::TimeStep::getTimeStepSec();

        Sounds::update();

        handlePlayerWin();

        handleScreenSwipe();

        if(m_player->getObj()->getOrigin().x < -28.0f)
        {
            m_player->getObj()->resetVelocities();
            m_player->getObj()->applyCentralImpulse(glm::vec3(1.0f, 0.0f, 0.0f) * 100.0f);
        }
        else if(m_player->getObj()->getOrigin().z > 58.0f)
        {
            m_player->getObj()->resetVelocities();
            m_player->getObj()->applyCentralImpulse(glm::vec3(0.55f, 1.0f, -1.0f) * 70.0f);
        }
        else if(m_player->getObj()->getOrigin().z < -58.0f)
        {
            m_player->getObj()->resetVelocities();
            m_player->getObj()->applyCentralImpulse(glm::vec3(0.55f, 1.0f, 1.0f) * 70.0f);
        }
    }

    void Map0Tutorial::updateAfterPhysics()
    {
        if(EnAndVars::gameOnPause)
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
            EnAndVars::mapPlayerWin = true;
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

        m_gui->tutorialSwipeEnabled = false;
        m_gui->tutorialHowToSwipeEnabled = false;
        m_gui->tutorialSwipeOnWallEnabled = false;
        // Tutorial tips on screen.
        if(m_player->getMoveSpeed() < 7.0f)
            m_gui->tutorialSwipeEnabled = true;
        else
            m_gui->tutorialSwipeEnabled = false;

        if(m_player->getObj()->getOrigin().x > 800.0f && m_player->getObj()->getOrigin().x < 1030.0f)
        {
            m_gui->tutorialHowToSwipeEnabled = false;
            m_gui->tutorialSwipeOnWallEnabled = true;
        }
        else if(m_player->getMoveSpeed() > 7.0f && m_player->getMoveSpeed() < 60.0f)
        {
            m_gui->tutorialHowToSwipeEnabled = true;
            m_gui->tutorialSwipeOnWallEnabled = false;
        }
        else
        {
            m_gui->tutorialHowToSwipeEnabled = false;
            m_gui->tutorialSwipeOnWallEnabled = false;
        }
    }

    void Map0Tutorial::draw()
    {
        //BR_INFO("%s", "scene draw call");
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

        if(EnAndVars::gameOnPause || EnAndVars::improvementSystemOnScreen)
            m_ambientLight = 0.25f;
        else
            m_ambientLight = 0.7f;

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

        m_simpleObjSunLightShadows->set1Float("specularLightStrength", 0.8f);

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
        m_simpleObjSunLightShadowsNormals->set1Float("specularLightStrength", 0.7f);

        for(const auto& normalMapObj : m_objWithNormalMap)
        {
            modelMatrix = normalMapObj->getModelMatrix();
            m_simpleObjSunLightShadowsNormals->setMatrix4x4Float("MVPLightMatrix", m_sunLightVPMatrix * modelMatrix);
            m_simpleObjSunLightShadowsNormals->setMatrix4x4Float("modelMatrix", modelMatrix);
            m_simpleObjSunLightShadowsNormals->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
            Beryll::Renderer::drawObject(normalMapObj, modelMatrix, m_simpleObjSunLightShadowsNormals);
        }

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
                                                                            Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE,
                                                                            Beryll::SceneObjectGroups::GROUND);

        m_objWithNormalMap.push_back(ground);
        ground->setFriction(EnAndVars::staticEnvFriction);

        const auto walls = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map0Tutorial/Walls.fbx",
                                                                                       0.0f,
                                                                                       false,
                                                                                       Beryll::CollisionFlags::STATIC,
                                                                                       Beryll::CollisionGroups::BUILDING,
                                                                                       Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE |
                                                                                       Beryll::CollisionGroups::CAMERA,
                                                                                       Beryll::SceneObjectGroups::BUILDING);

        for(const auto& obj : walls)
        {
            m_objWithNormalMap.push_back(obj);
            m_simpleObjForShadowMap.push_back(obj);
            obj->setFriction(EnAndVars::staticEnvFriction);
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
            const auto garbageCommon = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map0Tutorial/GarbageCommon.fbx",
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
                m_allGarbage.emplace_back(obj, GarbageType::COMMON, 40);
                m_allGarbage.back().enableGarbage();

                m_animatedOrDynamicObjects.push_back(obj);
                m_simpleObjForShadowMap.push_back(obj);

                obj->setDamping(EnAndVars::garbageLinearDamping, EnAndVars::garbageAngularDamping);
                obj->setGravity(EnAndVars::garbageGravityDefault, false, false);
                obj->setOrigin(glm::vec3(Beryll::RandomGenerator::getInt(300) + 200,
                                            Beryll::RandomGenerator::getInt(40) + 10,
                                            Beryll::RandomGenerator::getInt(100) - 50));
            }
        }

        for(int i = 0; i < 15; ++i) // 15 * 4 = 60
        {
            const auto garbageCopPistol = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map0Tutorial/GarbageCopPistol_4items.fbx",
                                                                                                   EnAndVars::garbageMass,
                                                                                                   false,
                                                                                                   Beryll::CollisionFlags::DYNAMIC,
                                                                                                   Beryll::CollisionGroups::GARBAGE,
                                                                                                   Beryll::CollisionGroups::GROUND | Beryll::CollisionGroups::BUILDING |
                                                                                                   Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE |
                                                                                                   Beryll::CollisionGroups::ENEMY_ATTACK,
                                                                                                   Beryll::SceneObjectGroups::GARBAGE);

            for(const auto& obj : garbageCopPistol)
            {
                m_allGarbage.emplace_back(obj, GarbageType::COP_WITH_PISTOL, 40);

                m_animatedOrDynamicObjects.push_back(obj);
                m_simpleObjForShadowMap.push_back(obj);

                obj->setDamping(EnAndVars::garbageLinearDamping, EnAndVars::garbageAngularDamping);
                obj->setGravity(EnAndVars::garbageGravityDefault, false, false);
            }
        }

        BR_INFO("Garbage::getCommonActiveCount() %d", Garbage::getCommonActiveCount());
    }

    void Map0Tutorial::loadEnemies()
    {
        for(int i = 0; i < 100; ++i)
        {
            auto cop = std::make_shared<StaticEnemy>("models3D/enemies/CopWithPistol.fbx",
                                                        0.0f,
                                                        false,
                                                        Beryll::CollisionFlags::STATIC,
                                                        Beryll::CollisionGroups::NONE,
                                                        Beryll::CollisionGroups::NONE,
                                                        Beryll::SceneObjectGroups::ENEMY);

            cop->setCurrentAnimationByIndex(EnAndVars::AnimationIndexes::stand, false, false);
            cop->setDefaultAnimationByIndex(EnAndVars::AnimationIndexes::stand);
            cop->unitType = UnitType::COP_WITH_PISTOL;
            cop->attackType = AttackType::RANGE_DAMAGE_ONE;

            cop->damage = 0.0f;
            cop->attackDistance = 100.0f;
            cop->timeBetweenAttacks = 2.0f + Beryll::RandomGenerator::getFloat() * 2.0f;

            cop->garbageAmountToDie = 10;
            cop->reducePlayerSpeedWhenDie = 2.0f;
            cop->experienceWhenDie = 0;

            m_animatedOrDynamicObjects.push_back(cop);
            m_allAnimatedEnemies.push_back(cop);
            m_animatedObjForShadowMap.push_back(cop);

            cop->setOrigin(glm::vec3(Beryll::RandomGenerator::getInt(250) + 600,
                                     cop->getFromOriginToBottom(),
                                     Beryll::RandomGenerator::getInt(100) - 50));

            cop->enableEnemy();
            cop->rotateToDirection(glm::vec3(-1.0f, 0.0f, 0.0f), true);
        }
    }
}
