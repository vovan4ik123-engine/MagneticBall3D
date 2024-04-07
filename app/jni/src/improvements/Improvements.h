#pragma once

#include "EngineHeaders.h"
#include "EnumsAndVariables.h"
#include "player/Player.h"

namespace MagneticBall3D
{
    enum class ImprovementType
    {
        NONE,
        PLAYER_SIZE,
        PLAYER_MAX_SPEED
    };

    struct ImprovementInfo
    {
        ImprovementInfo(ImprovementType t, int maxLvl)
        : type(t), maxLevel(maxLvl) {}

        ImprovementType type;
        int startLevel = 0;
        int currentLevel = 0;
        int maxLevel = 0;
    };

    struct ImprovementGUIBlock
    {
        ImprovementGUIBlock(ImprovementInfo inf, std::shared_ptr<Beryll::ButtonWithTexture>b, std::shared_ptr<Beryll::ProgressBar> pb)
        : info(inf), button(std::move(b)), progressBar(std::move(pb)) {}

        ImprovementInfo info;
        std::shared_ptr<Beryll::ButtonWithTexture> button;
        std::shared_ptr<Beryll::ProgressBar> progressBar;

        void update() const
        {
            button->updateBeforePhysics();
            progressBar->updateBeforePhysics();
        }

        void draw() const
        {
            button->draw();
            progressBar->draw();
        }
    };

    // Should have 2 GUI elements for every improvement.
    // 1. Button with image and description of improvements.
    // 2. Progress bar of this improvements.
    //
    class Improvements final
    {
    public:
        Improvements();
        Improvements(std::shared_ptr<Player> player, const std::vector<ImprovementInfo>& info);
        ~Improvements();

        void update();
        void draw();

        bool getIsGUIBlocksOnScreen() { return m_GUIBlocksOnScreen; }

    private:
        std::vector<ImprovementGUIBlock> m_allAvailableGUIBlocks;
        std::vector<ImprovementGUIBlock> m_GUIBlocksToShow;
        std::shared_ptr<Player> m_player;
        bool m_GUIBlocksOnScreen = false;
    };
}
