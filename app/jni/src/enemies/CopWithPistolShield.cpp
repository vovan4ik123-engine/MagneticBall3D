#include "CopWithPistolShield.h"

namespace MagneticBall3D
{
    CopWithPistolShield::CopWithPistolShield(const char* filePath,
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
        m_UnitType = UnitType::COP_WITH_PISTOL_SHIELD;
        m_attackType = AttackType::RANGE_DAMAGE_ONE;

        m_damage = 2;
        m_attackDistance = 65.0f + Beryll::RandomGenerator::getFloat() * 5.0f;
        m_damageRadius = 0.0f;
        m_lastAttackTime = 0.0f;
        m_timeBetweenAttacks = 1.5f + Beryll::RandomGenerator::getFloat();

        m_garbageAmountToDie = 40;
        m_reducePlayerSpeedWhenDie = 8.0f;

        m_experienceWhenDie = 2;
    }

    CopWithPistolShield::~CopWithPistolShield()
    {

    }
}
