#pragma once

#include "EngineHeaders.h"

namespace MagneticBall3D
{
    enum class SoundType
    {
        SWIPE_ENGINE,
        PISTOL_SHOT,
        PISTOL_HIT,
        GRENADE_LAUNCHER_SHOT,
        GRENADE_LAUNCHER_HIT,
        TANK_SHOT,
        TANK_HIT,
        SMASH_COP
    };

    class Sounds
    {
    public:
        Sounds() = delete;
        ~Sounds() = delete;

        static void loadSounds();
        static void update();
        static void playSound(SoundType type);

        static float pistolShotTime;
        static float pistolShotDelay;
        static float pistolHitTime;
        static float pistolHitDelay;
        static float grenadeLauncherShotTime;
        static float grenadeLauncherShotDelay;
        static float grenadeLauncherHitTime;
        static float grenadeLauncherHitDelay;
        static float tankShotTime;
        static float tankShotDelay;
        static float tankHitTime;
        static float tankHitDelay;

    private:
        static bool m_loaded;
        static int m_numberOfCurrentlyPlayingWAV;

        // Sounds.
        static std::string m_swipeEngine1;
        static std::string m_swipeEngine2;

        static std::string m_pistolShot1;
        static std::string m_pistolShot2;
        static std::string m_pistolShot3;
        static std::string m_pistolShot4;
        static std::string m_pistolHit1;
        static std::string m_pistolHit2;
        static std::string m_pistolHit3;
        static std::string m_pistolHit4;

        static std::string m_grenadeLauncherShot1;
        static std::string m_grenadeLauncherHit1;

        static std::string m_tankShot1;
        static std::string m_tankHit1;

        static std::string m_pop1;
        static std::string m_pop2;
        static std::string m_pop3;
        static std::string m_pop4;
        static std::string m_pop5;
        static std::string m_pop6;
        static std::string m_pop7;
        static std::string m_pop8;
    };
}
