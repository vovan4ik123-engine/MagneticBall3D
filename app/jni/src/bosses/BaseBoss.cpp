#include "BaseBoss.h"

namespace MagneticBall3D
{
    BaseBoss::BaseBoss(const char* filePath,
                       float collisionMassKg,
                       bool wantCollisionCallBack,
                       Beryll::CollisionFlags collFlag,
                       Beryll::CollisionGroups collGroup,
                       Beryll::CollisionGroups collMask,
                       Beryll::SceneObjectGroups sceneGroup,
                       float health)
                       : AnimatedCollidingCharacter(filePath,
                                                    collisionMassKg,
                                                    wantCollisionCallBack,
                                                    collFlag,
                                                    collGroup,
                                                    collMask,
                                                    sceneGroup)
    {
        m_maxHP = health;
        m_currentHP = health;

        disableBoss();
    }

    BaseBoss::~BaseBoss()
    {
        disableBoss();
    }

    void BaseBoss::update(const glm::vec3& playerOrigin)
    {
        if(getIsAttacking())
        {
            //BR_INFO("%s", "BaseBoss is attacking");
            // Do nothing. Attack animation should be playing now.
            rotateToPoint(playerOrigin, true);
        }
        else if(getIsDelayBeforeFirstAttack())
        {
            //BR_INFO("%s", "BaseBoss DelayBeforeFirstAttack");
            bossState = BossState::STAND_AIMING;
            setCurrentAnimationByIndex(EnumsAndVars::AnimationIndexes::stand, false, false);
            rotateToPoint(playerOrigin, true);
        }
        else if(glm::distance(m_origin, playerOrigin) > attackDistance)
        {
            //BR_INFO("%s", "BaseBoss move because distance");
            move();
        }
        else
        {
            //BR_INFO("%s", "BaseBoss IN_ATTACK_RADIUS");
            bossState = BossState::IN_ATTACK_RADIUS;

            if(getIsTimeToAttack())
            {
                //BR_INFO("%s", "BaseBoss if(getIsTimeToAttack())");
                // Check if enemy see player.
                Beryll::RayClosestHit rayEnv = Beryll::Physics::castRayClosestHit(m_origin,
                                                                                  playerOrigin,
                                                                                  Beryll::CollisionGroups::RAY_FOR_ENVIRONMENT,
                                                                                  Beryll::CollisionGroups::BUILDING | Beryll::CollisionGroups::GROUND);

                if(rayEnv)
                {
                    //BR_INFO("%s", "BaseBoss move because dont see");
                    move();
                }
                else
                {
                    if(m_prepareToFirstAttack)
                    {
                        //BR_INFO("%s", "BaseBoss prepareToFirstAttack");
                        m_prepareToFirstAttack = false;
                        m_prepareToFirstAttackStartTime = EnumsAndVars::mapPlayTimeSec;
                    }
                    else
                    {
                        //BR_INFO("%s", "BaseBoss CAN_ATTACK");
                        bossState = BossState::CAN_ATTACK;
                    }
                }
            }
        }
    }

    void BaseBoss::move()
    {
        bossState = BossState::MOVE;

        m_prepareToFirstAttack = true;

        setCurrentAnimationByIndex(EnumsAndVars::AnimationIndexes::run, false, false);

        getController().moveToPosition(currentPointToMove3DFloats, true, true, true);

        if(!getController().getIsMoving())
        {
            //BR_INFO("%s", "NOT   moving");

            // Go to next point if exists.
            if(pathArrayIndexToMove + 1 < pathArray.size())
            {
                ++pathArrayIndexToMove;

                currentPointToMove2DIntegers = pathArray[pathArrayIndexToMove];
                currentPointToMove3DFloats = glm::vec3(currentPointToMove2DIntegers.x,
                                                       getFromOriginToBottom(),
                                                       currentPointToMove2DIntegers.y);
            }
        }
    }

    void BaseBoss::attack(const glm::vec3& playerOrigin)
    {
        //BR_INFO("%s", "BaseBoss::attack()");
        rotateToPoint(playerOrigin, true);
        setCurrentAnimationByIndex(EnumsAndVars::AnimationIndexes::attack, true, true);
        m_lastAttackTime = EnumsAndVars::mapPlayTimeSec;
        bossState = BossState::ATTACKING;
    }

    void BaseBoss::enableBoss()
    {
        enableDraw();
        enableUpdate();
        enableCollisionMesh();
    }

    void BaseBoss::disableBoss()
    {
        disableDraw();
        disableUpdate();
        disableCollisionMesh();

        m_isEnabled = false;
        m_lastAttackTime = -9999.0f;
        m_prepareToFirstAttackStartTime = -9999.0f;
        m_prepareToFirstAttack = true;
    }
}
