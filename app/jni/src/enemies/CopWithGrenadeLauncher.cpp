#include "CopWithGrenadeLauncher.h"
#include "EnumsAndVariables.h"

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
        m_attackDistance = 35.0f;
        m_damageRadius = 5.0f;
        m_lastAttackTime = 0.0f;
        m_timeBetweenAttacks = 3.0f + Beryll::RandomGenerator::getFloat();

        m_garbageAmountToDie = 50;
        m_reducePlayerSpeedWhenDie = 2.0f;
    }

    CopWithGrenadeLauncher::~CopWithGrenadeLauncher()
    {

    }

    void CopWithGrenadeLauncher::update(const glm::vec3& playerOrigin)
    {
        if(glm::distance(m_origin, playerOrigin) > m_attackDistance && !getIsAttacking())
        {
            unitState = UnitState::MOVE;

            setCurrentAnimationByIndex(EnumsAndVariables::AnimationIndexes::RUN, false, false);

            getController().moveToPosition(currentPointToMove3DFloats, true, true, true);

            if(!getController().getIsMoving())
            {
                //BR_INFO("%s", "NOT   moving");

                // Go to next point if exists.
                if(indexInPathArray < pathArray.size() - 1)
                {
                    ++indexInPathArray;

                    currentPointToMove2DIntegers = pathArray[indexInPathArray];
                    currentPointToMove3DFloats = glm::vec3(currentPointToMove2DIntegers.x,
                                                           getController().getFromOriginToBottom(),
                                                           currentPointToMove2DIntegers.y);
                }
            }
        }
        else if(getIsTimeToAttack())
        {
            // Check if enemy see player.
            Beryll::RayClosestHit rayAttack = Beryll::Physics::castRayClosestHit(m_origin,
                                                                                 playerOrigin,
                                                                                 Beryll::CollisionGroups::ENEMY_ATTACK,
                                                                                 Beryll::CollisionGroups::BUILDING | Beryll::CollisionGroups::PLAYER);

            if(rayAttack && rayAttack.hittedCollGroup == Beryll::CollisionGroups::PLAYER)
            {
                unitState = UnitState::CAN_ATTACK;
            }
        }
        else
        {
            unitState = UnitState::IDLE;
        }
    }

    void CopWithGrenadeLauncher::attack(const glm::vec3& playerOrigin)
    {
        rotateToPoint(playerOrigin, true);
        setCurrentAnimationByIndex(EnumsAndVariables::AnimationIndexes::ATTACK, true, true);
        m_lastAttackTime = Beryll::TimeStep::getSecFromStart();
    }
}
