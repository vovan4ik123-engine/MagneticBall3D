#pragma once

#include "EngineHeaders.h"

namespace MagneticBall3D
{
    class ShopGUILayer : public Beryll::Layer
    {
    public:
        ShopGUILayer();
        ~ShopGUILayer() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

    private:
        std::vector<std::shared_ptr<Beryll::GUIObject>> m_guiObjects;

        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonBack;

        // Crystals item1.
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonItem1FirstBuy;
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonItem1;
        // Crystals item2.
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonItem2FirstBuy;
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonItem2;
        // Crystals item3.
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonItem3FirstBuy;
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonItem3;
        // Crystals item4.
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonItem4FirstBuy;
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonItem4;
        // Crystals item5.
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonItem5FirstBuy;
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonItem5;
        // Crystals item6.
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonItem6FirstBuy;
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonItem6;
        // Disable ads on maps.
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonDisableAdsOnMaps;

        void enableDisableButtons();

        // Purchase error.
        std::shared_ptr<Beryll::GUITexture> m_textureBackgroundError;
        std::shared_ptr<Beryll::GUITexture> m_textureError;
        std::shared_ptr<Beryll::ButtonWithTexture> m_buttonCloseError;

        // Billing system callbacks. Can be called from different thread.
        std::function<void()> m_buyItem1SuccessCallback;
        std::function<void()> m_buyItem2SuccessCallback;
        std::function<void()> m_buyItem3SuccessCallback;
        std::function<void()> m_buyItem4SuccessCallback;
        std::function<void()> m_buyItem5SuccessCallback;
        std::function<void()> m_buyItem6SuccessCallback;
        std::function<void()> m_buyDisableAdsOnMapsSuccessCallback;
        std::function<void()> m_commonErrorCallback;
        // Can be assigned from different thread.
        static std::atomic<bool> m_item1Bought;
        static std::atomic<bool> m_item2Bought;
        static std::atomic<bool> m_item3Bought;
        static std::atomic<bool> m_item4Bought;
        static std::atomic<bool> m_item5Bought;
        static std::atomic<bool> m_item6Bought;
        static std::atomic<bool> m_disableAdsOnMapsBought;
        static std::atomic<bool> m_buyError;
    };
}
