#include "CopWithPistol.h"

namespace MagneticBall3D
{
    CopWithPistol::CopWithPistol(const char* filePath,
                                 float collisionMassKg,
                                 bool wantCollisionCallBack,
                                 Beryll::CollisionFlags collFlag,
                                 Beryll::CollisionGroups collGroup,
                                 Beryll::CollisionGroups collMask,
                                 Beryll::SceneObjectGroups sceneGroup)
                                 : BaseEnemy(filePath,
                                             collisionMassKg,
                                             wantCollisionCallBack,
                                             collFlag,
                                             collGroup,
                                             collMask,
                                             sceneGroup)
    {
        unitState = UnitState::MOVE;
        m_UnitType = UnitType::COP_WITH_PISTOL;
        m_attackType = AttackType::RANGE_DAMAGE_ONE;

        m_damage = 5.0f;
        m_attackDistance = 100.0f + Beryll::RandomGenerator::getFloat() * 100.0f;
        m_timeBetweenAttacks = 1.5f + Beryll::RandomGenerator::getFloat() * 0.1f;

        m_garbageAmountToDie = 10;
        m_reducePlayerSpeedWhenDie = 6.0f;
        m_experienceWhenDie = 20;
        getController().moveSpeed = 25.0f;

        m_lastAttackTime = 0.0f;
    }

    CopWithPistol::~CopWithPistol()
    {

    }
}
