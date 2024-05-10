#include "TankWithCommander.h"

namespace MagneticBall3D
{
    TankWithCommander::TankWithCommander(const char* filePath,
                                         float collisionMassKg,
                                         bool wantCollisionCallBack,
                                         Beryll::CollisionFlags collFlag,
                                         Beryll::CollisionGroups collGroup,
                                         Beryll::CollisionGroups collMask,
                                         Beryll::SceneObjectGroups sceneGroup,
                                         float health)
                                         : BaseBoss(filePath,
                                                    collisionMassKg,
                                                    wantCollisionCallBack,
                                                    collFlag,
                                                    collGroup,
                                                    collMask,
                                                    sceneGroup,
                                                    health)
    {
        bossState = BossState::MOVE;
        m_bossType = BossType::TANK_WITH_COMMANDER;

        damage = 0.0f;
        attackDistance = 400.0f;
        timeBetweenAttacks = 3.0f;

        m_takeDamageDelay = 3.0f;

        getController().moveSpeed = 50.0f;

        m_lastAttackTime = 0.0f;
    }

    TankWithCommander::~TankWithCommander()
    {

    }
}
