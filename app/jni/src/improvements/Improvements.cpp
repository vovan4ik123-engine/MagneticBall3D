#include "Improvements.h"

namespace MagneticBall3D
{
    Improvements::Improvements()
    {

    }

    Improvements::Improvements(std::shared_ptr<Player> player, const std::vector<ImprovementInfo>& info)
    : m_player(std::move(player))
    {
        for(const ImprovementInfo& inf : info)
        {
            // Select only texture path here. Rest is same for all blocks. !!!
            std::string texturePath;
            if(inf.type == ImprovementType::PLAYER_MAX_SPEED)
            {
                texturePath = "GUI/improvements/PLAYER_MAX_SPEED.jpg";
                BR_INFO("%s", "Created GUI block for PLAYER_MAX_SPEED.");
            }
            else if(inf.type == ImprovementType::PLAYER_ACCELERATE_FASTER)
            {
                texturePath = "GUI/improvements/PLAYER_ACCELERATE_FASTER.jpg";
                BR_INFO("%s", "Created GUI block for PLAYER_ACCELERATE_FASTER.");
            }
            else if(inf.type == ImprovementType::PLAYER_MOVE_FASTER_THROUGH_ENEMIES)
            {
                texturePath = "GUI/improvements/PLAYER_MOVE_FASTER_THROUGH_ENEMIES.jpg";
                BR_INFO("%s", "Created GUI block for PLAYER_MOVE_FASTER_THROUGH_ENEMIES.");
            }
            else if(inf.type == ImprovementType::PLAYER_BETTER_CLUTCH_WITH_BUILDINGS)
            {
                texturePath = "GUI/improvements/PLAYER_BETTER_CLUTCH_WITH_BUILDINGS.jpg";
                BR_INFO("%s", "Created GUI block for PLAYER_BETTER_CLUTCH_WITH_BUILDINGS.");
            }
            else if(inf.type == ImprovementType::PLAYER_INCREASE_SIZE)
            {
                texturePath = "GUI/improvements/PLAYER_INCREASE_SIZE.jpg";
                BR_INFO("%s", "Created GUI block for PLAYER_INCREASE_SIZE.");
            }
            else if(inf.type == ImprovementType::PLAYER_INCREASE_MAX_HP)
            {
                texturePath = "GUI/improvements/PLAYER_INCREASE_MAX_HP.jpg";
                BR_INFO("%s", "Created GUI block for PLAYER_INCREASE_MAX_HP.");
            }
            else if(inf.type == ImprovementType::PLAYER_REDUCE_DAMAGE)
            {
                texturePath = "GUI/improvements/PLAYER_REDUCE_DAMAGE.jpg";
                BR_INFO("%s", "Created GUI block for PLAYER_REDUCE_DAMAGE.");
            }
            else if(inf.type == ImprovementType::PLAYER_TAKE_MORE_XP)
            {
                texturePath = "GUI/improvements/PLAYER_TAKE_MORE_XP.jpg";
                BR_INFO("%s", "Created GUI block for PLAYER_TAKE_MORE_XP.");
            }
            else if(inf.type == ImprovementType::PLAYER_HEAL_AT_NEW_LVL)
            {
                texturePath = "GUI/improvements/PLAYER_HEAL_AT_NEW_LVL.jpg";
                BR_INFO("%s", "Created GUI block for PLAYER_HEAL_AT_NEW_LVL.");
            }
            else if(inf.type == ImprovementType::GARBAGE_SPAWN_MORE_ON_MAP)
            {
                texturePath = "GUI/improvements/GARBAGE_SPAWN_MORE_ON_MAP.jpg";
                BR_INFO("%s", "Created GUI block for GARBAGE_SPAWN_MORE_ON_MAP.");
            }
            else if(inf.type == ImprovementType::GARBAGE_REDUCE_DAMAGE)
            {
                texturePath = "GUI/improvements/GARBAGE_REDUCE_DAMAGE.jpg";
                BR_INFO("%s", "Created GUI block for GARBAGE_REDUCE_DAMAGE.");
            }

            auto b = std::make_shared<Beryll::ButtonWithTexture>(texturePath.c_str(), "",
                                                                 m_leftDefault, m_buttonTop, m_buttonWidth, m_buttonHeight);

            auto txt = std::make_shared<Beryll::Text>("XX / XX", EnumsAndVariables::FontsPath::ROBOTO, m_progressHeight,
                                                      m_leftDefault, m_progressTop, 20, 5.5f);
            txt->setFontColor(0.62f, 0.0f, 0.77f, 1.0f);

            ImprovementGUIBlock guiBlock(inf, b, txt);
            m_allAvailableGUIBlocks.push_back(guiBlock);
        }
    }

    Improvements::~Improvements()
    {

    }

    void Improvements::update()
    {
        BR_ASSERT((m_player != nullptr), "%s", "m_player is nullptr.");

        if(m_player->getIsNextLevelAchieved())
        {
            m_player->handleLevelAchievement();

            // Disable all.
            for(auto& blockDisable : m_allAvailableGUIBlocks)
                blockDisable.onScreen = false;

            // Enable some blocks.
            if(m_allAvailableGUIBlocks.empty())
            {
                BR_INFO("%s", "m_allAvailableGUIBlocks.empty(). return;");
                EnumsAndVariables::improvementSystemOnScreen = false;
                return;
            }
            else if(m_allAvailableGUIBlocks.size() == 1)
            {
                m_allAvailableGUIBlocks[0].button->leftPos = m_leftPos1BlockButton;
                m_allAvailableGUIBlocks[0].progressText->leftPos = m_leftPos1BlockText;
                m_allAvailableGUIBlocks[0].progressText->text = std::to_string(m_allAvailableGUIBlocks[0].info.currentLevel);
                m_allAvailableGUIBlocks[0].progressText->text += "/";
                m_allAvailableGUIBlocks[0].progressText->text += std::to_string(m_allAvailableGUIBlocks[0].info.maxLevel);
                m_allAvailableGUIBlocks[0].onScreen = true;
            }
            else if(m_allAvailableGUIBlocks.size() == 2)
            {
                for(int i = 0; i < m_allAvailableGUIBlocks.size(); ++i)
                {
                    m_allAvailableGUIBlocks[i].button->leftPos = m_leftPos2BlocksButtons[i];
                    m_allAvailableGUIBlocks[i].progressText->leftPos = m_leftPos2BlocksTexts[i];
                    m_allAvailableGUIBlocks[i].progressText->text = std::to_string(m_allAvailableGUIBlocks[i].info.currentLevel);
                    m_allAvailableGUIBlocks[i].progressText->text += "/";
                    m_allAvailableGUIBlocks[i].progressText->text += std::to_string(m_allAvailableGUIBlocks[i].info.maxLevel);
                    m_allAvailableGUIBlocks[i].onScreen = true;
                }
            }
            else if(m_allAvailableGUIBlocks.size() == 3)
            {
                for(int i = 0; i < m_allAvailableGUIBlocks.size(); ++i)
                {
                    m_allAvailableGUIBlocks[i].button->leftPos = m_leftPos3BlocksButtons[i];
                    m_allAvailableGUIBlocks[i].progressText->leftPos = m_leftPos3BlocksTexts[i];
                    m_allAvailableGUIBlocks[i].progressText->text = std::to_string(m_allAvailableGUIBlocks[i].info.currentLevel);
                    m_allAvailableGUIBlocks[i].progressText->text += "/";
                    m_allAvailableGUIBlocks[i].progressText->text += std::to_string(m_allAvailableGUIBlocks[i].info.maxLevel);
                    m_allAvailableGUIBlocks[i].onScreen = true;
                }
            }
            else
            {
                // Exclude randomly all except 3 blocks.
                const int indexesCountToExclude = m_allAvailableGUIBlocks.size() - 3;
                std::vector<int> indexesToExclude;

                while(indexesToExclude.size() < indexesCountToExclude)
                {
                    int randomIndex = Beryll::RandomGenerator::getInt(m_allAvailableGUIBlocks.size() - 1);

                    if(std::find(indexesToExclude.begin(), indexesToExclude.end(), randomIndex) == indexesToExclude.end())
                    {
                        indexesToExclude.push_back(randomIndex);
                        BR_INFO("index to exclude %d", randomIndex);
                    }
                }

                int indexOnScreen = 0;
                for(int i = 0; i < m_allAvailableGUIBlocks.size(); ++i)
                {
                    if(std::find(indexesToExclude.begin(), indexesToExclude.end(), i) == indexesToExclude.end())
                    {
                        // Show block.
                        m_allAvailableGUIBlocks[i].button->leftPos = m_leftPos3BlocksButtons[indexOnScreen];
                        m_allAvailableGUIBlocks[i].progressText->leftPos = m_leftPos3BlocksTexts[indexOnScreen];
                        m_allAvailableGUIBlocks[i].progressText->text = std::to_string(m_allAvailableGUIBlocks[i].info.currentLevel);
                        m_allAvailableGUIBlocks[i].progressText->text += "/";
                        m_allAvailableGUIBlocks[i].progressText->text += std::to_string(m_allAvailableGUIBlocks[i].info.maxLevel);
                        m_allAvailableGUIBlocks[i].onScreen = true;

                        ++indexOnScreen;
                    }
                }
            }

            BR_INFO("%s", "improvementSystemOnScreen = true");
            EnumsAndVariables::improvementSystemOnScreen = true;
            Beryll::Physics::disableSimulation();
        }

        if(EnumsAndVariables::improvementSystemOnScreen)
        {
            int idToRemove = -1;

            for(auto& block : m_allAvailableGUIBlocks)
            {
                if(!block.onScreen)
                    continue;

                block.update();

                if(block.button->getIsPressed())
                {
                    ++block.info.currentLevel;

                    if(block.info.currentLevel >= block.info.maxLevel)
                        idToRemove = block.getID();

                    BR_INFO("%s", "improvementSystemOnScreen = false");
                    EnumsAndVariables::improvementSystemOnScreen = false;
                    Beryll::Physics::enableSimulation();

                    // Disable all.
                    for(auto& blockDisable : m_allAvailableGUIBlocks)
                        blockDisable.onScreen = false;

                    // Handle click.
                    if(block.info.type == ImprovementType::PLAYER_MAX_SPEED)
                    {
                        EnumsAndVariables::playerMaxSpeedXZ += 10.0f;
                        BR_INFO("%s", "Block PLAYER_MAX_SPEED pressed.");
                    }
                    else if(block.info.type == ImprovementType::PLAYER_ACCELERATE_FASTER)
                    {
                        EnumsAndVariables::playerImpulseFactorOnGround += 0.01f;
                        EnumsAndVariables::playerTorqueFactorOnGround += 0.01f;
                        EnumsAndVariables::playerImpulseFactorOnBuildingRoof += 0.009f;
                        EnumsAndVariables::playerTorqueFactorOnBuildingRoof += 0.009f;
                        BR_INFO("%s", "Block PLAYER_ACCELERATE_FASTER pressed.");
                    }
                    else if(block.info.type == ImprovementType::PLAYER_MOVE_FASTER_THROUGH_ENEMIES)
                    {
                        EnumsAndVariables::playerSpeedReductionMultiplier -= 0.1f;
                        if(EnumsAndVariables::playerSpeedReductionMultiplier < 0.0f)
                            EnumsAndVariables::playerSpeedReductionMultiplier = 0.0f;
                        BR_INFO("%s", "Block PLAYER_MOVE_FASTER_THROUGH_ENEMIES pressed.");
                    }
                    else if(block.info.type == ImprovementType::PLAYER_BETTER_CLUTCH_WITH_BUILDINGS)
                    {
                        EnumsAndVariables::playerTorqueFactorOnBuildingWall += 0.023f;
                        BR_INFO("%s", "Block PLAYER_BETTER_CLUTCH_WITH_BUILDINGS pressed.");
                    }
                    else if(block.info.type == ImprovementType::PLAYER_INCREASE_SIZE)
                    {
                        EnumsAndVariables::playerTorqueFactorOnBuildingWall += 0.005f;
                        EnumsAndVariables::playerRadiusToKillEnemies += 3.5f;
                        EnumsAndVariables::playerMagneticRadius += 4.5f;
                        EnumsAndVariables::garbageMaxCountMagnetized += 35;
                        m_player->selectNextModel();
                        BR_INFO("%s", "Block PLAYER_INCREASE_SIZE pressed.");
                    }
                    else if(block.info.type == ImprovementType::PLAYER_INCREASE_MAX_HP)
                    {
                        m_player->addToMaxHP(50.0f);
                        BR_INFO("%s", "Block PLAYER_INCREASE_MAX_HP pressed.");
                    }
                    else if(block.info.type == ImprovementType::PLAYER_REDUCE_DAMAGE)
                    {
                        EnumsAndVariables::playerDamageTakenMultiplier -= 0.1f;
                        if(EnumsAndVariables::playerDamageTakenMultiplier < 0.0f)
                            EnumsAndVariables::playerDamageTakenMultiplier = 0.0f;
                        BR_INFO("%s", "Block PLAYER_REDUCE_DAMAGE pressed.");
                    }
                    else if(block.info.type == ImprovementType::PLAYER_TAKE_MORE_XP)
                    {
                        EnumsAndVariables::playerXPMultiplier += 0.05f;
                        BR_INFO("%s", "Block PLAYER_TAKE_MORE_XP pressed.");
                    }
                    else if(block.info.type == ImprovementType::PLAYER_HEAL_AT_NEW_LVL)
                    {
                        EnumsAndVariables::playerRestoreHPAtNewLevel += m_player->getMaxHP() * 0.04f;
                        BR_INFO("%s", "Block PLAYER_HEAL_AT_NEW_LVL pressed.");
                    }
                    else if(block.info.type == ImprovementType::GARBAGE_SPAWN_MORE_ON_MAP)
                    {
                        EnumsAndVariables::garbageCommonSpawnDelay -= 0.1f;
                        if(EnumsAndVariables::garbageCommonSpawnDelay < 0.0f)
                            EnumsAndVariables::garbageCommonSpawnDelay = 0.0f;
                        BR_INFO("%s", "Block GARBAGE_SPAWN_MORE_ON_MAP pressed.");
                    }
                    else if(block.info.type == ImprovementType::GARBAGE_REDUCE_DAMAGE)
                    {
                        EnumsAndVariables::garbageDamageTakenMultiplier -= 0.1f;
                        if(EnumsAndVariables::garbageDamageTakenMultiplier < 0.0f)
                            EnumsAndVariables::garbageDamageTakenMultiplier = 0.0f;
                        BR_INFO("%s", "Block GARBAGE_REDUCE_DAMAGE pressed.");
                    }

                    break;
                }
            }

            if(idToRemove != -1)
            {
                auto iter = std::find_if(m_allAvailableGUIBlocks.begin(), m_allAvailableGUIBlocks.end(),
                                              [idToRemove](const ImprovementGUIBlock& b) { return b.getID() == idToRemove; });

                if(iter != m_allAvailableGUIBlocks.end())
                {
                    BR_INFO("%s", "Remove from m_allAvailableGUIBlocks");
                    m_allAvailableGUIBlocks.erase(iter);
                }
            }
        }
    }

    void Improvements::draw()
    {
        if(EnumsAndVariables::improvementSystemOnScreen)
        {
            for(const auto& block : m_allAvailableGUIBlocks)
            {
                if(block.onScreen)
                    block.draw();
            }
        }
    }
}
