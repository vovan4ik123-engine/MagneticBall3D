#include "MovableEnemy.h"

namespace MagneticBall3D
{
    MovableEnemy::MovableEnemy(const char* filePath,
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
    }

    MovableEnemy::~MovableEnemy()
    {

    }

    void MovableEnemy::update(const glm::vec3& playerOrigin)
    {
        if(getIsAttacking())
        {
            //BR_INFO("%s", "MovableEnemy is attacking");
            // Do nothing. Attack animation should be playing now.
        }
        else if(getIsDelayBeforeFirstAttack())
        {
            //BR_INFO("%s", "MovableEnemy DelayBeforeFirstAttack");
            unitState = UnitState::STAND_AIMING;
            setCurrentAnimationByIndex(EnAndVars::AnimationIndexes::stand, false, false);
            rotateToPoint(playerOrigin, true);
        }
        else if(glm::distance(m_origin, playerOrigin) > attackDistance)
        {
            //BR_INFO("%s", "MovableEnemy move because distance");
            move();
        }
        else
        {
            //BR_INFO("%s", "MovableEnemy IN_ATTACK_RADIUS");
            unitState = UnitState::IN_ATTACK_RADIUS;

            if(getIsTimeToAttack())
            {
                //BR_INFO("%s", "MovableEnemy if(getIsTimeToAttack())");
                // Check if enemy see player.
                Beryll::RayClosestHit rayBuildingHit = Beryll::Physics::castRayClosestHit(m_origin,
                                                                                          playerOrigin,
                                                                                          Beryll::CollisionGroups::RAY_FOR_BUILDING_CHECK,
                                                                                          Beryll::CollisionGroups::BUILDING);

                if(rayBuildingHit)
                {
                    //BR_INFO("%s", "MovableEnemy move because dont see");
                    move();
                }
                else
                {
                    if(m_prepareToFirstAttack)
                    {
                        //BR_INFO("%s", "MovableEnemy prepareToFirstAttack");
                        m_prepareToFirstAttack = false;
                        m_prepareToFirstAttackStartTime = EnAndVars::mapPlayTimeSec;
                    }
                    else
                    {
                        //BR_INFO("%s", "MovableEnemy CAN_ATTACK");
                        unitState = UnitState::CAN_ATTACK;
                    }
                }
            }
        }
    }

    void MovableEnemy::move()
    {
        unitState = UnitState::MOVE;

        m_prepareToFirstAttack = true;

        setCurrentAnimationByIndex(EnAndVars::AnimationIndexes::run, false, false);

        getController().moveToPosition(currentPointToMove3DFloats, true, true, true);

        if(!getController().getIsMoving())
        {
            //BR_INFO("%s", "MovableEnemy NOT   moving");

            // Go to next point if exists.
            if(indexInPathArray + 1 < pathArray.size())
            {
                ++indexInPathArray;

                currentPointToMove2DIntegers = pathArray[indexInPathArray];
                currentPointToMove3DFloats = glm::vec3(currentPointToMove2DIntegers.x,
                                                       getFromOriginToBottom(),
                                                       currentPointToMove2DIntegers.y);
            }
        }
    }

    void MovableEnemy::attack(const glm::vec3& playerOrigin)
    {
        //BR_INFO("%s", "MovableEnemy::attack()");
        rotateToPoint(playerOrigin, true);
        setCurrentAnimationByIndex(EnAndVars::AnimationIndexes::attack, true, true);
        m_lastAttackTime = EnAndVars::mapPlayTimeSec;
        unitState = UnitState::ATTACKING;
    }
}
