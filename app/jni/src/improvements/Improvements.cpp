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
                guiBlock.info.currentLevel = 3;
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

            // Fill m_GUIBlocksToShow.
            if(m_allAvailableGUIBlocks.empty())
            {
                // Prepare default improvement. For example restore 50% players hp.
            }
            else if(m_allAvailableGUIBlocks.size() == 1)
            {
                m_GUIBlocksToShow.push_back(m_allAvailableGUIBlocks[0]);
                m_GUIBlocksToShow.back().button->leftPos = m_leftPos1BlockButton;
                m_GUIBlocksToShow.back().progressText->leftPos = m_leftPos1BlockText;
                m_GUIBlocksToShow.back().progressText->text = std::to_string(m_GUIBlocksToShow.back().info.currentLevel);
                m_GUIBlocksToShow.back().progressText->text += "/";
                m_GUIBlocksToShow.back().progressText->text += std::to_string(m_GUIBlocksToShow.back().info.maxLevel);
            }
            else if(m_allAvailableGUIBlocks.size() == 2)
            {
                for(int i = 0; i < m_allAvailableGUIBlocks.size(); ++i)
                {
                    m_GUIBlocksToShow.push_back(m_allAvailableGUIBlocks[i]);
                    m_GUIBlocksToShow.back().button->leftPos = m_leftPos2BlocksButtons[i];
                    m_GUIBlocksToShow.back().progressText->leftPos = m_leftPos2BlocksTexts[i];
                    m_GUIBlocksToShow.back().progressText->text = std::to_string(m_GUIBlocksToShow.back().info.currentLevel);
                    m_GUIBlocksToShow.back().progressText->text += "/";
                    m_GUIBlocksToShow.back().progressText->text += std::to_string(m_GUIBlocksToShow.back().info.maxLevel);
                }
            }
            else if(m_allAvailableGUIBlocks.size() == 3)
            {
                for(int i = 0; i < m_allAvailableGUIBlocks.size(); ++i)
                {
                    m_GUIBlocksToShow.push_back(m_allAvailableGUIBlocks[i]);
                    m_GUIBlocksToShow.back().button->leftPos = m_leftPos3BlocksButtons[i];
                    m_GUIBlocksToShow.back().progressText->leftPos = m_leftPos3BlocksTexts[i];
                    m_GUIBlocksToShow.back().progressText->text = std::to_string(m_GUIBlocksToShow.back().info.currentLevel);
                    m_GUIBlocksToShow.back().progressText->text += "/";
                    m_GUIBlocksToShow.back().progressText->text += std::to_string(m_GUIBlocksToShow.back().info.maxLevel);
                }
            }
            else
            {
                // Select randomly 3 blocks.

            }
        }

        if(m_GUIBlocksOnScreen)
        {
            for(const auto& block : m_GUIBlocksToShow)
            {
                block.update();

                if(block.button->getIsPressed())
                {
                    if(block.info.type == ImprovementType::PLAYER_SIZE)
                    {
                        BR_INFO("%s", "block PLAYER_SIZE pressed");
                    }
                    else if(block.info.type == ImprovementType::PLAYER_MAX_SPEED)
                    {
                        BR_INFO("%s", "block PLAYER_MAX_SPEED pressed");
                    }

                    m_GUIBlocksOnScreen = false;
                    break;
                }
            }
        }
        else if(!m_GUIBlocksToShow.empty())
        {
            BR_INFO("%s", "m_GUIBlocksToShow.clear()");
            m_GUIBlocksToShow.clear();
        }
    }

    void Improvements::draw()
    {
        if(m_GUIBlocksOnScreen)
        {
            for(const auto& block : m_GUIBlocksToShow)
            {
                block.draw();
            }
        }
    }
}
