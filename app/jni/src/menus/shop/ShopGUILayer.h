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
        // GUI based on raw ImGUI.
        // ImGUI flags.
        int m_noBackgroundNoFrameNoFocus = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                           ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                                           ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus;

        int m_noFrame = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar;

        // Back.
        std::unique_ptr<Beryll::Texture> m_backButtonTexture;
        bool m_backButtonClicked = false;

        // Shop header.
        std::unique_ptr<Beryll::Texture> m_shopHeaderTexture;

        // Crystals item1.
        std::unique_ptr<Beryll::Texture> m_item1ButtonTexture;
        std::unique_ptr<Beryll::Texture> m_item1FirstBuyButtonTexture;
        bool m_item1ButtonClicked = false;
        // Crystals item2.
        std::unique_ptr<Beryll::Texture> m_item2ButtonTexture;
        std::unique_ptr<Beryll::Texture> m_item2FirstBuyButtonTexture;
        bool m_item2ButtonClicked = false;
        // Crystals item3.
        std::unique_ptr<Beryll::Texture> m_item3ButtonTexture;
        std::unique_ptr<Beryll::Texture> m_item3FirstBuyButtonTexture;
        bool m_item3ButtonClicked = false;
        // Crystals item4.
        std::unique_ptr<Beryll::Texture> m_item4ButtonTexture;
        std::unique_ptr<Beryll::Texture> m_item4FirstBuyButtonTexture;
        bool m_item4ButtonClicked = false;
        // Crystals item5.
        std::unique_ptr<Beryll::Texture> m_item5ButtonTexture;
        std::unique_ptr<Beryll::Texture> m_item5FirstBuyButtonTexture;
        bool m_item5ButtonClicked = false;
        // Crystals item6.
        std::unique_ptr<Beryll::Texture> m_item6ButtonTexture;
        std::unique_ptr<Beryll::Texture> m_item6FirstBuyButtonTexture;
        bool m_item6ButtonClicked = false;
        // Disable ads on maps.
        std::unique_ptr<Beryll::Texture> m_disableAdsOnMapsButtonTexture;
        bool m_disableAdsOnMapsButtonClicked = false;

        // Purchase error.
        std::unique_ptr<Beryll::Texture> m_errorTexture;
        std::unique_ptr<Beryll::Texture> m_errorButtonOkTexture;
        bool m_errorButtonOkClicked = false;
        bool m_showErrorMenu = false;

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
