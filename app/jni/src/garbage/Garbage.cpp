#include "Garbage.h"

namespace MagneticBall3D
{
    int Garbage::m_commonActiveCount = 0;

    Garbage::Garbage(std::shared_ptr<Beryll::SimpleCollidingObject> so, GarbageType type, float health)
    : obj(std::move(so)), m_type(type), m_maxHP(health), m_currentHP(health)
    {
        disableGarbage();
        damagedEnemyIDs.reserve(20);
    }

    Garbage::~Garbage()
    {
        //BR_INFO("%s", "Garbage::~Garbage()");
        disableGarbage();
    }

    void Garbage::update()
    {
        // Garbage has no HP or dropped down from map border.
        if(m_currentHP <= 0.0f || obj->getOrigin().y < -10.0f)
            disableGarbage();

        if(!m_canBeMagnetized && m_pauseMagnetizationTime + m_pauseMagnetizationDelay < EnumsAndVars::mapPlayTimeSec)
            m_canBeMagnetized = true;

        // Garbage can do damage as bullet only 2 sec after shot.
        if(m_addedToGarbageAsBulletArray && m_shotTime + 2.0f < EnumsAndVars::mapPlayTimeSec)
        {
            const auto iter = std::find(EnumsAndVars::garbageAsBulletsIDs.begin(), EnumsAndVars::garbageAsBulletsIDs.end(), obj->getID());
            BR_ASSERT((iter != EnumsAndVars::garbageAsBulletsIDs.end()), "%s", "garbage must exists in garbageAsBulletsIDs.");

            EnumsAndVars::garbageAsBulletsIDs.erase(iter);
            m_addedToGarbageAsBulletArray = false;
            damagedEnemyIDs.clear();
            //BR_INFO("%s", "remove garbage from EnumsAndVars::garbageAsBulletsIDs");
        }
    }

    void Garbage::enableGarbage()
    {
        if(m_isEnabled)
            return;

        // Count DEFAULT garbage because it has limit on map. Can be increased by improvements.
        if(m_type == GarbageType::COMMON)
            ++Garbage::m_commonActiveCount;

        obj->enableDraw();
        obj->enableUpdate();
        obj->enableCollisionMesh();

        m_isEnabled = true;
        m_canBeMagnetized = true;
        m_pauseMagnetizationTime = -999999.0f;
        m_pauseShotTime = -999999.0f;
        m_pauseResetVelocityTime = -999999.0f;

        m_currentHP = m_maxHP;
    }

    void Garbage::disableGarbage()
    {
        if(!m_isEnabled)
            return;

        //BR_INFO("%s", "disableGarbage()");

        // Count DEFAULT garbage because it has limit on map. Can be increased by improvements.
        if(Garbage::m_commonActiveCount > 0 && m_type == GarbageType::COMMON)
            --Garbage::m_commonActiveCount;

        obj->resetVelocities();
        obj->disableDraw();
        obj->disableUpdate();
        obj->disableCollisionMesh();

        m_isEnabled = false;
        m_canBeMagnetized = false;
        isMagnetized = false;

        if(m_addedToGarbageAsBulletArray)
        {
            const auto iter = std::find(EnumsAndVars::garbageAsBulletsIDs.begin(), EnumsAndVars::garbageAsBulletsIDs.end(), obj->getID());
            BR_ASSERT((iter != EnumsAndVars::garbageAsBulletsIDs.end()), "%s", "garbage must exists in garbageAsBulletsIDs.");

            EnumsAndVars::garbageAsBulletsIDs.erase(iter);
            m_addedToGarbageAsBulletArray = false;
            damagedEnemyIDs.clear();
            //BR_INFO("%s", "remove garbage from EnumsAndVars::garbageAsBulletsIDs");
        }
    }

    void Garbage::shoot(const glm::vec3& shootPos, const glm::vec3& shootDir)
    {
        pauseMagnetization(2.0f);
        obj->setGravity(EnumsAndVars::garbageGravityDefault, false, false);
        obj->setOrigin(shootPos, true);
        obj->applyCentralImpulse(shootDir * EnumsAndVars::damageShotPower);
        m_shotTime = EnumsAndVars::mapPlayTimeSec;

        if(!m_addedToGarbageAsBulletArray)
        {
            m_addedToGarbageAsBulletArray = true;
            EnumsAndVars::garbageAsBulletsIDs.push_back(obj->getID());
            //BR_INFO("%s", "add garbage to EnumsAndVars::garbageAsBulletsIDs");
        }
    }
}
