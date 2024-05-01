#include "Tank.h"

namespace MagneticBall3D
{
    Tank::Tank(const char* filePath,
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
        m_UnitType = UnitType::TANK;
        m_attackType = AttackType::RANGE_DAMAGE_RADIUS;

        damage = 20.0f;
        attackDistance = 200.0f + Beryll::RandomGenerator::getFloat() * 300.0f;
        damageRadius = 10.0f;
        timeBetweenAttacks = 2.5f + Beryll::RandomGenerator::getFloat() * 0.1f;

        garbageAmountToDie = 20;
        reducePlayerSpeedWhenDie = 30.0f;
        experienceWhenDie = 100;
        getController().moveSpeed = 15.0f;

        m_lastAttackTime = 0.0f;
    }

    Tank::~Tank()
    {

    }
}
