#include "Map1.h"
#include "EnumsAndVariables.h"
#include "enemies/CopWithPistol.h"
#include "enemies/CopWithPistolShield.h"
#include "enemies/CopWithGrenadeLauncher.h"
#include "enemies/Tank.h"
#include "enemies/Sniper.h"
#include "bosses/TankWithCommander.h"
#include "Sounds.h"

namespace MagneticBall3D
{
    Map1::Map1(std::shared_ptr<PlayStateGUILayer> gui) : BaseMap(std::move(gui))
    {
        Beryll::LoadingScreen::setTextureByIndex(0);
        Beryll::LoadingScreen::showProgress(10.0f);

        // Specific for this map only.
        loadPlayer();
        Beryll::LoadingScreen::showProgress(20.0f);
        loadEnv();
        Beryll::LoadingScreen::showProgress(40.0f);
        loadGarbage();
        Beryll::LoadingScreen::showProgress(60.0f);
        loadEnemies();
        Beryll::LoadingScreen::showProgress(80.0f);
        loadBoss();
        Beryll::LoadingScreen::showProgress(90.0f);

        Sniper::sniperPositions = std::vector<SniperPosAndAttackDist>{{glm::vec3(65.0f, 255.0f, -642.0f), 2500.0f},
                                                                      {glm::vec3(65.0f, 252.0f, -445.0f), 2500.0f},
                                                                      {glm::vec3(65.0f, 214.0f, -264.0f), 2500.0f},
                                                                      {glm::vec3(56.5f, 250.0f, -129.5f), 2500.0f},
                                                                      {glm::vec3(71.0f, 250.0f, -60.0f),  2500.0f},
                                                                      {glm::vec3(129.0f, 250.0f, -55.5f), 2500.0f},
                                                                      {glm::vec3(260.5f, 215.0f, -61.0f), 2500.0f},
                                                                      {glm::vec3(320.0f, 215.0f, -61.0f), 2500.0f},
                                                                      {glm::vec3(458.5f, 242.0f, -62.0f), 2500.0f},
                                                                      {glm::vec3(639.0f, 255.0f, -58.0f), 2500.0f},
                                                                      {glm::vec3(56.0f, 305.5f, 414.5f),  2500.0f},
                                                                      {glm::vec3(56.0f, 305.5f, 497.5f), 2500.0f},
                                                                      {glm::vec3(179.0f, 305.5f, 396.5f), 2500.0f},

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
        m_pointsToSpawnEnemies.reserve(m_pathAllowedPositionsXZ.size());
        m_pointsToSpawnCommonGarbage.reserve(m_pathAllowedPositionsXZ.size());

        m_pathFinderBoss = AStar(m_minX, m_maxX, m_minZ, m_maxZ, 40);
        std::vector<glm::vec3> wallsBoss = BeryllUtils::Common::loadMeshVerticesToVector("models3D/map1/PathBossWalls.fbx");
        for(const auto& wall : wallsBoss)
        {
            m_pathFinderBoss.addWallPosition({(int)std::roundf(wall.x), (int)std::roundf(wall.z)});
        }

        BR_INFO("Map1 pathfinder boss walls: %d", walls.size());

        std::vector<glm::vec3> allowedPointsBoss = BeryllUtils::Common::loadMeshVerticesToVector("models3D/map1/PathBossAllowedPositions.fbx");
        m_pathAllowedPositionsXZBoss.reserve(allowedPointsBoss.size());
        for(const auto& point : allowedPointsBoss)
        {
            m_pathAllowedPositionsXZBoss.push_back({(int)std::roundf(point.x), (int)std::roundf(point.z)});
        }

        BR_INFO("Map1 pathfinder allowed points boss: %d", m_pathAllowedPositionsXZBoss.size());

        m_dirToSun = glm::normalize(glm::vec3(-1.0f, 3.5f, -0.8f));
        m_sunLightDir = -m_dirToSun;
        m_sunDistance = 600.0f;

        std::vector<ImprovementInfo> imprInfoVector{{ImprovementType::PLAYER_MAX_SPEED,           5, {{[&]() { EnAndVars::playerMaxSpeedXZ += 10.0f; BR_INFO("%s", "Block PLAYER_MAX_SPEED pressed 0."); }},
                                                                                                                {[&]() { EnAndVars::playerMaxSpeedXZ += 15.0f; BR_INFO("%s", "Block PLAYER_MAX_SPEED pressed 1."); }},
                                                                                                                {[&]() { EnAndVars::playerMaxSpeedXZ += 20.0f; BR_INFO("%s", "Block PLAYER_MAX_SPEED pressed 2."); }},
                                                                                                                {[&]() { EnAndVars::playerMaxSpeedXZ += 25.0f; BR_INFO("%s", "Block PLAYER_MAX_SPEED pressed 3."); }},
                                                                                                                {[&]() { EnAndVars::playerMaxSpeedXZ += 30.0f; BR_INFO("%s", "Block PLAYER_MAX_SPEED pressed 4."); }}}},
                                              {ImprovementType::PLAYER_ACCELERATE_FASTER,   5, {{[&]() { EnAndVars::playerImpulseFactorOnGround += 0.01f;
                                                                                                                                  EnAndVars::playerTorqueFactorOnGround += 0.01f;
                                                                                                                                  EnAndVars::playerImpulseFactorOnBuildingRoof += 0.008f;
                                                                                                                                  EnAndVars::playerTorqueFactorOnBuildingRoof += 0.008f;
                                                                                                                                  BR_INFO("%s", "Block PLAYER_ACCELERATE_FASTER pressed 0."); }},
                                                                                                                {[&]() { EnAndVars::playerImpulseFactorOnGround += 0.013f;
                                                                                                                                  EnAndVars::playerTorqueFactorOnGround += 0.013f;
                                                                                                                                  EnAndVars::playerImpulseFactorOnBuildingRoof += 0.009f;
                                                                                                                                  EnAndVars::playerTorqueFactorOnBuildingRoof += 0.009f;
                                                                                                                                  BR_INFO("%s", "Block PLAYER_ACCELERATE_FASTER pressed 1."); }},
                                                                                                                {[&]() { EnAndVars::playerImpulseFactorOnGround += 0.016f;
                                                                                                                                  EnAndVars::playerTorqueFactorOnGround += 0.016f;
                                                                                                                                  EnAndVars::playerImpulseFactorOnBuildingRoof += 0.01f;
                                                                                                                                  EnAndVars::playerTorqueFactorOnBuildingRoof += 0.01f;
                                                                                                                                  BR_INFO("%s", "Block PLAYER_ACCELERATE_FASTER pressed 2."); }},
                                                                                                                {[&]() { EnAndVars::playerImpulseFactorOnGround += 0.019f;
                                                                                                                                  EnAndVars::playerTorqueFactorOnGround += 0.019f;
                                                                                                                                  EnAndVars::playerImpulseFactorOnBuildingRoof += 0.011f;
                                                                                                                                  EnAndVars::playerTorqueFactorOnBuildingRoof += 0.011f;
                                                                                                                                  BR_INFO("%s", "Block PLAYER_ACCELERATE_FASTER pressed 3."); }},
                                                                                                                {[&]() { EnAndVars::playerImpulseFactorOnGround += 0.022f;
                                                                                                                                  EnAndVars::playerTorqueFactorOnGround += 0.022f;
                                                                                                                                  EnAndVars::playerImpulseFactorOnBuildingRoof += 0.012f;
                                                                                                                                  EnAndVars::playerTorqueFactorOnBuildingRoof += 0.012f;
                                                                                                                                  BR_INFO("%s", "Block PLAYER_ACCELERATE_FASTER pressed 4."); }}}},
                                              {ImprovementType::PLAYER_MOVE_FASTER_THROUGH_ENEMIES,  5, {{[&]() { EnAndVars::playerSpeedReductionMultiplier -= 0.05f; BR_INFO("%s", "Block PLAYER_MOVE_FASTER_THROUGH_ENEMIES pressed 0."); }},
                                                                                                                         {[&]() { EnAndVars::playerSpeedReductionMultiplier -= 0.075f; BR_INFO("%s", "Block PLAYER_MOVE_FASTER_THROUGH_ENEMIES pressed 1."); }},
                                                                                                                         {[&]() { EnAndVars::playerSpeedReductionMultiplier -= 0.1f; BR_INFO("%s", "Block PLAYER_MOVE_FASTER_THROUGH_ENEMIES pressed 2."); }},
                                                                                                                         {[&]() { EnAndVars::playerSpeedReductionMultiplier -= 0.125f; BR_INFO("%s", "Block PLAYER_MOVE_FASTER_THROUGH_ENEMIES pressed 3."); }},
                                                                                                                         {[&]() { EnAndVars::playerSpeedReductionMultiplier -= 0.15f; BR_INFO("%s", "Block PLAYER_MOVE_FASTER_THROUGH_ENEMIES pressed 4."); }}}},
                                              {ImprovementType::PLAYER_BETTER_CLUTCH_WITH_BUILDINGS, 5, {{[&]() { EnAndVars::playerTorqueFactorOnBuildingWall += 0.03f;  BR_INFO("%s", "Block PLAYER_BETTER_CLUTCH_WITH_BUILDINGS pressed 0."); }},
                                                                                                                         {[&]() { EnAndVars::playerTorqueFactorOnBuildingWall += 0.035f; BR_INFO("%s", "Block PLAYER_BETTER_CLUTCH_WITH_BUILDINGS pressed 1."); }},
                                                                                                                         {[&]() { EnAndVars::playerTorqueFactorOnBuildingWall += 0.04f;  BR_INFO("%s", "Block PLAYER_BETTER_CLUTCH_WITH_BUILDINGS pressed 2."); }},
                                                                                                                         {[&]() { EnAndVars::playerTorqueFactorOnBuildingWall += 0.045f; BR_INFO("%s", "Block PLAYER_BETTER_CLUTCH_WITH_BUILDINGS pressed 3."); }},
                                                                                                                         {[&]() { EnAndVars::playerTorqueFactorOnBuildingWall += 0.05f;  BR_INFO("%s", "Block PLAYER_BETTER_CLUTCH_WITH_BUILDINGS pressed 4."); }}}},
                                              {ImprovementType::PLAYER_INCREASE_SIZE,                5, {{[&]() { EnAndVars::playerMagneticRadius += 3.0f;
                                                                                                                                           EnAndVars::garbageMaxCountMagnetized += 14;
                                                                                                                                           EnAndVars::playerTorqueFactorOnBuildingWall += 0.01f;
                                                                                                                                           m_player->selectNextModel();
                                                                                                                                           BR_INFO("%s", "Block PLAYER_INCREASE_SIZE pressed 0."); }},
                                                                                                                         {[&]() { EnAndVars::playerMagneticRadius += 4.0f;
                                                                                                                                           EnAndVars::garbageMaxCountMagnetized += 21;
                                                                                                                                           EnAndVars::playerTorqueFactorOnBuildingWall += 0.011f;
                                                                                                                                           m_player->selectNextModel();
                                                                                                                                           BR_INFO("%s", "Block PLAYER_INCREASE_SIZE pressed 1."); }},
                                                                                                                         {[&]() { EnAndVars::playerMagneticRadius += 5.0f;
                                                                                                                                           EnAndVars::garbageMaxCountMagnetized += 29;
                                                                                                                                           EnAndVars::playerTorqueFactorOnBuildingWall += 0.012f;
                                                                                                                                           m_player->selectNextModel();
                                                                                                                                           BR_INFO("%s", "Block PLAYER_INCREASE_SIZE pressed 2."); }},
                                                                                                                         {[&]() { EnAndVars::playerMagneticRadius += 6.0f;
                                                                                                                                           EnAndVars::garbageMaxCountMagnetized += 38;
                                                                                                                                           EnAndVars::playerTorqueFactorOnBuildingWall += 0.014f;
                                                                                                                                           m_player->selectNextModel();
                                                                                                                                           BR_INFO("%s", "Block PLAYER_INCREASE_SIZE pressed 3."); }},
                                                                                                                         {[&]() { EnAndVars::playerMagneticRadius += 7.0f;
                                                                                                                                           EnAndVars::garbageMaxCountMagnetized += 47;
                                                                                                                                           EnAndVars::playerTorqueFactorOnBuildingWall += 0.017f;
                                                                                                                                           m_player->selectNextModel();
                                                                                                                                           BR_INFO("%s", "Block PLAYER_INCREASE_SIZE pressed 4."); }},
                                                                                                                }},
                                              {ImprovementType::PLAYER_INCREASE_MAX_HP,              5, {{[&]() { m_player->addToMaxHP(20); BR_INFO("%s", "Block PLAYER_INCREASE_MAX_HP pressed 0."); }},
                                                                                                                         {[&]() { m_player->addToMaxHP(40); BR_INFO("%s", "Block PLAYER_INCREASE_MAX_HP pressed 1."); }},
                                                                                                                         {[&]() { m_player->addToMaxHP(60); BR_INFO("%s", "Block PLAYER_INCREASE_MAX_HP pressed 2."); }},
                                                                                                                         {[&]() { m_player->addToMaxHP(80); BR_INFO("%s", "Block PLAYER_INCREASE_MAX_HP pressed 3."); }},
                                                                                                                         {[&]() { m_player->addToMaxHP(100); BR_INFO("%s", "Block PLAYER_INCREASE_MAX_HP pressed 4."); }}}},
                                              {ImprovementType::PLAYER_REDUCE_DAMAGE,                5, {{[&]() { EnAndVars::playerDamageTakenMultiplier -= 0.05f;  BR_INFO("%s", "Block PLAYER_REDUCE_DAMAGE pressed 0."); }},
                                                                                                                         {[&]() { EnAndVars::playerDamageTakenMultiplier -= 0.075f; BR_INFO("%s", "Block PLAYER_REDUCE_DAMAGE pressed 1."); }},
                                                                                                                         {[&]() { EnAndVars::playerDamageTakenMultiplier -= 0.1f;   BR_INFO("%s", "Block PLAYER_REDUCE_DAMAGE pressed 2."); }},
                                                                                                                         {[&]() { EnAndVars::playerDamageTakenMultiplier -= 0.125f; BR_INFO("%s", "Block PLAYER_REDUCE_DAMAGE pressed 3."); }},
                                                                                                                         {[&]() { EnAndVars::playerDamageTakenMultiplier -= 0.15f;  BR_INFO("%s", "Block PLAYER_REDUCE_DAMAGE pressed 4."); }}}},
                                              {ImprovementType::PLAYER_TAKE_MORE_XP,                 5, {{[&]() { EnAndVars::playerXPMultiplier += 0.03f; BR_INFO("%s", "Block PLAYER_TAKE_MORE_XP pressed 0."); }},
                                                                                                                         {[&]() { EnAndVars::playerXPMultiplier += 0.04f; BR_INFO("%s", "Block PLAYER_TAKE_MORE_XP pressed 1."); }},
                                                                                                                         {[&]() { EnAndVars::playerXPMultiplier += 0.05f; BR_INFO("%s", "Block PLAYER_TAKE_MORE_XP pressed 2."); }},
                                                                                                                         {[&]() { EnAndVars::playerXPMultiplier += 0.06f; BR_INFO("%s", "Block PLAYER_TAKE_MORE_XP pressed 3."); }},
                                                                                                                         {[&]() { EnAndVars::playerXPMultiplier += 0.07f; BR_INFO("%s", "Block PLAYER_TAKE_MORE_XP pressed 4."); }}}},
                                              {ImprovementType::PLAYER_HEAL_AT_NEW_LVL,              5, {{[&]() { EnAndVars::playerRestoreHPAtNewLevel += m_player->getMaxHP() * 0.01f; BR_INFO("%s", "Block PLAYER_HEAL_AT_NEW_LVL pressed 0."); }},
                                                                                                                         {[&]() { EnAndVars::playerRestoreHPAtNewLevel += m_player->getMaxHP() * 0.02f; BR_INFO("%s", "Block PLAYER_HEAL_AT_NEW_LVL pressed 1."); }},
                                                                                                                         {[&]() { EnAndVars::playerRestoreHPAtNewLevel += m_player->getMaxHP() * 0.03f; BR_INFO("%s", "Block PLAYER_HEAL_AT_NEW_LVL pressed 2."); }},
                                                                                                                         {[&]() { EnAndVars::playerRestoreHPAtNewLevel += m_player->getMaxHP() * 0.04f; BR_INFO("%s", "Block PLAYER_HEAL_AT_NEW_LVL pressed 3."); }},
                                                                                                                         {[&]() { EnAndVars::playerRestoreHPAtNewLevel += m_player->getMaxHP() * 0.05f; BR_INFO("%s", "Block PLAYER_HEAL_AT_NEW_LVL pressed 4."); }}}},
                                              {ImprovementType::GARBAGE_SPAWN_MORE_ON_MAP,           5, {{[&]() { EnAndVars::garbageCommonSpawnDelay -= 0.05f; BR_INFO("%s", "Block GARBAGE_SPAWN_MORE_ON_MAP pressed 0."); }},
                                                                                                                         {[&]() { EnAndVars::garbageCommonSpawnDelay -= 0.07f; BR_INFO("%s", "Block GARBAGE_SPAWN_MORE_ON_MAP pressed 1."); }},
                                                                                                                         {[&]() { EnAndVars::garbageCommonSpawnDelay -= 0.09f; BR_INFO("%s", "Block GARBAGE_SPAWN_MORE_ON_MAP pressed 2."); }},
                                                                                                                         {[&]() { EnAndVars::garbageCommonSpawnDelay -= 0.11f; BR_INFO("%s", "Block GARBAGE_SPAWN_MORE_ON_MAP pressed 3."); }},
                                                                                                                         {[&]() { EnAndVars::garbageCommonSpawnDelay -= 0.13f; BR_INFO("%s", "Block GARBAGE_SPAWN_MORE_ON_MAP pressed 4."); }}}},
                                              {ImprovementType::GARBAGE_REDUCE_DAMAGE,               5, {{[&]() { EnAndVars::garbageDamageTakenMultiplier -= 0.05f;  BR_INFO("%s", "Block GARBAGE_REDUCE_DAMAGE pressed 0."); }},
                                                                                                                         {[&]() { EnAndVars::garbageDamageTakenMultiplier -= 0.075f; BR_INFO("%s", "Block GARBAGE_REDUCE_DAMAGE pressed 1."); }},
                                                                                                                         {[&]() { EnAndVars::garbageDamageTakenMultiplier -= 0.1f;   BR_INFO("%s", "Block GARBAGE_REDUCE_DAMAGE pressed 2."); }},
                                                                                                                         {[&]() { EnAndVars::garbageDamageTakenMultiplier -= 0.125f; BR_INFO("%s", "Block GARBAGE_REDUCE_DAMAGE pressed 3."); }},
                                                                                                                         {[&]() { EnAndVars::garbageDamageTakenMultiplier -= 0.15f;  BR_INFO("%s", "Block GARBAGE_REDUCE_DAMAGE pressed 4."); }}}}
                                              };


        m_improvements = Improvements(m_player, imprInfoVector);
        m_skyBox = Beryll::Renderer::createSkyBox("skyboxes/map1");

        EnAndVars::mapCurrentNumber = 1;

        Beryll::GoogleAnalytics::getInstance()->sendEventEmpty("Map1Start");

        Beryll::LoadingScreen::showProgress(100.0f);

        //BR_INFO(" %f", );
        //BR_INFO("%s", "");
    }

    Map1::~Map1()
    {

    }

    void Map1::draw()
    {
        m_improvements.draw();

        //BR_INFO("%s", "scene draw call");
        // 1. Draw into shadow map.
        glm::vec3 sunPos = m_player->getObj()->getOrigin() + (Beryll::Camera::getCameraFrontDirectionXZ() * 250.0f) + (m_dirToSun * m_sunDistance);
        updateSunPosition(sunPos, 700, 700, m_sunDistance * 2.0f);
        Beryll::Renderer::disableFaceCulling();
        m_shadowMap->drawIntoShadowMap(m_simpleObjForShadowMap, {}, m_sunLightVPMatrix);
        Beryll::Renderer::enableFaceCulling();

        // 2. Draw scene.
        glm::mat4 modelMatrix{1.0f};

        m_animatedObjSunLight->bind();
        m_animatedObjSunLight->set3Float("sunLightDir", m_sunLightDir);
        m_animatedObjSunLight->set1Float("ambientLight", m_gui->sliderAmbient->getValue());

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

        // If player is on building roof this building should be semitransparent.
        std::shared_ptr<Beryll::BaseSimpleObject> semiTransparentBuilding = nullptr;

        m_simpleObjSunLightShadows->bind();
        m_simpleObjSunLightShadows->set3Float("sunLightDir", m_sunLightDir);
        m_simpleObjSunLightShadows->set3Float("cameraPos", Beryll::Camera::getCameraPos());
        m_simpleObjSunLightShadows->set1Float("ambientLight", m_gui->sliderAmbient->getValue());
        m_simpleObjSunLightShadows->set1Float("specularLightStrength", 1.5f);
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

        m_simpleObjSunLightShadows->set1Float("specularLightStrength", 1.0f);

        for(const auto& staticObj : m_staticEnv)
        {
            if(staticObj->getIsEnabledDraw())
            {
                if(m_player->getIsOnBuildingRoof() && staticObj->getID() == m_player->getCollidingBuildingID())
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

    void Map1::loadPlayer()
    {
        auto playerAllBalls = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/player/Player6Models.fbx",
                                                                                       EnAndVars::playerMass,
                                                                                       true,
                                                                                       Beryll::CollisionFlags::DYNAMIC,
                                                                                       Beryll::CollisionGroups::PLAYER,
                                                                                Beryll::CollisionGroups::GROUND | Beryll::CollisionGroups::BUILDING |
                                                                                Beryll::CollisionGroups::GARBAGE | Beryll::CollisionGroups::ENEMY_ATTACK |
                                                                                Beryll::CollisionGroups::JUMPPAD | Beryll::CollisionGroups::BOSS,
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

        m_player = std::make_shared<Player>(playerAllBalls[0], EnAndVars::playerStartHP);
        m_player->setAllModels(playerAllBalls);

        m_player->getObj()->setOrigin(glm::vec3(-770.0f, 2.0f,0.0f));
        m_player->getObj()->setGravity(EnAndVars::playerGravityOnGround);
        m_player->getObj()->setFriction(EnAndVars::playerFriction);
        m_player->getObj()->setDamping(EnAndVars::playerLinearDamping, EnAndVars::playerAngularDamping);
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
        ground->setFriction(EnAndVars::staticEnvFriction);

        const auto objects1 = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map1/Buildings.fbx",
                                                                                       0.0f,
                                                                                       false,
                                                                                       Beryll::CollisionFlags::STATIC,
                                                                                       Beryll::CollisionGroups::BUILDING,
                                                                                       Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::GARBAGE |
                                                                                       Beryll::CollisionGroups::RAY_FOR_BUILDING_CHECK | Beryll::CollisionGroups::CAMERA,
                                                                                       Beryll::SceneObjectGroups::BUILDING);

        for(const auto& obj : objects1)
        {
            m_staticEnv.push_back(obj);
            m_simpleObjForShadowMap.push_back(obj);
            obj->setFriction(EnAndVars::staticEnvFriction);
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
        for(int i = 0; i < 8; ++i)
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
                m_allGarbage.emplace_back(obj, GarbageType::COMMON, 30);
                m_allGarbage.back().disableGarbage();

                m_animatedOrDynamicObjects.push_back(obj);
                m_simpleObjForShadowMap.push_back(obj);

                obj->setDamping(EnAndVars::garbageLinearDamping, EnAndVars::garbageAngularDamping);
                obj->setGravity(EnAndVars::garbageGravityDefault, false, false);
            }
        }
    }

    void Map1::loadEnemies()
    {
        for(int i = 0; i < 150; ++i)
        {
            auto copPistol = std::make_shared<CopWithPistol>("models3D/enemies/CopWithPistol.fbx",
                                                        0.0f,
                                                        false,
                                                        Beryll::CollisionFlags::STATIC,
                                                        Beryll::CollisionGroups::NONE,
                                                        Beryll::CollisionGroups::NONE,
                                                        Beryll::SceneObjectGroups::ENEMY);

            copPistol->setCurrentAnimationByIndex(EnAndVars::AnimationIndexes::run, false, false);
            copPistol->setDefaultAnimationByIndex(EnAndVars::AnimationIndexes::stand);

            m_animatedOrDynamicObjects.push_back(copPistol);
            m_allAnimatedEnemies.push_back(copPistol);
        }

        for(int i = 0; i < 150; ++i)
        {
            auto copShield = std::make_shared<CopWithPistolShield>("models3D/enemies/CopWithPistolShield.fbx",
                                                               0.0f,
                                                               false,
                                                               Beryll::CollisionFlags::STATIC,
                                                               Beryll::CollisionGroups::NONE,
                                                               Beryll::CollisionGroups::NONE,
                                                               Beryll::SceneObjectGroups::ENEMY);

            copShield->setCurrentAnimationByIndex(EnAndVars::AnimationIndexes::run, false, false);
            copShield->setDefaultAnimationByIndex(EnAndVars::AnimationIndexes::stand);

            m_animatedOrDynamicObjects.push_back(copShield);
            m_allAnimatedEnemies.push_back(copShield);
        }

        for(int i = 0; i < 13; ++i)
        {
            auto sniper = std::make_shared<Sniper>("models3D/enemies/Sniper.fbx",
                                                   0.0f,
                                                   false,
                                                   Beryll::CollisionFlags::STATIC,
                                                   Beryll::CollisionGroups::NONE,
                                                   Beryll::CollisionGroups::NONE,
                                                   Beryll::SceneObjectGroups::ENEMY);

            sniper->setCurrentAnimationByIndex(EnAndVars::AnimationIndexes::stand, false, false);
            sniper->setDefaultAnimationByIndex(EnAndVars::AnimationIndexes::stand);

            m_animatedOrDynamicObjects.push_back(sniper);
            m_allAnimatedEnemies.push_back(sniper);
        }

        for(int i = 0; i < 60; ++i)
        {
            auto copGrenade = std::make_shared<CopWithGrenadeLauncher>("models3D/enemies/CopWithGrenadeLauncher.fbx",
                                                                  0.0f,
                                                                  false,
                                                                  Beryll::CollisionFlags::STATIC,
                                                                  Beryll::CollisionGroups::NONE,
                                                                  Beryll::CollisionGroups::NONE,
                                                                  Beryll::SceneObjectGroups::ENEMY);

            copGrenade->setCurrentAnimationByIndex(EnAndVars::AnimationIndexes::run, false, false);
            copGrenade->setDefaultAnimationByIndex(EnAndVars::AnimationIndexes::stand);

            m_animatedOrDynamicObjects.push_back(copGrenade);
            m_allAnimatedEnemies.push_back(copGrenade);
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

            tank->setCurrentAnimationByIndex(EnAndVars::AnimationIndexes::run, false, false);
            tank->setDefaultAnimationByIndex(EnAndVars::AnimationIndexes::stand);

            m_animatedOrDynamicObjects.push_back(tank);
            m_allAnimatedEnemies.push_back(tank);
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

        m_boss->setCurrentAnimationByIndex(EnAndVars::AnimationIndexes::run, false, false);
        m_boss->setDefaultAnimationByIndex(EnAndVars::AnimationIndexes::stand);

        m_animatedOrDynamicObjects.push_back(m_boss);
    }

    void Map1::spawnEnemies()
    {
        // Prepare waves.
        if(m_prepareWave1 && EnAndVars::mapPlayTimeSec > m_enemiesWave1Time)
        {
            m_prepareWave1 = false;

            EnAndVars::enemiesMaxActiveCountOnGround = 0;

            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;
            }

            BR_INFO("prepare wave 1. Max enemies: %d", EnAndVars::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave2 && EnAndVars::mapPlayTimeSec > m_enemiesWave2Time)
        {
            m_prepareWave2 = false;

            EnAndVars::enemiesMaxActiveCountOnGround = 0;

            int copWithPistolCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;

                if(copWithPistolCount < 50 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolCount;
                    ++EnAndVars::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("prepare wave 2. Max enemies: %d", EnAndVars::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave3 && EnAndVars::mapPlayTimeSec > m_enemiesWave3Time)
        {
            m_prepareWave3 = false;

            EnAndVars::enemiesMaxActiveCountOnGround = 0;

            int copWithPistolCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;
                enemy->damage += 1.0f;

                if(copWithPistolCount < 75 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolCount;
                    ++EnAndVars::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("prepare wave 3. Max enemies: %d", EnAndVars::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave4 && EnAndVars::mapPlayTimeSec > m_enemiesWave4Time)
        {
            m_prepareWave4 = false;

            EnAndVars::enemiesMaxActiveCountOnGround = 0;

            int copWithPistolCount = 0;
            int copWithPistolShieldCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;
                enemy->damage += 1.0f;

                if(copWithPistolCount < 75 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolCount;
                    ++EnAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(copWithPistolShieldCount < 40 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL_SHIELD)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolShieldCount;
                    ++EnAndVars::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("prepare wave 4. Max enemies: %d", EnAndVars::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave5 && EnAndVars::mapPlayTimeSec > m_enemiesWave5Time)
        {
            m_prepareWave5 = false;

            EnAndVars::enemiesMaxActiveCountOnGround = 0;

            int copWithPistolCount = 0;
            int copWithPistolShieldCount = 0;
            int sniperCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;
                enemy->damage += 1.0f;

                if(copWithPistolCount < 75 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolCount;
                    ++EnAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(copWithPistolShieldCount < 75 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL_SHIELD)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolShieldCount;
                    ++EnAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(sniperCount < 4 && enemy->getUnitType() == UnitType::SNIPER)
                {
                    enemy->isCanBeSpawned = true;
                    ++sniperCount;
                    ++EnAndVars::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("prepare wave 5. Max enemies: %d", EnAndVars::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave6 && EnAndVars::mapPlayTimeSec > m_enemiesWave6Time)
        {
            m_prepareWave6 = false;

            EnAndVars::enemiesMaxActiveCountOnGround = 0;

            int copWithPistolCount = 0;
            int copWithPistolShieldCount = 0;
            int sniperCount = 0;
            int copWithGrenadeLauncherCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;
                enemy->damage += 1.0f;

                if(copWithPistolCount < 75 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolCount;
                    ++EnAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(copWithPistolShieldCount < 75 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL_SHIELD)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolShieldCount;
                    ++EnAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(sniperCount < 8 && enemy->getUnitType() == UnitType::SNIPER)
                {
                    enemy->isCanBeSpawned = true;
                    ++sniperCount;
                    ++EnAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(copWithGrenadeLauncherCount < 30 && enemy->getUnitType() == UnitType::COP_WITH_GRENADE_LAUNCHER)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithGrenadeLauncherCount;
                    ++EnAndVars::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("prepare wave 6. Max enemies: %d", EnAndVars::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave7 && EnAndVars::mapPlayTimeSec > m_enemiesWave7Time)
        {
            m_prepareWave7 = false;

            EnAndVars::enemiesMaxActiveCountOnGround = 0;

            int copWithPistolCount = 0;
            int copWithPistolShieldCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;
                enemy->damage += 1.0f;

                if(copWithPistolCount < 100 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolCount;
                    ++EnAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(copWithPistolShieldCount < 100 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL_SHIELD)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolShieldCount;
                    ++EnAndVars::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("prepare wave 7. Max enemies: %d", EnAndVars::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave8 && EnAndVars::mapPlayTimeSec > m_enemiesWave8Time)
        {
            m_prepareWave8 = false;

            EnAndVars::enemiesMaxActiveCountOnGround = 0;

            int copWithPistolCount = 0;
            int copWithPistolShieldCount = 0;
            int sniperCount = 0;
            int copWithGrenadeLauncherCount = 0;
            int tankCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;
                enemy->damage += 1.0f;

                if(copWithPistolCount < 100 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolCount;
                    ++EnAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(copWithPistolShieldCount < 100 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL_SHIELD)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolShieldCount;
                    ++EnAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(sniperCount < 13 && enemy->getUnitType() == UnitType::SNIPER)
                {
                    enemy->isCanBeSpawned = true;
                    ++sniperCount;
                    ++EnAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(copWithGrenadeLauncherCount < 40 && enemy->getUnitType() == UnitType::COP_WITH_GRENADE_LAUNCHER)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithGrenadeLauncherCount;
                    ++EnAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(tankCount < 5 && enemy->getUnitType() == UnitType::TANK)
                {
                    enemy->isCanBeSpawned = true;
                    ++tankCount;
                    ++EnAndVars::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("prepare wave 8. Max enemies: %d", EnAndVars::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave9 && EnAndVars::mapPlayTimeSec > m_enemiesWave9Time)
        {
            m_prepareWave9 = false;

            EnAndVars::enemiesMaxActiveCountOnGround = 0;

            int copWithPistolCount = 0;
            int copWithPistolShieldCount = 0;
            int sniperCount = 0;
            int copWithGrenadeLauncherCount = 0;
            int tankCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;
                enemy->damage += 1.0f;

                if(copWithPistolCount < 125 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolCount;
                    ++EnAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(copWithPistolShieldCount < 125 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL_SHIELD)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolShieldCount;
                    ++EnAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(sniperCount < 13 && enemy->getUnitType() == UnitType::SNIPER)
                {
                    enemy->isCanBeSpawned = true;
                    ++sniperCount;
                    ++EnAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(copWithGrenadeLauncherCount < 50 && enemy->getUnitType() == UnitType::COP_WITH_GRENADE_LAUNCHER)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithGrenadeLauncherCount;
                    ++EnAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(tankCount < 10 && enemy->getUnitType() == UnitType::TANK)
                {
                    enemy->isCanBeSpawned = true;
                    ++tankCount;
                    ++EnAndVars::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("prepare wave 9. Max enemies: %d", EnAndVars::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareWave10 && EnAndVars::mapPlayTimeSec > m_enemiesWave10Time)
        {
            m_prepareWave10 = false;

            EnAndVars::enemiesMaxActiveCountOnGround = 0;

            int copWithPistolCount = 0;
            int copWithPistolShieldCount = 0;
            int sniperCount = 0;
            int copWithGrenadeLauncherCount = 0;
            int tankCount = 0;
            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;
                enemy->damage += 1.0f;

                if(copWithPistolCount < 150 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolCount;
                    ++EnAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(copWithPistolShieldCount < 150 && enemy->getUnitType() == UnitType::COP_WITH_PISTOL_SHIELD)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithPistolShieldCount;
                    ++EnAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(sniperCount < 13 && enemy->getUnitType() == UnitType::SNIPER)
                {
                    enemy->isCanBeSpawned = true;
                    ++sniperCount;
                    ++EnAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(copWithGrenadeLauncherCount < 60 && enemy->getUnitType() == UnitType::COP_WITH_GRENADE_LAUNCHER)
                {
                    enemy->isCanBeSpawned = true;
                    ++copWithGrenadeLauncherCount;
                    ++EnAndVars::enemiesMaxActiveCountOnGround;
                }
                else if(tankCount < 20 && enemy->getUnitType() == UnitType::TANK)
                {
                    enemy->isCanBeSpawned = true;
                    ++tankCount;
                    ++EnAndVars::enemiesMaxActiveCountOnGround;
                }
            }

            BR_INFO("prepare wave 10. Max enemies: %d", EnAndVars::enemiesMaxActiveCountOnGround);
        }
        else if(m_prepareToBoss && EnAndVars::mapPlayTimeSec > m_prepareToBossTime)
        {
            m_prepareToBoss = false;

            EnAndVars::enemiesMaxActiveCountOnGround = 0;

            for(auto& enemy : m_allAnimatedEnemies)
            {
                enemy->isCanBeSpawned = false;
            }

            prepareToBossPhase();

            BR_INFO("prepareToBossPhase(). Max enemies: %d", EnAndVars::enemiesMaxActiveCountOnGround);
        }

        bool sortSnipersPositions = true;

        // Spawn enemies on ground.
        if(!m_pointsToSpawnEnemies.empty())
        {
            for(const auto& enemy : m_allAnimatedEnemies)
            {
                if(BaseEnemy::getActiveCount() >= EnAndVars::enemiesMaxActiveCountOnGround)
                    break;

                if(enemy->getIsEnabledUpdate() || !enemy->isCanBeSpawned)
                    continue;

                if(enemy->getUnitType() != UnitType::SNIPER)
                {
                    enemy->enableEnemy();
                    enemy->disableDraw();

                    const glm::ivec2& spawnPoint2D = m_pointsToSpawnEnemies[Beryll::RandomGenerator::getInt(m_pointsToSpawnEnemies.size() - 1)];
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
                }
                // Spawn sniper.
                else if(Sniper::spawnTime + Sniper::spawnDelay < EnAndVars::mapPlayTimeSec)
                {
                    if(sortSnipersPositions)
                    {
                        sortSnipersPositions = false;

                        // Sort by distance from player.
                        std::sort(Sniper::sniperPositions.begin(), Sniper::sniperPositions.end(),
                                  [&](const SniperPosAndAttackDist& pos1, const SniperPosAndAttackDist& pos2)
                                  { return glm::distance(m_player->getObj()->getOrigin(), pos1.position) < glm::distance(m_player->getObj()->getOrigin(), pos2.position); });
                    }

                    for(SniperPosAndAttackDist& posAndDist : Sniper::sniperPositions)
                    {
                        Beryll::RayClosestHit rayBuildingHit = Beryll::Physics::castRayClosestHit(m_player->getObj()->getOrigin(),
                                                                                                  posAndDist.position,
                                                                                                  Beryll::CollisionGroups::RAY_FOR_BUILDING_CHECK,
                                                                                                  Beryll::CollisionGroups::BUILDING);

                        // Check position in free + not closest than 150m + sniper see player(rayBuildingHit does not hit).
                        if(posAndDist.isFreePosition && !rayBuildingHit &&
                           glm::distance(m_player->getObj()->getOrigin(), posAndDist.position) > 150.0f)
                        {
                            posAndDist.isFreePosition = false;

                            enemy->enableEnemy();
                            enemy->disableDraw();
                            enemy->setOrigin(posAndDist.position);
                            enemy->attackDistance = posAndDist.attackDist;
                            //BR_INFO("Spawn sniper. Y = %f", posAndDist.position.y);

                            Sniper::spawnTime = EnAndVars::mapPlayTimeSec;
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
        EnAndVars::bossPhase = true;
        EnAndVars::prepareToBossPhase = false;

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

        const glm::ivec2& spawnPoint2D = pointsToSpawnBoss[Beryll::RandomGenerator::getInt(pointsToSpawnBoss.size() - 1)];
        glm::vec3 spawnPoint3D{spawnPoint2D.x,
                               m_boss->getFromOriginToBottom(),
                               spawnPoint2D.y};
        m_boss->setOrigin(spawnPoint3D);

        m_boss->pathArray = m_pathFinderBoss.findPath(spawnPoint2D, playerClosestAllowedPos, 10);
        if(m_boss->pathArray.size() > 1)
            m_boss->indexInPathArray = 1;
        else
            m_boss->indexInPathArray = 0;

        m_boss->currentPointToMove2DIntegers = m_boss->pathArray[m_boss->indexInPathArray];
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
        m_boss->indexInPathArray = 0;
        m_boss->currentPointToMove2DIntegers = m_boss->pathArray[m_boss->indexInPathArray];
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

                Sounds::playSound(SoundType::GRENADE_LAUNCHER_SHOT);

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
            EnAndVars::mapPlayerWin = true;
    }
}
