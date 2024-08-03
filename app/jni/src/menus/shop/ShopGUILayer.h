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
        static const std::string m_backButtonID;
        std::unique_ptr<Beryll::Texture> m_backButtonTexture;
        bool m_backButtonClicked = false;

        // Shop header.
        static const std::string m_shopHeaderID;
        std::unique_ptr<Beryll::Texture> m_shopHeaderTexture;

        // Shop all items.
        static const std::string m_allItemsMenuID;
        // Crystals item1.
        static const std::string m_item1ButtonID;
        std::unique_ptr<Beryll::Texture> m_item1ButtonTexture;
        bool m_item1ButtonClicked = false;
        // Crystals item2.
        static const std::string m_item2ButtonID;
        std::unique_ptr<Beryll::Texture> m_item2ButtonTexture;
        bool m_item2ButtonClicked = false;
        // Crystals item3.
        static const std::string m_item3ButtonID;
        std::unique_ptr<Beryll::Texture> m_item3ButtonTexture;
        bool m_item3ButtonClicked = false;
        // Crystals item4.
        static const std::string m_item4ButtonID;
        std::unique_ptr<Beryll::Texture> m_item4ButtonTexture;
        bool m_item4ButtonClicked = false;
        // Crystals item5.
        static const std::string m_item5ButtonID;
        std::unique_ptr<Beryll::Texture> m_item5ButtonTexture;
        bool m_item5ButtonClicked = false;
        // Crystals item6.
        static const std::string m_item6ButtonID;
        std::unique_ptr<Beryll::Texture> m_item6ButtonTexture;
        bool m_item6ButtonClicked = false;

        // Purchase error.
        static const std::string m_errorMenuID;
        std::unique_ptr<Beryll::Texture> m_errorTexture;
        static const std::string m_errorButtonOkID;
        std::unique_ptr<Beryll::Texture> m_errorButtonOkTexture;
        bool m_errorButtonOkClicked = false;
        bool m_showErrorMenu = false;

        // These callbacks can be called from different thread.
        std::function<void()> m_buyItem1SuccessCallback;
        std::function<void()> m_buyItem2SuccessCallback;
        std::function<void()> m_buyItem3SuccessCallback;
        std::function<void()> m_buyItem4SuccessCallback;
        std::function<void()> m_buyItem5SuccessCallback;
        std::function<void()> m_buyItem6SuccessCallback;
        std::function<void()> m_commonErrorCallback;
        // Assigned from different thread.
        static std::atomic<bool> m_item1Bought;
        static std::atomic<bool> m_item2Bought;
        static std::atomic<bool> m_item3Bought;
        static std::atomic<bool> m_item4Bought;
        static std::atomic<bool> m_item5Bought;
        static std::atomic<bool> m_item6Bought;
        static std::atomic<bool> m_buyError;
    };
}
