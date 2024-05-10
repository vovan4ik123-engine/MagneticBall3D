#include "Sounds.h"
#include "EnumsAndVariables.h"

namespace MagneticBall3D
{
    bool Sounds::m_loaded = false;

    std::string Sounds::m_swipeEngine1 = "sounds/SwipeEngine1.wav";
    std::string Sounds::m_swipeEngine2 = "sounds/SwipeEngine2.wav";

    std::string Sounds::m_pistolShot1 = "sounds/PistolShot1.wav";
    std::string Sounds::m_pistolShot2 = "sounds/PistolShot2.wav";
    std::string Sounds::m_pistolShot3 = "sounds/PistolShot3.wav";
    std::string Sounds::m_pistolShot4 = "sounds/PistolShot4.wav";
    float Sounds::pistolShotTime = 0.0f;
    float Sounds::pistolShotDelay = 0.004f;
    std::string Sounds::m_pistolHit1 = "sounds/PistolHit1.wav";
    std::string Sounds::m_pistolHit2 = "sounds/PistolHit2.wav";
    std::string Sounds::m_pistolHit3 = "sounds/PistolHit3.wav";
    std::string Sounds::m_pistolHit4 = "sounds/PistolHit4.wav";
    float Sounds::pistolHitTime = 0.0f;
    float Sounds::pistolHitDelay = 0.004f;

    std::string Sounds::m_grenadeLauncherShot1 = "sounds/GrenadeLauncherShot1.wav";
    float Sounds::grenadeLauncherShotTime = 0.0f;
    float Sounds::grenadeLauncherShotDelay = 0.004f;
    std::string Sounds::m_grenadeLauncherHit1 = "sounds/GrenadeLauncherHit1.wav";
    float Sounds::grenadeLauncherHitTime = 0.0f;
    float Sounds::grenadeLauncherHitDelay = 0.004f;

    std::string Sounds::m_tankShot1 = "sounds/TankShot1.wav";
    float Sounds::tankShotTime = 0.0f;
    float Sounds::tankShotDelay = 0.004f;
    std::string Sounds::m_tankHit1 = "sounds/TankHit1.wav";
    float Sounds::tankHitTime = 0.0f;
    float Sounds::tankHitDelay = 0.004f;

    std::string Sounds::m_smash1 = "sounds/Smash1.wav";
    std::string Sounds::m_smash2 = "sounds/Smash2.wav";
    std::string Sounds::m_smash3 = "sounds/Smash3.wav";
    std::string Sounds::m_smash4 = "sounds/Smash4.wav";
    std::string Sounds::m_smash5 = "sounds/Smash5.wav";
    float Sounds::smashTime = 0.0f;
    float Sounds::smashDelay = 0.004f;

    void Sounds::loadSounds()
    {
        if(m_loaded)  { return; }

        Beryll::SoundsManager::loadWAV(m_swipeEngine1);
        Beryll::SoundsManager::loadWAV(m_swipeEngine2);

        Beryll::SoundsManager::loadWAV(m_pistolShot1);
        Beryll::SoundsManager::loadWAV(m_pistolShot2);
        Beryll::SoundsManager::loadWAV(m_pistolShot3);
        Beryll::SoundsManager::loadWAV(m_pistolShot4);
        Beryll::SoundsManager::loadWAV(m_pistolHit1);
        Beryll::SoundsManager::loadWAV(m_pistolHit2);
        Beryll::SoundsManager::loadWAV(m_pistolHit3);
        Beryll::SoundsManager::loadWAV(m_pistolHit4);

        Beryll::SoundsManager::loadWAV(m_grenadeLauncherShot1);
        Beryll::SoundsManager::loadWAV(m_grenadeLauncherHit1);

        Beryll::SoundsManager::loadWAV(m_tankShot1);
        Beryll::SoundsManager::loadWAV(m_tankHit1);

        Beryll::SoundsManager::loadWAV(m_smash1);
        Beryll::SoundsManager::loadWAV(m_smash2);
        Beryll::SoundsManager::loadWAV(m_smash3);
        Beryll::SoundsManager::loadWAV(m_smash4);
        Beryll::SoundsManager::loadWAV(m_smash5);

        m_loaded = true;
    }

    void Sounds::playSound(SoundType type)
    {
        if(type == SoundType::SWIPE_ENGINE)
        {
            float randomValue = Beryll::RandomGenerator::getFloat();

            if(randomValue < 0.65f)
                Beryll::SoundsManager::playWAV(m_swipeEngine1);
            else
                Beryll::SoundsManager::playWAV(m_swipeEngine2);

        }
        else if(type == SoundType::PISTOL_SHOT)
        {
            if(pistolShotTime + pistolShotDelay < EnumsAndVariables::mapPlayTimeSec)
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

                pistolShotTime = EnumsAndVariables::mapPlayTimeSec;
            }
        }
        else if(type == SoundType::PISTOL_HIT)
        {
            if(pistolHitTime + pistolHitDelay < EnumsAndVariables::mapPlayTimeSec)
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

                pistolHitTime = EnumsAndVariables::mapPlayTimeSec;
            }
        }
        else if(type == SoundType::GRENADE_LAUNCHER_SHOT)
        {
            if(grenadeLauncherShotTime + grenadeLauncherShotDelay < EnumsAndVariables::mapPlayTimeSec)
            {
                Beryll::SoundsManager::playWAV(m_grenadeLauncherShot1);
                grenadeLauncherShotTime = EnumsAndVariables::mapPlayTimeSec;
            }
        }
        else if(type == SoundType::GRENADE_LAUNCHER_HIT)
        {
            if(grenadeLauncherHitTime + grenadeLauncherHitDelay < EnumsAndVariables::mapPlayTimeSec)
            {
                Beryll::SoundsManager::playWAV(m_grenadeLauncherHit1);
                grenadeLauncherHitTime = EnumsAndVariables::mapPlayTimeSec;
            }
        }
        else if(type == SoundType::TANK_SHOT)
        {
            if(tankShotTime + tankShotDelay < EnumsAndVariables::mapPlayTimeSec)
            {
                Beryll::SoundsManager::playWAV(m_tankShot1);
                tankShotTime = EnumsAndVariables::mapPlayTimeSec;
            }
        }
        else if(type == SoundType::TANK_HIT)
        {
            if(tankHitTime + tankHitDelay < EnumsAndVariables::mapPlayTimeSec)
            {
                Beryll::SoundsManager::playWAV(m_tankHit1);
                tankHitTime = EnumsAndVariables::mapPlayTimeSec;
            }
        }
        else if(type == SoundType::SMASH_COP)
        {
            if(smashTime + smashDelay < EnumsAndVariables::mapPlayTimeSec)
            {
                float randomValue = Beryll::RandomGenerator::getFloat();

                if(randomValue < 0.2f)
                    Beryll::SoundsManager::playWAV(m_smash1);
                else if(randomValue < 0.4f)
                    Beryll::SoundsManager::playWAV(m_smash2);
                else if(randomValue < 0.6f)
                    Beryll::SoundsManager::playWAV(m_smash3);
                else if(randomValue < 0.8f)
                    Beryll::SoundsManager::playWAV(m_smash4);
                else if(randomValue < 1.0f)
                    Beryll::SoundsManager::playWAV(m_smash5);

                smashTime = EnumsAndVariables::mapPlayTimeSec;
            }
        }
    }
}
