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
        COP_WITH_GRENADE_LAUNCHER,
        SNIPER
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
        static int garbageCountInMagneticRadius;
        bool isInMagneticRadius = false;

        int hp = 0;

    private:
        bool m_isEnabled = false;
        static int m_activeGarbageCount;
        GarbageType m_type = GarbageType::NONE;
    };
}
