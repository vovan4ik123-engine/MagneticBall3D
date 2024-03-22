#include "BaseEnemy.h"
#include "EnumsAndVariables.h"

namespace MagneticBall3D
{
    int BaseEnemy::m_activeEnemiesCount = 0;

    BaseEnemy::BaseEnemy(const char* filePath,
                         float collisionMassKg,
                         bool wantCollisionCallBack,
                         Beryll::CollisionFlags collFlag,
                         Beryll::CollisionGroups collGroup,
                         Beryll::CollisionGroups collMask,
                         Beryll::SceneObjectGroups sceneGroup)
                                                   : AnimatedCollidingCharacter(filePath,
                                                                                collisionMassKg,
                                                                                wantCollisionCallBack,
                                                                                collFlag,
                                                                                collGroup,
                                                                                collMask,
                                                                                sceneGroup)
    {
        disableEnemy();
    }

    BaseEnemy::~BaseEnemy()
    {

    }

    void BaseEnemy::update(const glm::vec3& playerOrigin)
    {
        if(glm::distance(m_origin, playerOrigin) > m_attackDistance)
        {
            if(getIsDelayBeforeFirstAttack())
            {
                //BR_INFO("%s", "more that attack radius - preparing for first attack");
                unitState = UnitState::IDLE;
                setCurrentAnimationByIndex(EnumsAndVariables::AnimationIndexes::IDLE, false, false);
                rotateToPoint(playerOrigin, true);
            }
            else
            {
                //BR_INFO("%s", "more that attack radius - move");
                m_enterInAttackRadius = false;
                m_timeEnterInAttackRadius = 0.0f;

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
        }
        else if(!m_enterInAttackRadius)
        {
            //BR_INFO("%s", "enter attack radius");
            m_enterInAttackRadius = true;
            m_timeEnterInAttackRadius = Beryll::TimeStep::getSecFromStart();
        }
        else if(m_enterInAttackRadius && !getIsDelayBeforeFirstAttack() && getIsTimeToAttack())
        {
            //BR_INFO("%s", "attack");
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
        else if(unitState == UnitState::ATTACKING && getIsAttacking())
        {
            //BR_INFO("%s", "ATTACKING");
            // Just wait.
        }
        else
        {
            //BR_INFO("%s", "idle");
            unitState = UnitState::IDLE;
            setCurrentAnimationByIndex(EnumsAndVariables::AnimationIndexes::IDLE, false, false);
            rotateToPoint(playerOrigin, true);
        }
    }

    void BaseEnemy::attack(const glm::vec3& playerOrigin)
    {
        rotateToPoint(playerOrigin, true);
        setCurrentAnimationByIndex(EnumsAndVariables::AnimationIndexes::ATTACK, true, true);
        m_lastAttackTime = Beryll::TimeStep::getSecFromStart();
        unitState = UnitState::ATTACKING;
    }

    void BaseEnemy::enableEnemy()
    {
        enableDraw();
        enableUpdate();
        enableCollisionMesh();

        ++BaseEnemy::m_activeEnemiesCount;
        m_isEnabled = true;
    }

    void BaseEnemy::disableEnemy()
    {
        disableDraw();
        disableUpdate();
        disableCollisionMesh();

        if(BaseEnemy::m_activeEnemiesCount > 0)
            --BaseEnemy::m_activeEnemiesCount;

        m_isEnabled = false;
    }
}
