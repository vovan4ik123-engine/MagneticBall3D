#pragma once

#include "EngineHeaders.h"

namespace MagneticBall3D
{
    enum class SoundType
    {
        PISTOL_SHOT,
        PISTOL_HIT,
        GRENADE_LAUNCHER_SHOT,
        TANK_SHOT,
        RIFLE_SHOT,
        POP,


        BACKGROUND_MUSIC_1,
        BACKGROUND_MUSIC_2
    };

    class Sounds
    {
    public:
        Sounds() = delete;
        ~Sounds() = delete;

        static void reset();

        static void loadSounds();
        static void update();
        static void playSoundEffect(SoundType type);
        static void startBackgroundMusic(SoundType type);
        static void stopBackgroundMusic();

        static float pistolShotTime;
        static float pistolShotDelay;
        static float pistolHitTime;
        static float pistolHitDelay;
        static float grenadeLauncherShotTime;
        static float grenadeLauncherShotDelay;
        static float tankShotTime;
        static float tankShotDelay;

    private:
        static bool m_loaded;
        static int m_numberOfCurrentlyPlayingWAV;

        // Sounds.
        static std::string m_pistolShot1;
        static std::string m_pistolShot2;
        static std::string m_pistolShot3;
        static std::string m_pistolShot4;
        static std::string m_pistolHit1;
        static std::string m_pistolHit2;
        static std::string m_pistolHit3;
        static std::string m_pistolHit4;

        static std::string m_grenadeLauncherShot1;

        static std::string m_tankShot1;

        static std::string m_rifleShot1;

        static std::string m_pop1;
        static std::string m_pop2;
        static std::string m_pop3;
        static std::string m_pop4;
        static std::string m_pop5;
        static std::string m_pop6;
        static std::string m_pop7;
        static std::string m_pop8;

        // Music.
        static std::string m_backgroundMusic1;
        static std::string m_backgroundMusic2;
    };
}
