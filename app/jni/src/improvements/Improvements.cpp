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
            if(inf.type == ImprovementType::PLAYER_SIZE)
            {
                auto b = std::make_shared<Beryll::ButtonWithTexture>("GUI/improvements/IncreaseSize.jpg", "",
                                                                               m_leftDefault, m_buttonTop, m_totalWidth, m_buttonHeight);

                auto prt = std::make_shared<Beryll::Text>("XX / XX", EnumsAndVariables::FontsPath::ROBOTO, m_progressHeight,
                                                                      m_leftDefault, m_progressTop);
                prt->setFontColor(0.62f, 0.0f, 0.77f, 1.0f);

                ImprovementGUIBlock guiBlock(inf, b, prt);
                m_allAvailableGUIBlocks.push_back(guiBlock);

                BR_INFO("%s", "Created GUI block for PLAYER_SIZE.");
            }
            else if(inf.type == ImprovementType::PLAYER_MAX_SPEED)
            {
                auto b = std::make_shared<Beryll::ButtonWithTexture>("GUI/improvements/MaxSpeed.jpg", "",
                                                                     m_leftDefault, m_buttonTop, m_totalWidth, m_buttonHeight);

                auto prt = std::make_shared<Beryll::Text>("XX / XX", EnumsAndVariables::FontsPath::ROBOTO, m_progressHeight,
                                                          m_leftDefault, m_progressTop);
                prt->setFontColor(0.62f, 0.0f, 0.77f, 1.0f);

                ImprovementGUIBlock guiBlock(inf, b, prt);
                m_allAvailableGUIBlocks.push_back(guiBlock);

                BR_INFO("%s", "Created GUI block for PLAYER_MAX_SPEED.");
            }

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

            m_GUIBlocksOnScreen = true;

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
                // Select randomly 3 blocks.

            }
        }

        if(m_GUIBlocksOnScreen)
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

                    m_GUIBlocksOnScreen = false;
                    // Disable all.
                    for(auto& blockDisable : m_allAvailableGUIBlocks)
                        blockDisable.onScreen = false;

                    // Handle click.
                    if(block.info.type == ImprovementType::PLAYER_SIZE)
                    {
                        BR_INFO("%s", "block PLAYER_SIZE pressed");
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
        if(m_GUIBlocksOnScreen)
        {
            for(const auto& block : m_allAvailableGUIBlocks)
            {
                if(block.onScreen)
                    block.draw();
            }
        }
    }
}
