#include "BaseEnemy.h"

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
        //BR_INFO("%s", "BaseEnemy::~BaseEnemy()");
    }

    void BaseEnemy::update(const glm::vec3& playerOrigin)
    {
        if(getIsAttacking())
        {
            //BR_INFO("%s", "is attacking");
            // Do nothing. Attack animation should be playing now.
        }
        else if(getIsDelayBeforeFirstAttack())
        {
            //BR_INFO("%s", "DelayBeforeFirstAttack");
            unitState = UnitState::IDLE;
            setCurrentAnimationByIndex(EnumsAndVariables::AnimationIndexes::stand, false, false);
            rotateToPoint(playerOrigin, true);
        }
        else if(glm::distance(m_origin, playerOrigin) > m_attackDistance)
        {
            //BR_INFO("%s", "move because distance");
            move();
        }
        else
        {
            //BR_INFO("%s", "IN_ATTACK_RADIUS");
            unitState = UnitState::IN_ATTACK_RADIUS;

            if(getIsTimeToAttack())
            {
                //BR_INFO("%s", "if(getIsTimeToAttack())");
                // Check if enemy see player.
                Beryll::RayClosestHit rayAttack = Beryll::Physics::castRayClosestHit(m_origin,
                                                                                     playerOrigin,
                                                                                     Beryll::CollisionGroups::ENEMY_ATTACK,
                                                                                     Beryll::CollisionGroups::BUILDING | Beryll::CollisionGroups::PLAYER);

                if(rayAttack && rayAttack.hittedCollGroup == Beryll::CollisionGroups::PLAYER)
                {
                    if(m_prepareToFirstAttack)
                    {
                        //BR_INFO("%s", " prepareToFirstAttack");
                        m_prepareToFirstAttack = false;
                        m_prepareToFirstAttackStartTime = EnumsAndVariables::mapPlayTimeSec;
                    }
                    else
                    {
                        //BR_INFO("%s", "UnitState::CAN_ATTACK");
                        unitState = UnitState::CAN_ATTACK;
                    }
                }
                else
                {
                    //BR_INFO("%s", "move because dont see");
                    move();
                }
            }
        }
    }

    void BaseEnemy::move()
    {
        unitState = UnitState::MOVE;

        m_prepareToFirstAttack = true;

        setCurrentAnimationByIndex(EnumsAndVariables::AnimationIndexes::run, false, false);

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
                                                       getFromOriginToBottom(),
                                                       currentPointToMove2DIntegers.y);
            }
//            else
//            {
//                BR_INFO("%s", "idle because indexInPathArray >= pathArray.size() - 1");
//                unitState = UnitState::IDLE;
//                setCurrentAnimationByIndex(EnumsAndVariables::AnimationIndexes::stand, false, false);
//            }
        }
    }

    void BaseEnemy::attack(const glm::vec3& playerOrigin)
    {
        //BR_INFO("%s", "BaseEnemy::attack()");
        rotateToPoint(playerOrigin, true);
        setCurrentAnimationByIndex(EnumsAndVariables::AnimationIndexes::attack, true, true);
        m_lastAttackTime = EnumsAndVariables::mapPlayTimeSec;
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
        isSeePlayerOrGarbage = false;
        m_lastAttackTime = 0.0f;
        m_prepareToFirstAttackStartTime = 0.0f;
        m_prepareToFirstAttack = true;
    }
}
