#pragma once

#include "EngineHeaders.h"

namespace MagneticBall3D
{
    class SettingsMenuGUILayer : public Beryll::Layer
    {
    public:
        SettingsMenuGUILayer();
        ~SettingsMenuGUILayer() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

    private:
        // GUI based on raw ImGUI.
        // ImGUI flags.
        int m_noBackgroundNoFrameNoFocus = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                           ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                                           ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus;

        // Background.
        std::unique_ptr<Beryll::Texture> m_backgroundTexture;

        // Back.
        std::unique_ptr<Beryll::Texture> m_backButtonTexture;
        bool m_backButtonClicked = false;

        ImFont* m_fontForAllCheckBoxes;

        // FPS limit.
        std::unique_ptr<Beryll::Texture> m_FPSLimitTexture;
        bool m_30FPSChecked = false;
        bool m_60FPSChecked = false;
        bool m_120FPSChecked = false;
        bool m_250FPSChecked = false;

        void resetFPS(int fps);

        // FPS tip.
        std::unique_ptr<Beryll::Texture> m_FPSTipTexture;

        // Background music.
        std::unique_ptr<Beryll::Texture> m_musicTexture;
        bool m_musicCheckBoxChecked;

        // Meteor particles.
        std::unique_ptr<Beryll::Texture> m_meteorParticlesTexture;
        bool m_meteorParticlesCheckBoxChecked;

        // Interface GUI.
        std::unique_ptr<Beryll::Texture> m_interfaceGUITexture;
        bool m_interfaceGUICheckBoxChecked;
    };
}
