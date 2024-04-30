#include "Garbage.h"

namespace MagneticBall3D
{
    int Garbage::m_activeCommonGarbageCount = 0;

    Garbage::Garbage(std::shared_ptr<Beryll::SimpleCollidingObject> so, GarbageType type, float health)
    : obj(std::move(so)), m_type(type), m_maxHP(health), m_currentHP(health)
    {

    }

    Garbage::~Garbage()
    {
        //BR_INFO("%s", "Garbage::~Garbage()");
        disableGarbage();
    }

    void Garbage::update()
    {
        // Garbage nas no HP or dropped down from map border.
        if(m_currentHP <= 0 || obj->getOrigin().y < -500.0f)
        {
            disableGarbage();
        }
    }

    void Garbage::enableGarbage()
    {
        obj->enableDraw();
        obj->enableUpdate();
        obj->enableCollisionMesh();

        // Count DEFAULT garbage because it has limit on map.
        if(m_type == GarbageType::COMMON)
            ++Garbage::m_activeCommonGarbageCount;

        m_isEnabled = true;

        m_currentHP = m_maxHP;
    }

    void Garbage::disableGarbage()
    {
        obj->resetVelocities();
        obj->disableDraw();
        obj->disableUpdate();
        obj->disableCollisionMesh();

        // Count DEFAULT garbage because it has limit on map.
        if(Garbage::m_activeCommonGarbageCount > 0 && m_type == GarbageType::COMMON)
            --Garbage::m_activeCommonGarbageCount;

        m_isEnabled = false;
        isMagnetized = false;
    }
}
