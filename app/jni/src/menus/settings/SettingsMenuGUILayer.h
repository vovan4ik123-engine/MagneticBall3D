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

        // Button back.
        static const std::string m_buttonBackID;
        std::unique_ptr<Beryll::Texture> m_buttonBackTexture;
        bool m_buttonBackClicked = false;

        ImFont* m_fontForAllCheckBoxes;

        // Texture settings.
        static const std::string m_settingsTextureID;
        std::unique_ptr<Beryll::Texture> m_settingsTexture;

        // FPS limit.
        static const std::string m_FPSLimitTextureID;
        std::unique_ptr<Beryll::Texture> m_FPSLimitTexture;

        static const std::string m_30FPSCheckBoxID;
        bool m_30FPSChecked = false;
        static const std::string m_60FPSCheckBoxID;
        bool m_60FPSChecked = false;
        static const std::string m_120FPSCheckBoxID;
        bool m_120FPSChecked = false;
        static const std::string m_250FPSCheckBoxID;
        bool m_250FPSChecked = false;

        void resetFPS(int fps);

        // Background music.
        static const std::string m_musicTextureID;
        std::unique_ptr<Beryll::Texture> m_musicTexture;

        static const std::string m_musicCheckBoxID;
        bool m_musicCheckBoxChecked;
    };
}
