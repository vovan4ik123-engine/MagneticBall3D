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

        damage = 5.0f;
        attackDistance = 130.0f + Beryll::RandomGenerator::getFloat() * 120.0f;
        damageRadius = 5.0f;
        timeBetweenAttacks = 2.0f + Beryll::RandomGenerator::getFloat() * 0.1f;

        garbageAmountToDie = 20;
        reducePlayerSpeedWhenDie = 8.0f;
        experienceWhenDie = 40;
        getController().moveSpeed = 20.0f;

        m_lastAttackTime = 0.0f;
    }

    CopWithGrenadeLauncher::~CopWithGrenadeLauncher()
    {

    }
}
