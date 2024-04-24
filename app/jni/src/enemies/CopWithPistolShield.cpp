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

        m_damage = 5.0f;
        m_attackDistance = 70.0f + Beryll::RandomGenerator::getFloat() * 5.0f;
        m_timeBetweenAttacks = 1.5f + Beryll::RandomGenerator::getFloat() * 0.1f;

        m_garbageAmountToDie = 40;
        m_reducePlayerSpeedWhenDie = 12.0f;
        m_experienceWhenDie = 10;
        getController().moveSpeed = 20.0f;

        m_lastAttackTime = 0.0f;
    }

    CopWithPistolShield::~CopWithPistolShield()
    {

    }
}
