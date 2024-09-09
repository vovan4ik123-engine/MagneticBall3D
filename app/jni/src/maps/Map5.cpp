#include "Map5.h"
#include "EnumsAndVariables.h"
#include "enemies/MovableEnemy.h"
#include "Sounds.h"

namespace MagneticBall3D
{
    Map5::Map5(std::shared_ptr<PlayStateGUILayer> gui) : BaseMap(gui)
    {
        loadPlayer();
        m_player->getObj()->setOrigin(glm::vec3(-600.0f, 2.0f,-545.0f));
        m_improvements.setPlayer(m_player);
        loadEnv();
        loadGarbage();
        BR_ASSERT((m_allGarbage.size() < m_maxGarbageCount), "%s", "m_allGarbage reallocation happened. Increase maxGarbageCount.");
        loadEnemies();

        loadShaders();
        //m_cameraAngleOffset = glm::normalize(glm::vec3(-0.1f, 0.0f, -1.0f));
        handleCamera();

        m_minX = -800.0f;
        m_maxX = 800.0f;
        m_minZ = -800.0f;
        m_maxZ = 800.0f;

        m_pathFinderEnemies = AStar(m_minX, m_maxX, m_minZ, m_maxZ, 20);
//        std::vector<glm::vec3> walls = BeryllUtils::Common::loadMeshVerticesToVector("models3D/map4/PathEnemiesWalls.fbx");
//        for(const auto& wall : walls)
//        {
//            m_pathFinderEnemies.addWallPosition({(int)std::roundf(wall.x), (int)std::roundf(wall.z)});
//        }
//
//        BR_INFO("Map4 pathfinder walls: %d", walls.size());

        std::vector<glm::vec3> allowedPoints = BeryllUtils::Common::loadMeshVerticesToVector("models3D/map4/PathEnemiesAllowedPositions.fbx");
        m_pathAllowedPositionsXZ.reserve(allowedPoints.size());
        for(const auto& point : allowedPoints)
        {
            m_pathAllowedPositionsXZ.push_back({(int)std::roundf(point.x), (int)std::roundf(point.z)});
        }

        BR_INFO("Map4 pathfinder allowed points: %d", m_pathAllowedPositionsXZ.size());
        m_pointsToSpawnEnemies.reserve(m_pathAllowedPositionsXZ.size());
        m_pointsToSpawnCommonGarbage.reserve(m_pathAllowedPositionsXZ.size());

        m_dirToSun = glm::normalize(glm::vec3(-1.0f, 1.8f, 0.5f));
        m_sunLightDir = -m_dirToSun;

        m_skyBox = Beryll::Renderer::createSkyBox("skyboxes/nightClouds");

        if(EnumsAndVars::playerMagneticRadius < 35)
            EnumsAndVars::playerMagneticRadius = 35.0f;

        SendStatisticsHelper::sendMapStart();

        //Sounds::startBackgroundMusic(SoundType::BACKGROUND_MUSIC_1);

        //BR_INFO(" %f", );
        //BR_INFO("%s", "");
    }

    Map5::~Map5()
    {
        Sounds::stopBackgroundMusic();
    }

    void Map5::draw()
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

    void Map5::loadEnv()
    {
        const auto mainGround = std::make_shared<Beryll::SimpleCollidingObject>("models3D/map5/MainGround.fbx",
                                                                                0.0f,
                                                                                false,
                                                                                Beryll::CollisionFlags::STATIC,
                                                                                Beryll::CollisionGroups::GROUND,
                                                                                Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE |
                                                                                Beryll::CollisionGroups::MOVABLE_ENEMY,
                                                                                Beryll::SceneObjectGroups::GROUND);

        m_objWithNormalMap.push_back(mainGround);
        mainGround->setFriction(EnumsAndVars::staticEnvFriction);

        const auto heapNormalMap = std::make_shared<Beryll::SimpleCollidingObject>("models3D/map5/GroundHeapNormalMap.fbx",
                                                                                   0.0f,
                                                                                   false,
                                                                                   Beryll::CollisionFlags::STATIC,
                                                                                   Beryll::CollisionGroups::GROUND,
                                                                                   Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE |
                                                                                   Beryll::CollisionGroups::CAMERA | Beryll::CollisionGroups::MOVABLE_ENEMY |
                                                                                   Beryll::CollisionGroups::RAY_FOR_ENVIRONMENT,
                                                                                   Beryll::SceneObjectGroups::GROUND);

        m_objWithNormalMap.push_back(heapNormalMap);
        heapNormalMap->setFriction(EnumsAndVars::staticEnvFriction);

        const auto otherGrounds = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map5/OtherGrounds.fbx",
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
            obj->setFriction(EnumsAndVars::staticEnvFriction);
        }

        const auto buildings = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map5/Buildings.fbx",
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

        const auto jumpPads = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map5/JumpPads.fbx",
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

        const auto envNoColliders1 = Beryll::SimpleObject::loadManyModelsFromOneFile("models3D/map5/EnvNoColliders.fbx", Beryll::SceneObjectGroups::BUILDING);

        for(const auto& obj : envNoColliders1)
        {
            m_staticEnv.push_back(obj);
            m_simpleObjForShadowMap.push_back(obj);
        }
    }

    void Map5::loadGarbage()
    {

    }

    void Map5::loadEnemies()
    {

    }

    void Map5::spawnEnemies()
    {

    }
}
