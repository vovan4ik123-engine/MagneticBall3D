#pragma once

#include "EngineHeaders.h"
#include "EnumsAndVariables.h"
#include "player/Player.h"

namespace MagneticBall3D
{
    enum class ImprovementType
    {
        NONE,
        PLAYER_MAX_SPEED,
        PLAYER_ACCELERATE_FASTER,
        PLAYER_MOVE_FASTER_THROUGH_ENEMIES,
        PLAYER_BETTER_CLUTCH_WITH_BUILDINGS,
        PLAYER_INCREASE_SIZE,
        PLAYER_INCREASE_MAX_HP,
        PLAYER_REDUCE_DAMAGE,
        PLAYER_TAKE_MORE_XP,
        PLAYER_HEAL_AT_NEW_LVL,
        GARBAGE_SPAWN_MORE_ON_MAP,
        GARBAGE_REDUCE_DAMAGE
    };

    struct ImprovementInfo
    {
        ImprovementInfo(ImprovementType t, int maxLvl, std::vector<std::function<void()>> action)
        : type(t), maxLevel(maxLvl), actions(std::move(action))
        {
            BR_ASSERT((actions.empty() == false), "%s", "actions can not be empty.")
        }

        ImprovementType type;
        int startLevel = 0;
        int currentLevel = 0;
        int maxLevel = 0;
        std::vector<std::function<void()>> actions;
    };

    struct ImprovementGUIBlock
    {
        ImprovementGUIBlock(ImprovementInfo inf, std::shared_ptr<Beryll::ButtonWithTexture>b, std::shared_ptr<Beryll::Text> pt)
        : info(inf), button(std::move(b)), progressText(std::move(pt)) {}

        ImprovementInfo info;
        std::shared_ptr<Beryll::ButtonWithTexture> button;
        std::shared_ptr<Beryll::Text> progressText;

        bool onScreen = false;

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

        int getID() const { return m_id; }

    private:
        int m_id = BeryllUtils::Common::generateID();
    };

    class Improvements final
    {
    public:
        Improvements();
        Improvements(std::shared_ptr<Player> player, const std::vector<ImprovementInfo>& info);
        ~Improvements();

        void update();
        void draw();

    private:
        std::vector<ImprovementGUIBlock> m_allAvailableGUIBlocks;
        std::shared_ptr<Player> m_player;

        // Positions of GUI block in % of screen. Range 0.0f...100.0f.
        // To create blocks.
        float m_leftDefault = 0.0f;
        float m_buttonWidth = 30.0f;
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

        float m_timeAppearsOnScreen = 0.0f;
        // Because user can accidentally click when he actively swipe and buttons appears on screen.
        float m_delayBeforeCanBeClicked = 1.1f;
    };
}
