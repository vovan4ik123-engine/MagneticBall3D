#include "Improvements.h"
#include "DataBaseHelper.h"

namespace MagneticBall3D
{
    Improvements::Improvements()
    {

    }

    Improvements::Improvements(std::shared_ptr<Player> player, const std::vector<ImprovementInfo>& infoVector)
    : m_player(std::move(player))
    {
        for(const ImprovementInfo& info : infoVector)
        {
            // Select only texture path here. Rest is same for all blocks. !!!
            std::string mainTexturePath;
            std::string selectedTexturePath;
            if(info.type == ImprovementType::PLAYER_MAX_SPEED)
            {
                mainTexturePath = "GUI/improvements/PLAYER_MAX_SPEED.jpg";
                selectedTexturePath = "GUI/improvements/PLAYER_MAX_SPEED_SELECTED.jpg";
                BR_INFO("%s", "Created GUI block for PLAYER_MAX_SPEED.");
            }
            else if(info.type == ImprovementType::PLAYER_ACCELERATE_FASTER)
            {
                mainTexturePath = "GUI/improvements/PLAYER_ACCELERATE_FASTER.jpg";
                selectedTexturePath = "GUI/improvements/PLAYER_ACCELERATE_FASTER_SELECTED.jpg";
                BR_INFO("%s", "Created GUI block for PLAYER_ACCELERATE_FASTER.");
            }
            else if(info.type == ImprovementType::PLAYER_MOVE_FASTER_THROUGH_ENEMIES)
            {
                mainTexturePath = "GUI/improvements/PLAYER_MOVE_FASTER_THROUGH_ENEMIES.jpg";
                selectedTexturePath = "GUI/improvements/PLAYER_MOVE_FASTER_THROUGH_ENEMIES_SELECTED.jpg";
                BR_INFO("%s", "Created GUI block for PLAYER_MOVE_FASTER_THROUGH_ENEMIES.");
            }
            else if(info.type == ImprovementType::PLAYER_BETTER_CLUTCH_WITH_BUILDINGS)
            {
                mainTexturePath = "GUI/improvements/PLAYER_BETTER_CLUTCH_WITH_BUILDINGS.jpg";
                selectedTexturePath = "GUI/improvements/PLAYER_BETTER_CLUTCH_WITH_BUILDINGS_SELECTED.jpg";
                BR_INFO("%s", "Created GUI block for PLAYER_BETTER_CLUTCH_WITH_BUILDINGS.");
            }
            else if(info.type == ImprovementType::PLAYER_INCREASE_SIZE)
            {
                mainTexturePath = "GUI/improvements/PLAYER_INCREASE_SIZE.jpg";
                selectedTexturePath = "GUI/improvements/PLAYER_INCREASE_SIZE_SELECTED.jpg";
                BR_INFO("%s", "Created GUI block for PLAYER_INCREASE_SIZE.");
            }
            else if(info.type == ImprovementType::PLAYER_INCREASE_MAX_HP)
            {
                mainTexturePath = "GUI/improvements/PLAYER_INCREASE_MAX_HP.jpg";
                selectedTexturePath = "GUI/improvements/PLAYER_INCREASE_MAX_HP_SELECTED.jpg";
                BR_INFO("%s", "Created GUI block for PLAYER_INCREASE_MAX_HP.");
            }
            else if(info.type == ImprovementType::PLAYER_REDUCE_DAMAGE)
            {
                mainTexturePath = "GUI/improvements/PLAYER_REDUCE_DAMAGE.jpg";
                selectedTexturePath = "GUI/improvements/PLAYER_REDUCE_DAMAGE_SELECTED.jpg";
                BR_INFO("%s", "Created GUI block for PLAYER_REDUCE_DAMAGE.");
            }
            else if(info.type == ImprovementType::PLAYER_TAKE_MORE_XP)
            {
                mainTexturePath = "GUI/improvements/PLAYER_TAKE_MORE_XP.jpg";
                selectedTexturePath = "GUI/improvements/PLAYER_TAKE_MORE_XP_SELECTED.jpg";
                BR_INFO("%s", "Created GUI block for PLAYER_TAKE_MORE_XP.");
            }
            else if(info.type == ImprovementType::PLAYER_HEAL_AT_NEW_LVL)
            {
                mainTexturePath = "GUI/improvements/PLAYER_HEAL_AT_NEW_LVL.jpg";
                selectedTexturePath = "GUI/improvements/PLAYER_HEAL_AT_NEW_LVL_SELECTED.jpg";
                BR_INFO("%s", "Created GUI block for PLAYER_HEAL_AT_NEW_LVL.");
            }
            else if(info.type == ImprovementType::GARBAGE_SPAWN_MORE_ON_MAP)
            {
                mainTexturePath = "GUI/improvements/GARBAGE_SPAWN_MORE_ON_MAP.jpg";
                selectedTexturePath = "GUI/improvements/GARBAGE_SPAWN_MORE_ON_MAP_SELECTED.jpg";
                BR_INFO("%s", "Created GUI block for GARBAGE_SPAWN_MORE_ON_MAP.");
            }
            else if(info.type == ImprovementType::GARBAGE_REDUCE_DAMAGE)
            {
                mainTexturePath = "GUI/improvements/GARBAGE_REDUCE_DAMAGE.jpg";
                selectedTexturePath = "GUI/improvements/GARBAGE_REDUCE_DAMAGE_SELECTED.jpg";
                BR_INFO("%s", "Created GUI block for GARBAGE_REDUCE_DAMAGE.");
            }

            auto b = std::make_shared<Beryll::ButtonWithTexture>(mainTexturePath.c_str(), "",
                                                                 m_leftDefault, m_buttonTop, m_buttonWidth, m_buttonHeight);

            auto st = std::make_shared<Beryll::GUITexture>(selectedTexturePath.c_str(), 0.1f, 0.1f, 0.1f, 0.1f);

            auto txt = std::make_shared<Beryll::Text>("00/00", EnAndVars::FontsPath::ROBOTO, m_progressHeight,
                                                      m_leftDefault, m_progressTop, 0.2f, 0.055f, false, true);
            txt->setFontColor(0.62f, 0.0f, 0.77f, 1.0f);

            ImprovementGUIBlock guiBlock(info, b, st, txt);
            m_allAvailableGUIBlocks.push_back(guiBlock);
        }

        const float screenAR = Beryll::MainImGUI::getInstance()->getGUIScreenAspectRation();

        for(int i = 0; i < m_maxImprovementsSelectedCount; ++i)
        {
            SelectedImprovement selected;
            selected.leftPos = 0.022f + (i * 0.12f);
            selected.topPos = 0.25f;
            selected.width = 0.115f;
            selected.height = selected.width / screenAR;
            selected.texture = std::make_shared<Beryll::GUITexture>("GUI/improvements/SELECTED_DEFAULT.jpg",
                                                                    selected.leftPos, selected.topPos,
                                                                    selected.width, selected.height);

            m_selectedImprovements.push_back(selected);
        }

        buttonReroll= std::make_shared<Beryll::ButtonWithTexture>("GUI/improvements/Reroll.jpg", "", 0.35f, 0.65f, 0.3f, 0.05f);
        buttonReroll->disable();
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

            if(m_allAvailableGUIBlocks.empty())
                return;

            selectImprovementsToShow();

            if(m_rerollAttempts > 0)
                buttonReroll->enable();

            BR_INFO("%s", "improvementSystemOnScreen = true");
            EnAndVars::improvementSystemOnScreen = true;
            Beryll::Physics::disableSimulation();
            m_timeAppearsOnScreen = Beryll::TimeStep::getSecFromStart();
        }

        if(EnAndVars::improvementSystemOnScreen && m_timeAppearsOnScreen + m_delayBeforeCanBeClicked < Beryll::TimeStep::getSecFromStart())
        {
            if(buttonReroll->getIsEnabled())
            {
                buttonReroll->updateBeforePhysics();

                if(EnAndVars::CurrencyBalance::crystals >= m_rerollPrice && buttonReroll->getIsPressed())
                {
                    --m_rerollAttempts;
                    EnAndVars::CurrencyBalance::crystals -= m_rerollPrice;
                    DataBaseHelper::storeCurrencyBalanceCrystals(EnAndVars::CurrencyBalance::crystals);

                    selectImprovementsToShow();

                    if(m_rerollAttempts <= 0)
                        buttonReroll->disable();
                }
            }

            int idToRemove = -1;

            for(auto& block : m_allAvailableGUIBlocks)
            {
                if(!block.onScreen)
                    continue;

                block.update();

                if(block.button->getIsPressed())
                {
                    BR_INFO("%s", "improvementSystemOnScreen = false");
                    EnAndVars::improvementSystemOnScreen = false;
                    Beryll::Physics::enableSimulation();

                    // Disable all.
                    for(auto& blockDisable : m_allAvailableGUIBlocks)
                        blockDisable.onScreen = false;

                    buttonReroll->disable();

                    // Handle click.
                    if(block.info.currentLevel < block.info.actions.size())
                        block.info.actions[block.info.currentLevel](); // Call lambda stored in vector.
                    else
                        block.info.actions.back()(); // Call lambda stored in vector.

                    ++block.info.currentLevel;

                    if(block.info.currentLevel >= block.info.maxLevel)
                        idToRemove = block.getID();

                    const auto iter = std::find_if(m_IDsSelected.begin(), m_IDsSelected.end(), [&block](int id) { return block.getID() == id; });
                    if(iter == m_IDsSelected.end())
                    {
                        BR_INFO("%s", "Push new unique improvement ID.");
                        m_IDsSelected.push_back(block.getID());

                        for(auto& selectedImpr : m_selectedImprovements)
                        {
                            if(selectedImpr.defaultTexture)
                            {
                                selectedImpr.defaultTexture = false;

                                selectedImpr.texture = block.selectedTexture;
                                selectedImpr.texture->leftPos = selectedImpr.leftPos;
                                selectedImpr.texture->topPos = selectedImpr.topPos;
                                selectedImpr.texture->width = selectedImpr.width;
                                selectedImpr.texture->height = selectedImpr.height;

                                break;
                            }
                        }
                    }

                    break;
                }
            }

            if(idToRemove != -1)
            {
                const auto iter = std::find_if(m_allAvailableGUIBlocks.begin(), m_allAvailableGUIBlocks.end(),
                                              [idToRemove](const ImprovementGUIBlock& b) { return b.getID() == idToRemove; });

                if(iter != m_allAvailableGUIBlocks.end())
                {
                    BR_INFO("%s", "Remove from m_allAvailableGUIBlocks.");
                    m_allAvailableGUIBlocks.erase(iter);
                }
            }

            if(m_IDsSelected.size() == m_maxImprovementsSelectedCount)
            {
                //BR_INFO("%s", "m_IDsSelected.size() == m_maxImprovementsSelectedCount.");
                // Remove all GUI blocks from m_allAvailableGUIBlocks if theirs id is not in m_IDsSelected.
                std::vector<int> IDsToRemove;

                for(const auto& block : m_allAvailableGUIBlocks)
                {
                    const auto iter = std::find_if(m_IDsSelected.begin(), m_IDsSelected.end(), [&block](const int id) { return block.getID() == id; });
                    if(iter == m_IDsSelected.end())
                    {
                        IDsToRemove.push_back(block.getID());
                    }
                }

                //BR_INFO("IDsToRemove.size(): %d", IDsToRemove.size());

                for(const int IDToRemove : IDsToRemove)
                {
                    const auto iter = std::find_if(m_allAvailableGUIBlocks.begin(), m_allAvailableGUIBlocks.end(),
                                             [IDToRemove](const ImprovementGUIBlock& b) { return b.getID() == IDToRemove; });

                    if(iter != m_allAvailableGUIBlocks.end())
                    {
                        BR_INFO("%s", "Remove from m_allAvailableGUIBlocks because > than m_maxImprovementsSelectedCount.");
                        m_allAvailableGUIBlocks.erase(iter);
                    }
                }
            }
        }
    }

    void Improvements::draw()
    {
        if(EnAndVars::improvementSystemOnScreen)
        {
            for(const auto& block : m_allAvailableGUIBlocks)
            {
                if(block.onScreen)
                    block.draw();
            }

            for(const auto& selected : m_selectedImprovements)
            {
                selected.texture->draw();
            }

            if(buttonReroll->getIsEnabled())
                buttonReroll->draw();
        }
    }

    void Improvements::selectImprovementsToShow()
    {
        // Disable all.
        for(auto& blockDisable : m_allAvailableGUIBlocks)
            blockDisable.onScreen = false;

        // Enable some blocks.
        if(m_allAvailableGUIBlocks.empty())
        {
            BR_INFO("%s", "m_allAvailableGUIBlocks.empty(). return;");
            EnAndVars::improvementSystemOnScreen = false;
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
            std::vector<int> randomIndexes; // Select random 3 blocks.

            while(randomIndexes.size() < 3)
            {
                int randomIndex = Beryll::RandomGenerator::getInt(m_allAvailableGUIBlocks.size() - 1);

                if(std::find(randomIndexes.begin(), randomIndexes.end(), randomIndex) == randomIndexes.end())
                {
                    randomIndexes.push_back(randomIndex);
                    BR_INFO("Random index to show: %d", randomIndex);
                }
            }

            int indexOnScreen = 0;
            for(int i = 0; i < m_allAvailableGUIBlocks.size(); ++i)
            {
                if(std::find(randomIndexes.begin(), randomIndexes.end(), i) != randomIndexes.end())
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
    }
}
