#pragma once

#include "EngineHeaders.h"
#include "EnumsAndVariables.h"

namespace MagneticBall3D
{
    enum class GarbageType
    {
        NONE,
        // Spawned randomly on map around player.
        COMMON,
        // Appears after killing specific type of enemy.
        ENEMY_GARBAGE1,
        ENEMY_GARBAGE2,
        ENEMY_GARBAGE3,
        ENEMY_GARBAGE4,
        ENEMY_GARBAGE5
    };

    class Garbage
    {
    public:
        Garbage() = delete;
        Garbage(std::shared_ptr<Beryll::SimpleCollidingObject> so, GarbageType type, float health);
        ~Garbage();

        void update();
        void enableGarbage();
        void disableGarbage();
        bool getIsEnabled() { return m_isEnabled; }
        GarbageType getType() { return m_type; }
        static int getCommonActiveCount() { return m_commonActiveCount; }

        const std::shared_ptr<Beryll::SimpleCollidingObject> obj;
        bool isMagnetized = false;

        float getMaxHP() { return m_maxHP; }
        float getCurrentHP() { return m_currentHP; }
        void takeDamage(const float d) { m_currentHP -= (d * std::max(0.0f, EnumsAndVars::garbageDamageTakenMultiplier)); }

        bool getCanBeMagnetized() { return m_canBeMagnetized; }
        void pauseMagnetization(float timeSec)
        {
            isMagnetized = false;
            m_canBeMagnetized = false;
            m_pauseMagnetizationDelay = timeSec;
            m_pauseMagnetizationTime = EnumsAndVars::mapPlayTimeSec;
        }

        bool getCanBeShot() { return m_pauseShotTime + m_pauseShotDelay < EnumsAndVars::mapPlayTimeSec; }
        void shoot(const glm::vec3& shootPos, const glm::vec3& shootDir);
        void pauseShot(float timeSec)
        {
            m_pauseShotDelay = timeSec;
            m_pauseShotTime = EnumsAndVars::mapPlayTimeSec;
        }
        // Shot garbage can damage one enemy only 1 time. Store enemy ID here when damage and dont damage again.
        std::vector<int> damagedEnemyIDs;

        bool getCanVelocityBeReseted() { return m_pauseResetVelocityTime + m_pauseResetVelocityDelay < EnumsAndVars::mapPlayTimeSec; }
        void pauseResetVelocity(float timeSec)
        {
            m_pauseResetVelocityDelay = timeSec;
            m_pauseResetVelocityTime = EnumsAndVars::mapPlayTimeSec;
        }

    private:
        static int m_commonActiveCount;
        GarbageType m_type = GarbageType::NONE;

        bool m_isEnabled = true;
        bool m_canBeMagnetized = true;
        float m_pauseMagnetizationTime = -999999.0f;
        float m_pauseMagnetizationDelay = 0.0f;

        float m_pauseShotTime = -999999.0f;
        float m_pauseShotDelay = 0.0f;
        float m_shotTime = 0.0f;
        bool m_addedToGarbageAsBulletArray = false;

        float m_pauseResetVelocityTime = -999999.0f;
        float m_pauseResetVelocityDelay = 0.0f;

        // Hp.
        const float m_maxHP = 0.0f;
        float m_currentHP = 0.0f;
    };
}
