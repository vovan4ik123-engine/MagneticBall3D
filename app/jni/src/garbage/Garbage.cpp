#include "Garbage.h"

namespace MagneticBall3D
{
    int Garbage::m_activeDefaultGarbageCount = 0;

    Garbage::Garbage(std::shared_ptr<Beryll::SimpleCollidingObject> so, GarbageType type, int health)
    : obj(std::move(so)), m_type(type), m_maxHP(health)
    {
        currentHP = m_maxHP;
    }

    Garbage::~Garbage()
    {
        //BR_INFO("%s", "Garbage::~Garbage()");
    }

    void Garbage::update()
    {
        // Garbage nas no HP or dropped down from map border.
        if(currentHP <= 0 || obj->getOrigin().y < -500.0f)
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
        if(m_type == GarbageType::DEFAULT)
            ++Garbage::m_activeDefaultGarbageCount;

        m_isEnabled = true;

        currentHP = m_maxHP;
    }

    void Garbage::disableGarbage()
    {
        obj->disableDraw();
        obj->disableUpdate();
        obj->disableCollisionMesh();

        // Count DEFAULT garbage because it has limit on map.
        if(Garbage::m_activeDefaultGarbageCount > 0 && m_type == GarbageType::DEFAULT)
            --Garbage::m_activeDefaultGarbageCount;

        m_isEnabled = false;
        isMagnetized = false;
    }
}
