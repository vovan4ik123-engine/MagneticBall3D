#include "Sounds.h"
#include "EnumsAndVariables.h"

namespace MagneticBall3D
{
    bool Sounds::m_loaded = false;
    int Sounds::m_numberOfCurrentlyPlayingWAV = 0;

    std::string Sounds::m_pistolShot1 = "sounds/PistolShot1.wav";
    std::string Sounds::m_pistolShot2 = "sounds/PistolShot2.wav";
    std::string Sounds::m_pistolShot3 = "sounds/PistolShot3.wav";
    std::string Sounds::m_pistolShot4 = "sounds/PistolShot4.wav";
    float Sounds::pistolShotTime = 0.0f;
    float Sounds::pistolShotDelay = 0.01f;
    std::string Sounds::m_pistolHit1 = "sounds/PistolHit1.wav";
    std::string Sounds::m_pistolHit2 = "sounds/PistolHit2.wav";
    std::string Sounds::m_pistolHit3 = "sounds/PistolHit3.wav";
    std::string Sounds::m_pistolHit4 = "sounds/PistolHit4.wav";
    float Sounds::pistolHitTime = 0.0f;
    float Sounds::pistolHitDelay = 0.01f;

    std::string Sounds::m_grenadeLauncherShot1 = "sounds/GrenadeLauncherShot1.wav";
    float Sounds::grenadeLauncherShotTime = 0.0f;
    float Sounds::grenadeLauncherShotDelay = 0.01f;

    std::string Sounds::m_tankShot1 = "sounds/TankShot1.wav";
    float Sounds::tankShotTime = 0.0f;
    float Sounds::tankShotDelay = 0.01f;

    std::string Sounds::m_rifleShot1 = "sounds/RifleShot1.wav";

    std::string Sounds::m_pop1 = "sounds/Pop1.wav";
    std::string Sounds::m_pop2 = "sounds/Pop2.wav";
    std::string Sounds::m_pop3 = "sounds/Pop3.wav";
    std::string Sounds::m_pop4 = "sounds/Pop4.wav";
    std::string Sounds::m_pop5 = "sounds/Pop5.wav";
    std::string Sounds::m_pop6 = "sounds/Pop6.wav";
    std::string Sounds::m_pop7 = "sounds/Pop7.wav";
    std::string Sounds::m_pop8 = "sounds/Pop8.wav";

    std::string Sounds::m_backgroundMusic1 = "sounds/BackgroundMusic1.mp3";
    std::string Sounds::m_backgroundMusic2 = "sounds/BackgroundMusic2.mp3";

    void Sounds::reset()
    {
        pistolShotTime = 0.0f;
        pistolHitTime = 0.0f;
        grenadeLauncherShotTime = 0.0f;
        tankShotTime = 0.0f;

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

        Beryll::SoundsManager::loadBackgroundMP3(m_backgroundMusic1, 21);
        Beryll::SoundsManager::loadBackgroundMP3(m_backgroundMusic2, 21);

        m_loaded = true;
    }

    void Sounds::update()
    {
        m_numberOfCurrentlyPlayingWAV = Beryll::SoundsManager::getNumberOfWAVCurrentlyPlaying();
        //BR_INFO("m_numberOfCurrentlyPlayingWAV: %d", m_numberOfCurrentlyPlayingWAV);
    }

    void Sounds::playSoundEffect(SoundType type)
    {
        if(m_numberOfCurrentlyPlayingWAV >= 8)
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

        if(m_numberOfCurrentlyPlayingWAV >= 4)
            return;

        if(type == SoundType::PISTOL_SHOT)
        {
            if(pistolShotTime + pistolShotDelay < EnAndVars::mapPlayTimeSec)
            {
                float randomValue = Beryll::RandomGenerator::getFloat();

                if(randomValue < 0.25f)
                    Beryll::SoundsManager::playWAV(m_pistolShot1);
                else if(randomValue < 0.5f)
                    Beryll::SoundsManager::playWAV(m_pistolShot2);
                else if(randomValue < 0.75f)
                    Beryll::SoundsManager::playWAV(m_pistolShot3);
                else if(randomValue < 1.0f)
                    Beryll::SoundsManager::playWAV(m_pistolShot4);

                pistolShotTime = EnAndVars::mapPlayTimeSec;
            }
        }
        else if(type == SoundType::PISTOL_HIT)
        {
            if(pistolHitTime + pistolHitDelay < EnAndVars::mapPlayTimeSec)
            {
                float randomValue = Beryll::RandomGenerator::getFloat();

                if(randomValue < 0.25f)
                    Beryll::SoundsManager::playWAV(m_pistolHit1);
                else if(randomValue < 0.5f)
                    Beryll::SoundsManager::playWAV(m_pistolHit2);
                else if(randomValue < 0.75f)
                    Beryll::SoundsManager::playWAV(m_pistolHit3);
                else if(randomValue < 1.0f)
                    Beryll::SoundsManager::playWAV(m_pistolHit4);

                pistolHitTime = EnAndVars::mapPlayTimeSec;
            }
        }
        else if(type == SoundType::GRENADE_LAUNCHER_SHOT)
        {
            if(grenadeLauncherShotTime + grenadeLauncherShotDelay < EnAndVars::mapPlayTimeSec)
            {
                Beryll::SoundsManager::playWAV(m_grenadeLauncherShot1);
                grenadeLauncherShotTime = EnAndVars::mapPlayTimeSec;
            }
        }
        else if(type == SoundType::TANK_SHOT)
        {
            if(tankShotTime + tankShotDelay < EnAndVars::mapPlayTimeSec)
            {
                Beryll::SoundsManager::playWAV(m_tankShot1);
                tankShotTime = EnAndVars::mapPlayTimeSec;
            }
        }
        else if(type == SoundType::RIFLE_SHOT)
        {
            Beryll::SoundsManager::playWAV(m_rifleShot1);
        }
    }

    void Sounds::startBackgroundMusic(SoundType type)
    {
        if(!EnAndVars::SettingsMenu::backgroundMusic)
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
}
