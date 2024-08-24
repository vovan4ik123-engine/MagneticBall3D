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
        int m_noBackgroundNoFrame = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground |
                                    ImGuiWindowFlags_NoScrollbar;

        // Back.
        std::unique_ptr<Beryll::Texture> m_backButtonTexture;
        bool m_backButtonClicked = false;

        ImFont* m_fontForAllCheckBoxes;

        // Settings header.
        std::unique_ptr<Beryll::Texture> m_settingsHeaderTexture;

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
    };
}
