#pragma once

#include "EngineHeaders.h"

namespace MagneticBall3D
{
    class StartMenuGUILayer : public Beryll::Layer
    {
    public:
        StartMenuGUILayer();
        ~StartMenuGUILayer() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

    private:
        // GUI based on raw ImGUI.
        // ImGUI flags.
        int m_noBackgroundNoFrame = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground |
                                    ImGuiWindowFlags_NoScrollbar;

        // Button play.
        static const std::string m_buttonPlayID;
        std::unique_ptr<Beryll::Texture> m_buttonPlayTexture;
        bool m_buttonPlayClicked = false;

        // Button shop.
        static const std::string m_buttonShopID;
        std::unique_ptr<Beryll::Texture> m_buttonShopTexture;
        bool m_buttonShopClicked = false;

        // Button settings.
        static const std::string m_buttonSettingsID;
        std::unique_ptr<Beryll::Texture> m_buttonSettingsTexture;
        bool m_buttonSettingsClicked = false;

        // Button map swipe left.
        static const std::string m_buttonMapSwipeLeftID;
        std::unique_ptr<Beryll::Texture> m_buttonMapSwipeLeftTexture;
        bool m_buttonMapSwipeLeftClicked = false;

        // Button map swipe right.
        static const std::string m_buttonMapSwipeRightID;
        std::unique_ptr<Beryll::Texture> m_buttonMapSwipeRightTexture;
        bool m_buttonMapSwipeRightClicked = false;

        // Texture map(preview).
        static const std::string m_mapTextureID;
        std::vector<std::unique_ptr<Beryll::Texture>> m_allMapsTextures;

        // Text crystals.
        static const std::string m_textCrystalsID;
        ImFont* m_fontCrystals;
    };
}
