#include "Sounds.h"
#include "EnumsAndVariables.h"

namespace MagneticBall3D
{
    bool Sounds::m_loaded = false;
    int Sounds::m_numberOfCurrentlyPlayingWAV = 0;

    // Sounds.
    std::string Sounds::m_pop1 = "sounds/Pop1.wav";
    std::string Sounds::m_pop2 = "sounds/Pop2.wav";
    std::string Sounds::m_pop3 = "sounds/Pop3.wav";
    std::string Sounds::m_pop4 = "sounds/Pop4.wav";
    std::string Sounds::m_pop5 = "sounds/Pop5.wav";
    std::string Sounds::m_pop6 = "sounds/Pop6.wav";
    std::string Sounds::m_pop7 = "sounds/Pop7.wav";
    std::string Sounds::m_pop8 = "sounds/Pop8.wav";
    std::string Sounds::m_jumppad = "sounds/Jumppad.wav";
    std::string Sounds::m_pistolShot1 = "sounds/PistolShot1.wav";
    std::string Sounds::m_pistolShot2 = "sounds/PistolShot2.wav";
    std::string Sounds::m_pistolShot3 = "sounds/PistolShot3.wav";
    std::string Sounds::m_pistolShot4 = "sounds/PistolShot4.wav";
    float Sounds::m_pistolShotTime = 0.0f;
    float Sounds::m_pistolShotDelay = 0.1f;
    std::string Sounds::m_pistolHit1 = "sounds/PistolHit1.wav";
    std::string Sounds::m_pistolHit2 = "sounds/PistolHit2.wav";
    std::string Sounds::m_pistolHit3 = "sounds/PistolHit3.wav";
    std::string Sounds::m_pistolHit4 = "sounds/PistolHit4.wav";
    float Sounds::m_pistolHitTime = 0.0f;
    float Sounds::m_pistolHitDelay = 0.1f;
    std::string Sounds::m_grenadeLauncherShot1 = "sounds/GrenadeLauncherShot1.wav";
    float Sounds::m_grenadeLauncherShotTime = 0.0f;
    float Sounds::m_grenadeLauncherShotDelay = 0.1f;
    std::string Sounds::m_tankShot1 = "sounds/TankShot1.wav";
    float Sounds::m_tankShotTime = 0.0f;
    float Sounds::m_tankShotDelay = 0.1f;
    std::string Sounds::m_rifleShot1 = "sounds/RifleShot1.wav";
    std::string Sounds::m_stickHit1 = "sounds/StickHit1.wav";
    std::string Sounds::m_stickHit2 = "sounds/StickHit2.wav";
    std::string Sounds::m_broomHit1 = "sounds/BroomHit1.wav";
    std::string Sounds::m_fellOnGround1 = "sounds/FellOnGround1.wav";
    float Sounds::m_fellOnGroundTime = 0.0f;
    float Sounds::m_fellOnGroundDelay = 1.0f;
    std::string Sounds::m_bigRocketLaunch1 = "sounds/BigRocketLaunch1.wav";
    float Sounds::m_bigRocketLaunchTime = 0.0f;
    float Sounds::m_bigRocketLaunchDelay = 0.5f;
    std::string Sounds::m_spit1 = "sounds/Spit1.wav";
    std::string Sounds::m_spit2 = "sounds/Spit2.wav";
    std::string Sounds::m_stoneHit1 = "sounds/StoneHit1.wav";
    std::string Sounds::m_stoneHit2 = "sounds/StoneHit2.wav";
    std::string Sounds::m_stoneHit3 = "sounds/StoneHit3.wav";

    // Music.
    std::string Sounds::m_backgroundMusic1 = "sounds/BackgroundMusic1.mp3";
    std::string Sounds::m_backgroundMusic2 = "sounds/BackgroundMusic2.mp3";

    void Sounds::reset()
    {
        m_pistolShotTime = 0.0f;
        m_pistolHitTime = 0.0f;
        m_grenadeLauncherShotTime = 0.0f;
        m_tankShotTime = 0.0f;
        m_fellOnGroundTime = 0.0f;
        m_bigRocketLaunchTime = 0.0f;

        stopBackgroundMusic();
    }

    void Sounds::loadSounds()
    {
        if(m_loaded)  { return; }

        Beryll::SoundsManager::loadWAV(m_pistolShot1, 11);
        Beryll::SoundsManager::loadWAV(m_pistolShot2, 11);
        Beryll::SoundsManager::loadWAV(m_pistolShot3, 11);
        Beryll::SoundsManager::loadWAV(m_pistolShot4, 11);
        Beryll::SoundsManager::loadWAV(m_pistolHit1, 14);
        Beryll::SoundsManager::loadWAV(m_pistolHit2, 14);
        Beryll::SoundsManager::loadWAV(m_pistolHit3, 14);
        Beryll::SoundsManager::loadWAV(m_pistolHit4, 14);
        Beryll::SoundsManager::loadWAV(m_grenadeLauncherShot1, 15);
        Beryll::SoundsManager::loadWAV(m_tankShot1, 20);
        Beryll::SoundsManager::loadWAV(m_rifleShot1, 25);
        Beryll::SoundsManager::loadWAV(m_pop1, 80);
        Beryll::SoundsManager::loadWAV(m_pop2, 80);
        Beryll::SoundsManager::loadWAV(m_pop3, 80);
        Beryll::SoundsManager::loadWAV(m_pop4, 80);
        Beryll::SoundsManager::loadWAV(m_pop5, 80);
        Beryll::SoundsManager::loadWAV(m_pop6, 80);
        Beryll::SoundsManager::loadWAV(m_pop7, 80);
        Beryll::SoundsManager::loadWAV(m_pop8, 80);
        Beryll::SoundsManager::loadWAV(m_jumppad, 80);
        Beryll::SoundsManager::loadWAV(m_stickHit1, 40);
        Beryll::SoundsManager::loadWAV(m_stickHit2, 40);
        Beryll::SoundsManager::loadWAV(m_broomHit1, 40);
        Beryll::SoundsManager::loadWAV(m_fellOnGround1, 80);
        Beryll::SoundsManager::loadWAV(m_bigRocketLaunch1, 40);
        Beryll::SoundsManager::loadWAV(m_spit1, 30);
        Beryll::SoundsManager::loadWAV(m_spit2, 30);
        Beryll::SoundsManager::loadWAV(m_stoneHit1, 30);
        Beryll::SoundsManager::loadWAV(m_stoneHit2, 30);
        Beryll::SoundsManager::loadWAV(m_stoneHit3, 30);

        Beryll::SoundsManager::loadBackgroundMP3(m_backgroundMusic1, 15);
        Beryll::SoundsManager::loadBackgroundMP3(m_backgroundMusic2, 15);

        m_loaded = true;
    }

    void Sounds::update()
    {
        m_numberOfCurrentlyPlayingWAV = Beryll::SoundsManager::getNumberOfWAVCurrentlyPlaying();
        //BR_INFO("m_numberOfCurrentlyPlayingWAV: %d", m_numberOfCurrentlyPlayingWAV);
    }

    void Sounds::playSoundEffect(SoundType type)
    {
        if(type == SoundType::FELL_ON_GROUND)
        {
            if(m_fellOnGroundTime + m_fellOnGroundDelay < EnumsAndVars::mapPlayTimeSec)
            {
                m_fellOnGroundTime = EnumsAndVars::mapPlayTimeSec;
                Beryll::SoundsManager::playWAV(m_fellOnGround1);
            }
        }

        if(type == SoundType::NONE || m_numberOfCurrentlyPlayingWAV >= 8)
            return;

        if(type == SoundType::POP)
        {
            float randomValue = Beryll::RandomGenerator::getFloat();
            if(randomValue < 0.1f)
                Beryll::SoundsManager::playWAV(m_pop1);
            else if(randomValue < 0.2f)
                Beryll::SoundsManager::playWAV(m_pop2);
            else if(randomValue < 0.3f)
                Beryll::SoundsManager::playWAV(m_pop3);
            else if(randomValue < 0.4f)
                Beryll::SoundsManager::playWAV(m_pop4);
            else if(randomValue < 0.5f)
                Beryll::SoundsManager::playWAV(m_pop5);
            else if(randomValue < 0.7f)
                Beryll::SoundsManager::playWAV(m_pop6);
            else if(randomValue < 0.8f)
                Beryll::SoundsManager::playWAV(m_pop7);
            else if(randomValue < 1.0f)
                Beryll::SoundsManager::playWAV(m_pop8);

            return;
        }
        else if(type == SoundType::JUMPPAD)
        {
            Beryll::SoundsManager::playWAV(m_jumppad);
        }

        if(m_numberOfCurrentlyPlayingWAV >= 4)
            return;

        if(type == SoundType::PISTOL_SHOT)
        {
            if(m_pistolShotTime + m_pistolShotDelay < EnumsAndVars::mapPlayTimeSec)
            {
                m_pistolShotTime = EnumsAndVars::mapPlayTimeSec;

                float randomValue = Beryll::RandomGenerator::getFloat();
                if(randomValue < 0.25f)
                    Beryll::SoundsManager::playWAV(m_pistolShot1);
                else if(randomValue < 0.5f)
                    Beryll::SoundsManager::playWAV(m_pistolShot2);
                else if(randomValue < 0.75f)
                    Beryll::SoundsManager::playWAV(m_pistolShot3);
                else if(randomValue < 1.0f)
                    Beryll::SoundsManager::playWAV(m_pistolShot4);
            }
        }
        else if(type == SoundType::PISTOL_HIT)
        {
            if(m_pistolHitTime + m_pistolHitDelay < EnumsAndVars::mapPlayTimeSec)
            {
                m_pistolHitTime = EnumsAndVars::mapPlayTimeSec;

                float randomValue = Beryll::RandomGenerator::getFloat();
                if(randomValue < 0.25f)
                    Beryll::SoundsManager::playWAV(m_pistolHit1);
                else if(randomValue < 0.5f)
                    Beryll::SoundsManager::playWAV(m_pistolHit2);
                else if(randomValue < 0.75f)
                    Beryll::SoundsManager::playWAV(m_pistolHit3);
                else if(randomValue < 1.0f)
                    Beryll::SoundsManager::playWAV(m_pistolHit4);
            }
        }
        else if(type == SoundType::GRENADE_LAUNCHER_SHOT)
        {
            if(m_grenadeLauncherShotTime + m_grenadeLauncherShotDelay < EnumsAndVars::mapPlayTimeSec)
            {
                m_grenadeLauncherShotTime = EnumsAndVars::mapPlayTimeSec;
                Beryll::SoundsManager::playWAV(m_grenadeLauncherShot1);
            }
        }
        else if(type == SoundType::TANK_SHOT)
        {
            if(m_tankShotTime + m_tankShotDelay < EnumsAndVars::mapPlayTimeSec)
            {
                m_tankShotTime = EnumsAndVars::mapPlayTimeSec;
                Beryll::SoundsManager::playWAV(m_tankShot1);
            }
        }
        else if(type == SoundType::RIFLE_SHOT)
        {
            Beryll::SoundsManager::playWAV(m_rifleShot1);
        }
        else if(type == SoundType::STICK_HIT)
        {
            float randomValue = Beryll::RandomGenerator::getFloat();
            if(randomValue < 0.5f)
                Beryll::SoundsManager::playWAV(m_stickHit1);
            else
                Beryll::SoundsManager::playWAV(m_stickHit2);
        }
        else if(type == SoundType::BROOM_HIT)
        {
            Beryll::SoundsManager::playWAV(m_broomHit1);
        }
        else if(type == SoundType::BIG_ROCKET_LAUNCH)
        {
            if(m_bigRocketLaunchTime + m_bigRocketLaunchDelay < EnumsAndVars::mapPlayTimeSec)
            {
                m_bigRocketLaunchTime = EnumsAndVars::mapPlayTimeSec;
                Beryll::SoundsManager::playWAV(m_bigRocketLaunch1);
            }
        }
        else if(type == SoundType::SPIT)
        {
            float randomValue = Beryll::RandomGenerator::getFloat();
            if(randomValue < 0.5f)
                Beryll::SoundsManager::playWAV(m_spit1);
            else
                Beryll::SoundsManager::playWAV(m_spit2);
        }
        else if(type == SoundType::STONE_HIT)
        {
            float randomValue = Beryll::RandomGenerator::getFloat();
            if(randomValue < 0.45f)
                Beryll::SoundsManager::playWAV(m_stoneHit1);
            else if(randomValue < 0.9f)
                Beryll::SoundsManager::playWAV(m_stoneHit2);
            else
                Beryll::SoundsManager::playWAV(m_stoneHit3);
        }
    }

    void Sounds::startBackgroundMusic(SoundType type)
    {
        if(!EnumsAndVars::SettingsMenu::backgroundMusic)
            return;

        if(type == SoundType::BACKGROUND_MUSIC_1)
        {
            Beryll::SoundsManager::startBackgroundMP3(m_backgroundMusic1, true);
        }
        else if(type == SoundType::BACKGROUND_MUSIC_2)
        {
            Beryll::SoundsManager::startBackgroundMP3(m_backgroundMusic2, true);
        }
    }

    void Sounds::stopBackgroundMusic()
    {
        Beryll::SoundsManager::stopBackgroundMP3();
    }

    void Sounds::pauseBackgroundMusic()
    {
        Beryll::SoundsManager::pauseBackgroundMP3();
    }

    void Sounds::resumeBackgroundMusic()
    {
        Beryll::SoundsManager::resumeBackgroundMP3();
    }
}
