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
        ImprovementGUIBlock(ImprovementInfo inf, std::shared_ptr<Beryll::ButtonWithTexture>b, std::shared_ptr<Beryll::Text> pt)
        : info(inf), button(std::move(b)), progressText(std::move(pt)) {}

        ImprovementInfo info;
        std::shared_ptr<Beryll::ButtonWithTexture> button;
        std::shared_ptr<Beryll::Text> progressText;

        void update() const
        {
            button->updateBeforePhysics();
            progressText->updateBeforePhysics();
        }

        void draw() const
        {
            button->draw();
            progressText->draw();
        }
    };

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

        // Positions of GUI block in % of screen. Range 0.0f...100.0f.
        // To create blocks.
        float m_leftDefault = 0.0f;
        float m_totalWidth = 30.0f;
        float m_buttonTop = 35.0f;
        float m_buttonHeight = 25.0f;
        float m_progressTop = 55.0f;
        float m_progressHeight = 5.0f;
        // To position before show on screen. Only left-right pos should be changed. Range 0.0f...1.0f.
        float m_leftPos1BlockButton = 0.35f;
        float m_leftPos1BlockText = 0.42f;
        glm::vec2 m_leftPos2BlocksButtons{0.15f, 0.55f};
        glm::vec2 m_leftPos2BlocksTexts{0.22f, 0.62f};
        glm::vec3 m_leftPos3BlocksButtons{0.02f, 0.35f, 0.68f};
        glm::vec3 m_leftPos3BlocksTexts{0.09f, 0.42f, 0.75f};
    };
}
