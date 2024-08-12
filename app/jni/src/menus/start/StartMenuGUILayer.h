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

        int m_noFrame = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar;

        int m_noFrameNoFocus = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                               ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBringToFrontOnFocus;

        // Crystals.
        static const std::string m_mainMenuID;
        std::unique_ptr<Beryll::Texture> m_crystalIconTexture;
        ImFont* m_crystalsFont;

        // Play.
        static const std::string m_playButtonID;
        std::unique_ptr<Beryll::Texture> m_playButtonTexture;
        bool m_playButtonClicked = false;

        // Shop.
        static const std::string m_shopButtonID;
        std::unique_ptr<Beryll::Texture> m_shopButtonTexture;
        bool m_shopButtonClicked = false;

        // Talents.
        static const std::string m_talentsButtonID;
        std::unique_ptr<Beryll::Texture> m_talentsButtonTexture;
        bool m_talentsButtonClicked = false;

        // Settings.
        static const std::string m_settingsButtonID;
        std::unique_ptr<Beryll::Texture> m_settingsButtonTexture;
        bool m_settingsButtonClicked = false;

        // Map swipe left.
        static const std::string m_mapSwipeLeftButtonID;
        std::unique_ptr<Beryll::Texture> m_mapSwipeLeftButtonTexture;
        bool m_mapSwipeLeftButtonClicked = false;

        // Map swipe right.
        static const std::string m_mapSwipeRightButtonID;
        std::unique_ptr<Beryll::Texture> m_mapSwipeRightButtonTexture;
        bool m_mapSwipeRightButtonClicked = false;

        // Map preview.
        std::vector<std::unique_ptr<Beryll::Texture>> m_allMapsPreviewsTextures;
    };
}
