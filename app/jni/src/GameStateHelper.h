#pragma once

namespace MagneticBall3D
{
    class GameStateHelper
    {
    public:
        static void pushStartMenuState();
        static void pushPlayState();
        static void pushSettingsState();
        static void pushShopState();
        static void popState();
    };
}
