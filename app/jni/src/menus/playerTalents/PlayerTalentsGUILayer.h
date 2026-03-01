#pragma once

#include "EngineHeaders.h"

namespace MagneticBall3D
{
    class PlayerTalentsGUILayer : public Beryll::Layer
    {
    public:
        PlayerTalentsGUILayer();
        ~PlayerTalentsGUILayer() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

    private:
        std::vector<std::shared_ptr<Beryll::GUIObject>> m_guiObjects;

        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonBack;

        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonMaxSpeed;
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonMagneticRadius;
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonAmountOfMagnetizedItems;
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonAccelerateFaster;
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonBallAndItemsProtection;
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonResurrectionAttempts;
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonSmashDamage;
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonShotDamage;

        // Menu selected talent.
        int m_selectedCurrentLevel = 0;
        int m_selectedMaxLevel = 0;
        int m_selectedCanBeImprovedByAds = 0;
        static int m_selectedIndex;

        void selectTalent(int index);

        std::shared_ptr<Beryll::GUITexture> m_textureImproveByAd;
        std::shared_ptr<Beryll::GUITexture> m_textureImproveByAdTimer;
        std::shared_ptr<Beryll::GUITexture> m_textureImproveByCrystals;
        std::shared_ptr<Beryll::GUITexture> m_textureMaxLevelReached;

        std::shared_ptr<Beryll::GUIText> m_textSelectedDescription1;
        std::shared_ptr<Beryll::GUIText> m_textSelectedDescription2;
        std::shared_ptr<Beryll::GUIText> m_textSelectedValue;
        std::shared_ptr<Beryll::GUIText> m_textSelectedValueToAdd;
        std::shared_ptr<Beryll::GUIText> m_textOr;
        std::string m_adTimerValue;
        std::shared_ptr<Beryll::GUIText> m_textAdTimer;
        int m_selectedPriceCrystals = 0;
        std::shared_ptr<Beryll::GUIText> m_textSelectedPriceCrystals;

        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonImproveTalentByAd;
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonImproveTalentByCrystal;

        // Ad error + Not enough crystals error.
        std::shared_ptr<Beryll::GUITexture> m_textureBackgroundError;
        std::shared_ptr<Beryll::GUITexture> m_textureAdError;
        std::shared_ptr<Beryll::GUITexture> m_textureCrystalsError;
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonCloseError;

        // Ad callbacks. Can be called from different thread.
        std::function<void()> m_adSuccessCallback;
        std::function<void()> m_adErrorCallback;
        // Can be assigned from different thread.
        static std::atomic<bool> m_adSuccess;
        static std::atomic<bool> m_adError;
    };
}
