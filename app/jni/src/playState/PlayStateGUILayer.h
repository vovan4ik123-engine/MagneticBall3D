#pragma once

#include "EngineHeaders.h"

namespace MagneticBall3D
{
    class PlayStateGUILayer : public Beryll::Layer
    {
    public:
        PlayStateGUILayer();
        ~PlayStateGUILayer() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

        void showMenuResurrect();
        void showMenuLose();
        void showMenuWin();

        // Menus before specific bosses.
        void showMenuBossTankWithCommander();

        std::shared_ptr<Beryll::SliderHorizontal> slider1;
        std::shared_ptr<Beryll::SliderHorizontal> slider2;
        std::shared_ptr<Beryll::SliderHorizontal> slider3;

        std::shared_ptr<Beryll::Joystick> playerJoystick;
        std::shared_ptr<Beryll::ButtonWithTexture> m_shotButton;

        float progressBarHP = 1.0f;
        float progressBarXP = 0.0f;

        bool resurrectPlayer = false;
        bool tutorialMoveShow = false;
        bool tutorialTextTipsShow = false;

    private:
        std::vector<std::shared_ptr<Beryll::GUIObject>> m_guiObjects;

#if defined(BR_DEBUG)
        bool m_statisticsShow = true;
#else
        bool m_statisticsShow = false;
#endif
        std::shared_ptr<Beryll::Text> m_statistics1;
        std::shared_ptr<Beryll::Text> m_statistics2;
        std::shared_ptr<Beryll::Text> m_statistics3;
        uint64_t m_statisticsUpdateTime = 0;

        // GUI based on raw ImGUI.
        // ImGUI flags.
        int m_noBackgroundNoFrame = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground |
                                    ImGuiWindowFlags_NoScrollbar;

        int m_noFrame = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar;

        // Common textures.
        std::unique_ptr<Beryll::Texture> m_okTexture;
        std::unique_ptr<Beryll::Texture> m_exitTexture;

        // Play timer.
        std::string m_mapPlayTimerText;

        // Counters.
        std::unique_ptr<Beryll::Texture> m_countersTexture;
        ImFont* m_countersFont;

        // Pause button.
        std::unique_ptr<Beryll::Texture> m_pauseButtonTexture;
        bool m_pauseButtonClicked = false;
        // Pause menu.
        std::unique_ptr<Beryll::Texture> m_resumeButtonTexture;
        bool m_pauseMenuShow = false;
        bool m_resumeButtonClicked = false;

        // Exit.
        bool m_exitButtonClicked = false;

        // Map0Tutorial.
        std::unique_ptr<Beryll::Texture> m_tutorialMoveTexture;
        std::unique_ptr<Beryll::Texture> m_tutorialCameraTexture;
        std::unique_ptr<Beryll::Texture> m_tutorialKillEnemiesTexture;
        std::unique_ptr<Beryll::Texture> m_tutorialHealthTexture;
        std::unique_ptr<Beryll::Texture> m_tutorialShotButtonTexture;
        std::unique_ptr<Beryll::Texture> m_tutorialCompletedTexture;
        bool m_tutorialCompletedButtonClicked = false;

        // Resurrect.
        std::unique_ptr<Beryll::Texture> m_resurrectTexture;
        std::unique_ptr<Beryll::Texture> m_resurrectByCrystalsButtonTexture;
        std::unique_ptr<Beryll::Texture> m_resurrectByAdButtonTexture;
        bool m_resurrectMenuShow = false;
        bool m_resurrectByCrystalsButtonClicked = false;
        bool m_resurrectByAdButtonClicked = false;

        // Not enough crystals menu.
        std::unique_ptr<Beryll::Texture> m_noCrystalsTexture;
        bool m_noCrystalsButtonOkClicked = false;
        bool m_noCrystalsMenuShow = false;

        // Ad error.
        std::unique_ptr<Beryll::Texture> m_adErrorTexture;
        bool m_adErrorButtonOkClicked = false;
        bool m_adErrorMenuShow = false;

        // Lose.
        std::unique_ptr<Beryll::Texture> m_loseTexture;
        bool m_loseMenuShow = false;

        // Win.
        std::unique_ptr<Beryll::Texture> m_winTexture;
        std::unique_ptr<Beryll::Texture> m_winPrize1ButtonTexture;
        std::unique_ptr<Beryll::Texture> m_winPrize2ButtonTexture;
        bool m_winMenuShow = false;
        bool m_winPrize1ButtonClicked = false;
        bool m_winPrize2ButtonClicked = false;

        float m_m_interruptAdLastTimePlayed = 0.0f;
        const float m_m_interruptAdDelaySec = 120.0f;
        // Ad callbacks. Can be called from different thread.
        std::function<void()> m_resurrectAdSuccessCallback;
        std::function<void()> m_winPrize2AdSuccessCallback;
        std::function<void()> m_commonAdErrorCallback;
        std::function<void()> m_tutorialAdSuccessErrorCallback;
        std::function<void()> m_interruptAdSuccessErrorCallback;
        // Can be assigned from different thread.
        static std::atomic<bool> m_resurrectAdSuccess;
        static std::atomic<bool> m_winPrize2AdSuccess;
        static std::atomic<bool> m_commonAdError;
        static std::atomic<bool> m_tutorialAdSuccessError;
        static std::atomic<bool> m_interruptAdSuccessError;

        // Menus before specific bosses.
        // Tank with commander.
        std::unique_ptr<Beryll::Texture> m_tankWithCommanderTexture;
        bool m_tankWithCommanderMenuShow = false;
        bool m_tankWithCommanderButtonClicked = false;
    };
}
