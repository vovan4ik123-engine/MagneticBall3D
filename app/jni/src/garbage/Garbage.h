#pragma once

#include "EngineHeaders.h"

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
        Garbage(std::shared_ptr<Beryll::SimpleCollidingObject> so, GarbageType type, int health);
        ~Garbage();

        void update();
        void enableGarbage();
        void disableGarbage();
        bool getIsEnabled() { return m_isEnabled; }
        static int getActiveCommonGarbageCount() { return Garbage::m_activeCommonGarbageCount; }
        GarbageType getType() { return m_type; }

        const std::shared_ptr<Beryll::SimpleCollidingObject> obj;
        bool isMagnetized = false;

        int currentHP = 0;

    private:
        bool m_isEnabled = true;
        static int m_activeCommonGarbageCount;
        GarbageType m_type = GarbageType::NONE;

        // Hp.
        const int m_maxHP = 0;
    };
}
