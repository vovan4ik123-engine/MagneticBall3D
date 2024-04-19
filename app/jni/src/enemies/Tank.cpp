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

        m_damage = 5;
        m_attackDistance = 100.0f + Beryll::RandomGenerator::getFloat() * 20.0f;
        m_damageRadius = 10.0f;
        m_lastAttackTime = 0.0f;
        m_timeBetweenAttacks = 4.0f + Beryll::RandomGenerator::getFloat();

        m_garbageAmountToDie = 100;
        m_reducePlayerSpeedWhenDie = 10.0f;
        m_experienceWhenDie = 4;
    }

    Tank::~Tank()
    {

    }
}
