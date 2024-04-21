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
            if(inf.type == ImprovementType::PLAYER_SIZE)
            {
                texturePath = "GUI/improvements/IncreaseSize.jpg";
                BR_INFO("%s", "Created GUI block for PLAYER_SIZE.");
            }
            else if(inf.type == ImprovementType::PLAYER_MAX_SPEED)
            {
                texturePath = "GUI/improvements/MaxSpeed.jpg";
                BR_INFO("%s", "Created GUI block for PLAYER_MAX_SPEED.");
            }

            auto b = std::make_shared<Beryll::ButtonWithTexture>(texturePath.c_str(), "",
                                                                 m_leftDefault, m_buttonTop, m_totalWidth, m_buttonHeight);

            auto txt = std::make_shared<Beryll::Text>("XX / XX", EnumsAndVariables::FontsPath::ROBOTO, m_progressHeight,
                                                      m_leftDefault, m_progressTop);
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

        if(m_player->getIsNextLevelAchieved() && !m_allAvailableGUIBlocks.empty())
        {
            m_player->handleLevelAchievement();

            BR_INFO("%s", "improvementSystemOnScreen = true");
            EnumsAndVariables::improvementSystemOnScreen = true;
            Beryll::Physics::disableSimulation();

            // Disable all.
            for(auto& blockDisable : m_allAvailableGUIBlocks)
                blockDisable.onScreen = false;

            // Enable some blocks.
            if(m_allAvailableGUIBlocks.empty())
            {
                BR_INFO("%s", "m_allAvailableGUIBlocks.empty()");
                // Prepare default improvement. For example restore 50% players hp.
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
                    if(block.info.type == ImprovementType::PLAYER_SIZE)
                    {
                        BR_INFO("%s", "block PLAYER_SIZE pressed");
                        m_player->selectNextModel();
                    }
                    else if(block.info.type == ImprovementType::PLAYER_MAX_SPEED)
                    {
                        BR_INFO("%s", "block PLAYER_MAX_SPEED pressed");
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
