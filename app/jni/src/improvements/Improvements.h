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
        PLAYER_PUSH_ENEMIES_BETTER,
        PLAYER_BETTER_CLUTCH_WITH_WALLS,
        PLAYER_MAGNETIZE_MORE_GARBAGE,
        PLAYER_INCREASE_MAX_HP,
        PLAYER_TAKE_LESS_DAMAGE,
        PLAYER_EXP_GAIN,
        PLAYER_DAMAGE_GROUND_AFTER_FALL,
        PLAYER_SHOOT_FASTER,
        GARBAGE_SPAWN_MORE_ON_MAP,
        GARBAGE_TAKE_LESS_DAMAGE,
        JUMPPAD_POWER
    };

    struct ImprovementInfo
    {
        ImprovementInfo(ImprovementType t, int maxLvl, std::vector<std::function<void()>> action)
        : type(t), maxLevel(maxLvl), actions(std::move(action))
        {
            BR_ASSERT((actions.empty() == false), "%s", "actions can not be empty.");
        }

        ImprovementType type;
        int startLevel = 0;
        int currentLevel = 0;
        int maxLevel = 0;
        std::vector<std::function<void()>> actions;
    };

    struct ImprovementGUIBlock
    {
        ImprovementGUIBlock(ImprovementInfo inf,
                            std::shared_ptr<Beryll::ButtonWithTexture> b,
                            std::shared_ptr<Beryll::GUITexture> st,
                            std::shared_ptr<Beryll::GUIText> pt)
                            : info(std::move(inf)), button(std::move(b)), selectedTexture(std::move(st)), progressText(std::move(pt)) {}

        ImprovementInfo info;
        std::shared_ptr<Beryll::ButtonWithTexture> button;
        std::shared_ptr<Beryll::GUITexture> selectedTexture;
        std::shared_ptr<Beryll::GUIText> progressText;

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

    struct SelectedImprovement
    {
        std::shared_ptr<Beryll::GUITexture> texture;
        bool defaultTexture = true;
    };

    class Improvements final
    {
    public:
        Improvements();
        ~Improvements();

        void setPlayer(std::shared_ptr<Player> player) { m_player = std::move(player); }
        void update();
        void draw();

    private:
        void selectImprovementsToShow();

        std::vector<ImprovementGUIBlock> m_allAvailableGUIBlocks;
        std::vector<SelectedImprovement> m_selectedImprovements;
        static constexpr int m_maxImprovementsSelectedCount = 6; // Max unique improvements can be selected. Rest should be unavailable after reach limit.
        static constexpr int m_maxLevel = 5; // Every improvement should has 5 levels.
        std::vector<int> m_IDsSelected; // Contains IDs of selected improvements.
        std::shared_ptr<Player> m_player;
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonReroll;
        int m_rerollAttempts = 1;

        // Piggy bank with available levels. Store player levels. Can be spent for improvements when user wants(button click).
        std::shared_ptr<Beryll::ButtonWithAnimation> m_buttonPiggyBank;
        std::shared_ptr<Beryll::GUIText> m_textPiggyBankLvl;

        int m_piggyBankCurrentLevel = 0;
        int m_piggyBankLevelsCollectedCount = 0;
        const int m_piggyBankMaxLevel = m_maxImprovementsSelectedCount * m_maxLevel;
        bool m_selectImprovement = false;

        // To position before show on screen. Only pos X should be changed.
        float m_leftPos1BlockButton = 43.19f;
        float m_leftPos1BlockText = 45.9f;
        glm::vec2 m_leftPos2BlocksButtons{33.38f, 53.0f};
        glm::vec2 m_leftPos2BlocksTexts{36.09f, 55.71f};
        glm::vec3 m_leftPos3BlocksButtons{23.57f, 43.19f, 62.81f};
        glm::vec3 m_leftPos3BlocksTexts{26.28f, 45.9f, 65.52f};
    };
}
