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
                auto b = std::make_shared<Beryll::ButtonWithTexture>("GUI/improvements/increaseSize.jpg", "", 0, 35, 30, 30);

                auto pr = std::make_shared<Beryll::ProgressBar>( 0, 65, 30, 3);
                pr->setFontColor(0.0f, 0.0f, 0.0f, 0.0f);
                pr->setProgressColor(0.3f, 0.0f, 0.51f, 1.0f);
                pr->setBackgroundColor(0.6f, 0.6f, 0.6f, 1.0f);
                pr->setProgress(0.4f);

                ImprovementGUIBlock guiBlock(inf, b, pr);
                m_allAvailableGUIBlocks.push_back(guiBlock);

                BR_INFO("%s", "Created GUI block for PLAYER_SIZE.");
            }
            else if(inf.type == ImprovementType::PLAYER_MAX_SPEED)
            {

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
            m_GUIBlocksToShow.push_back(m_allAvailableGUIBlocks.back());
        }

        if(m_GUIBlocksOnScreen)
        {
            for(const auto& block : m_GUIBlocksToShow)
            {
                block.update();

                if(block.button->getIsPressed())
                {
                    BR_INFO("%s", "block.button pressed");
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
