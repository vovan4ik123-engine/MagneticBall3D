#include "Garbage.h"

namespace MagneticBall3D
{
    int Garbage::m_activeGarbageCount = 0;
    int Garbage::garbageCountInMagneticRadius = 0;

    Garbage::Garbage(std::shared_ptr<Beryll::SimpleCollidingObject> so, GarbageType type, int health)
    : obj(std::move(so)), m_type(type), hp(health)
    {
        enableGarbage();
    }

    Garbage::~Garbage()
    {

    }

    void Garbage::update()
    {
        if(hp <= 0)
            disableGarbage();
    }

    void Garbage::enableGarbage()
    {
        obj->enableDraw();
        obj->enableUpdate();
        obj->enableCollisionMesh();

        ++Garbage::m_activeGarbageCount;
        m_isEnabled = true;
    }

    void Garbage::disableGarbage()
    {
        obj->disableDraw();
        obj->disableUpdate();
        obj->disableCollisionMesh();

        if(Garbage::m_activeGarbageCount > 0)
            --Garbage::m_activeGarbageCount;

        m_isEnabled = false;
    }
}
