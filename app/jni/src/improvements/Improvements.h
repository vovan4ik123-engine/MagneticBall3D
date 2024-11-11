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
        PLAYER_HEAL_AT_NEW_LVL,
        PLAYER_DAMAGE_GROUND_AFTER_FALL,
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
                            std::shared_ptr<Beryll::Text> pt)
                            : info(std::move(inf)), button(std::move(b)), selectedTexture(std::move(st)), progressText(std::move(pt)) {}

        ImprovementInfo info;
        std::shared_ptr<Beryll::ButtonWithTexture> button;
        std::shared_ptr<Beryll::GUITexture> selectedTexture;
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

    struct SelectedImprovement
    {
        float leftPos = 0.0f;
        float topPos = 0.0f;
        float width = 0.0f;
        float height = 0.0f;
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
        std::shared_ptr<Beryll::ButtonWithTexture> buttonReroll;
        int m_rerollAttempts = 1;

        // Piggy bank with available levels. Store player levels. Can be spent for improvements when user wants(button click).
        int m_piggyBankCurrentLevel = 0;
        int m_piggyBankLevelsCollectedCount = 0;
        const int m_piggyBankMaxLevel = m_maxImprovementsSelectedCount * m_maxLevel;
        bool m_piggyBankShow = false;
        float m_piggyBankAnimationStartTime = 0.0f;
        float m_piggyBankAnimationTime = 1.0f;
        bool m_selectImprovement = false;
        std::vector<std::unique_ptr<Beryll::Texture>> m_piggyBankAnimationTextures;
        ImFont* m_piggyBankLevelFont;
        int m_noBackgroundNoFrame = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground |
                                    ImGuiWindowFlags_NoScrollbar;

        // Positions of GUI block in range 0...1.
        // To create blocks.
        float m_leftDefault = 0.0f;
        float m_buttonWidth = 0.1362f;
        float m_buttonTop = 0.25f;
        float m_buttonHeight = 0.537f;
        float m_progressTextTop = 0.6705f;
        float m_progressTextHeight = 0.11f;
        // To position before show on screen. Only left-right pos should be changed.
        float m_leftPos1BlockButton = 0.4319f;
        float m_leftPos1BlockText = 0.4648f;
        glm::vec2 m_leftPos2BlocksButtons{0.3338f, 0.53f};
        glm::vec2 m_leftPos2BlocksTexts{0.3667f, 0.5629f};
        glm::vec3 m_leftPos3BlocksButtons{0.2357f, 0.4319f, 0.6281f};
        glm::vec3 m_leftPos3BlocksTexts{0.2686f, 0.4648f, 0.661f};
    };
}
