#include "CopWithGrenadeLauncher.h"

namespace MagneticBall3D
{
    CopWithGrenadeLauncher::CopWithGrenadeLauncher(const char* filePath,
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
        m_UnitType = UnitType::COP_WITH_GRENADE_LAUNCHER;
        m_attackType = AttackType::RANGE_DAMAGE_RADIUS;

        m_damage = 1;
        m_attackDistance = 80.0f + Beryll::RandomGenerator::getFloat() * 10.0f;
        m_damageRadius = 5.0f;
        m_lastAttackTime = 0.0f;
        m_timeBetweenAttacks = 2.0f + Beryll::RandomGenerator::getFloat();

        m_garbageAmountToDie = 50;
        m_reducePlayerSpeedWhenDie = 7.0f;

        m_experienceWhenDie = 2;
    }

    CopWithGrenadeLauncher::~CopWithGrenadeLauncher()
    {

    }
}
