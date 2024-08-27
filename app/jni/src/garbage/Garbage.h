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
        void takeDamage(float d) { m_currentHP -= (d * std::max(0.0f, EnumsAndVars::garbageDamageTakenMultiplier)); }
        bool getIsDie() { return m_currentHP <= 0.0f; }

        bool getCanBeMagnetized() { return m_canBeMagnetized; }
        void pauseMagnetization(float timeSec)
        {
            isMagnetized = false;
            m_canBeMagnetized = false;
            m_pauseMagnetizationDelay = timeSec;
            m_pauseMagnetizationTime = EnumsAndVars::mapPlayTimeSec;
        }

    private:
        bool m_isEnabled = false;
        bool m_canBeMagnetized = true;
        float m_pauseMagnetizationTime = 0;
        float m_pauseMagnetizationDelay = 0;
        GarbageType m_type = GarbageType::NONE;
        static int m_commonActiveCount;

        // Hp.
        const float m_maxHP = 0.0f;
        float m_currentHP = 0.0f;
    };
}
