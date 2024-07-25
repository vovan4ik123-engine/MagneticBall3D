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
        int m_noBackgroundNoFrame = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground |
                                    ImGuiWindowFlags_NoScrollbar;

        // Button back.
        static const std::string m_buttonBackID;
        std::unique_ptr<Beryll::Texture> m_buttonBackTexture;
        bool m_buttonBackClicked = false;

        // Shop header.
        static const std::string m_shopHeaderID;
        std::unique_ptr<Beryll::Texture> m_shopHeaderTexture;

        // Button item1.
        static const std::string m_buttonItem1ID;
        std::unique_ptr<Beryll::Texture> m_buttonItem1Texture;
        bool m_buttonItem1Clicked = false;

        // Button item2.
        static const std::string m_buttonItem2ID;
        std::unique_ptr<Beryll::Texture> m_buttonItem2Texture;
        bool m_buttonItem2Clicked = false;

        // Button item3.
        static const std::string m_buttonItem3ID;
        std::unique_ptr<Beryll::Texture> m_buttonItem3Texture;
        bool m_buttonItem3Clicked = false;

        // Button item4.
        static const std::string m_buttonItem4ID;
        std::unique_ptr<Beryll::Texture> m_buttonItem4Texture;
        bool m_buttonItem4Clicked = false;

        // Button item5.
        static const std::string m_buttonItem5ID;
        std::unique_ptr<Beryll::Texture> m_buttonItem5Texture;
        bool m_buttonItem5Clicked = false;

        // Button item6.
        static const std::string m_buttonItem6ID;
        std::unique_ptr<Beryll::Texture> m_buttonItem6Texture;
        bool m_buttonItem6Clicked = false;

        // These callbacks can be called from different thread.
        std::function<void()> m_buyItem1SuccessCallback;
        std::function<void()> m_buyItem2SuccessCallback;
        std::function<void()> m_buyItem3SuccessCallback;
        std::function<void()> m_buyItem4SuccessCallback;
        std::function<void()> m_buyItem5SuccessCallback;
        std::function<void()> m_buyItem6SuccessCallback;
        std::function<void()> m_commonErrorCallback;
        // Сan be assigned from different thread.
        static std::atomic<bool> m_item1Bought;
        static std::atomic<bool> m_item2Bought;
        static std::atomic<bool> m_item3Bought;
        static std::atomic<bool> m_item4Bought;
        static std::atomic<bool> m_item5Bought;
        static std::atomic<bool> m_item6Bought;
        static std::atomic<bool> m_buyError;
    };
}
