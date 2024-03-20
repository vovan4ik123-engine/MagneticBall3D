#pragma once

#include "EngineHeaders.h"

namespace MagneticBall3D
{
    enum class GarbageType
    {
        NONE,
        // Appears on map after loading.
        DEFAULT,
        // Appears after killing specific type of enemy.
        COP_WITH_PISTOL,
        PISTOL,
        COP_WITH_GRENADE_LAUNCHER,
        GRENADE_LAUNCHER,
        SNIPER,
        SNIPER_GUN
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
        static int getActiveCount() { return Garbage::m_activeGarbageCount; }
        GarbageType getType() { return m_type; }

        const std::shared_ptr<Beryll::SimpleCollidingObject> obj;
        bool isMagnetized = false;

        int currentHP = 0;

    private:
        bool m_isEnabled = true;
        static int m_activeGarbageCount;
        GarbageType m_type = GarbageType::NONE;
        const int m_maxHP = 0;
    };
}
