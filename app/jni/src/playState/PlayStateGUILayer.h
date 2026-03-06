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

        std::shared_ptr<Beryll::Joystick> playerJoystick;
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonShot;

        float progressBarHP = 1.0f;
        float progressBarXP = 0.0f;

        bool resurrectPlayer = false;
        bool tutorialMoveShow = false;
        bool tutorialTextTipsShow = false;

    private:
        std::vector<std::shared_ptr<Beryll::GUIObject>> m_guiObjects;

        // Common button.
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonExit;
        // Pause.
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonPause;
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonResume;
        // Lose.
        std::shared_ptr<Beryll::GUITexture> m_textureLose;
        // Win.
        std::shared_ptr<Beryll::GUITexture> m_textureWin;
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonWinPrize1;
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonWinPrize2;
        // Tutorial.
        std::shared_ptr<Beryll::GUITexture> m_textureTutorialCompleted;
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonOkTutorialCompleted;
        // Counters.
        std::shared_ptr<Beryll::GUITexture> m_textureCounters;
        std::shared_ptr<Beryll::GUIText> m_textEnemiesKilled;
        std::shared_ptr<Beryll::GUIText> m_textSpeed;
        std::shared_ptr<Beryll::GUIText> m_textPlayTime;
        // Map0Tutorial.
        std::shared_ptr<Beryll::GUITexture> m_textureTutorialMove;
        std::shared_ptr<Beryll::GUITexture> m_textureTutorialCamera;
        std::shared_ptr<Beryll::GUITexture> m_textureTutorialKillEnemies;
        std::shared_ptr<Beryll::GUITexture> m_textureTutorialHealth;
        // Resurrect menu.
        std::shared_ptr<Beryll::GUITexture> m_textureResurrect;
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonResurrectByCrystals;
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonResurrectByAd;
        // Ad error + Not enough crystals error.
        std::shared_ptr<Beryll::GUITexture> m_textureBackgroundError;
        std::shared_ptr<Beryll::GUITexture> m_textureAdError;
        std::shared_ptr<Beryll::GUITexture> m_textureCrystalsError;
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonCloseError;

        // Menus before specific bosses.
        // Tank with commander.
        std::shared_ptr<Beryll::GUITexture> m_textureTankWithCommander;
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonCloseBossMessage;

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
        static std::atomic<bool> m_interruptAdSuccessError;

#if defined(BR_DEBUG)
        bool m_statisticsShow = true;
#else
        bool m_statisticsShow = false;
#endif

        std::shared_ptr<Beryll::GUIText> m_statistics1;
        std::shared_ptr<Beryll::GUIText> m_statistics2;
        std::shared_ptr<Beryll::GUIText> m_statistics3;
        uint64_t m_statisticsUpdateTime = 0;
    };
}
