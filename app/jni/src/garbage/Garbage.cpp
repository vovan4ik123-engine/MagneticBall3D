#include "Garbage.h"

namespace MagneticBall3D
{
    int Garbage::m_commonActiveCount = 0;

    Garbage::Garbage(std::shared_ptr<Beryll::SimpleCollidingObject> so, GarbageType type, float health)
    : obj(std::move(so)), m_type(type), m_maxHP(health), m_currentHP(health)
    {
        disableGarbage();
    }

    Garbage::~Garbage()
    {
        //BR_INFO("%s", "Garbage::~Garbage()");
        disableGarbage();
    }

    void Garbage::update()
    {
        // Garbage has no HP or dropped down from map border.
        if(m_currentHP <= 0 || obj->getOrigin().y < -50.0f)
        {
            disableGarbage();
        }
    }

    void Garbage::enableGarbage()
    {
        // Count DEFAULT garbage because it has limit on map. Can be increased by improvements.
        if(m_type == GarbageType::COMMON)
            ++Garbage::m_commonActiveCount;

        obj->enableDraw();
        obj->enableUpdate();
        obj->enableCollisionMesh();

        m_isEnabled = true;

        m_currentHP = m_maxHP;
    }

    void Garbage::disableGarbage()
    {
        //BR_INFO("%s", "disableGarbage()");

        // Count DEFAULT garbage because it has limit on map. Can be increased by improvements.
        if(m_isEnabled && m_commonActiveCount > 0 && m_type == GarbageType::COMMON)
            --Garbage::m_commonActiveCount;

        obj->resetVelocities();
        obj->disableDraw();
        obj->disableUpdate();
        obj->disableCollisionMesh();

        m_isEnabled = false;
        isMagnetized = false;
    }
}
