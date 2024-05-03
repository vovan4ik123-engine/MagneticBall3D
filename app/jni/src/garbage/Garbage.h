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
        COP_WITH_PISTOL,
        PISTOL,
        COP_WITH_PISTOL_SHIELD,
        SHIELD,
        COP_WITH_GRENADE_LAUNCHER,
        GRENADE_LAUNCHER,
        SNIPER,
        SNIPER_GUN,
        TANK_TRACK,
        TANK_TURRET
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

        const std::shared_ptr<Beryll::SimpleCollidingObject> obj;
        bool isMagnetized = false;

        float getCurrentHP() { return m_currentHP; }
        float getMaxHP() { return m_maxHP; }
        void takeDamage(float damage) { m_currentHP -= (damage * EnumsAndVariables::garbageDamageTakenMultiplier); }

    private:
        bool m_isEnabled = true;
        GarbageType m_type = GarbageType::NONE;

        // Hp.
        float m_currentHP = 0;
        const float m_maxHP = 0;
    };
}
