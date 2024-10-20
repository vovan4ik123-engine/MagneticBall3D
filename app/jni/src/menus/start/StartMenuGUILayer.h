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
        int m_noBackgroundNoFrameNoFocus = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                           ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                                           ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus;

        int m_noFrame = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar;

        int m_noFrameNoFocus = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                               ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBringToFrontOnFocus;

        // Background.
        std::unique_ptr<Beryll::Texture> m_backgroundTexture;
        // Crystals text.
        ImFont* m_crystalsFont;

        // Play.
        std::unique_ptr<Beryll::Texture> m_playButtonTexture;
        bool m_playButtonClicked = false;

        // Shop.
        std::unique_ptr<Beryll::Texture> m_shopButtonTexture;
        bool m_shopButtonClicked = false;

        // Talents.
        std::unique_ptr<Beryll::Texture> m_talentsButtonTexture;
        bool m_talentsButtonClicked = false;

        // Settings.
        std::unique_ptr<Beryll::Texture> m_settingsButtonTexture;
        bool m_settingsButtonClicked = false;

        // Map swipe left.
        std::unique_ptr<Beryll::Texture> m_mapSwipeLeftButtonTexture;
        bool m_mapSwipeLeftButtonClicked = false;

        // Map swipe right.
        std::unique_ptr<Beryll::Texture> m_mapSwipeRightButtonTexture;
        bool m_mapSwipeRightButtonClicked = false;

        // Map preview.
        std::vector<std::unique_ptr<Beryll::Texture>> m_allMapsPreviewsTextures;
    };
}
