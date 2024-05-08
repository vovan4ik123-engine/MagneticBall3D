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
        m_unitType = UnitType::COP_WITH_PISTOL_SHIELD;
        m_attackType = AttackType::RANGE_DAMAGE_ONE;

        damage = 7.0f;
        attackDistance = 70.0f + Beryll::RandomGenerator::getFloat() * 50.0f;
        timeBetweenAttacks = 1.5f + Beryll::RandomGenerator::getFloat() * 0.1f;

        garbageAmountToDie = 10;
        reducePlayerSpeedWhenDie = 12.0f;
        experienceWhenDie = 20;
        getController().moveSpeed = 20.0f;

        m_lastAttackTime = 0.0f;
    }

    CopWithPistolShield::~CopWithPistolShield()
    {

    }
}
