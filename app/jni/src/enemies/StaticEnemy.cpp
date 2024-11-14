#include "StaticEnemy.h"

namespace MagneticBall3D
{
    std::vector<StaticPosition> StaticEnemy::staticPositions{};
    float StaticEnemy::spawnTime = -999999.0f;
    float StaticEnemy::spawnDelay = 20.0f;

    StaticEnemy::StaticEnemy(const char* filePath,
                             float collisionMassKg,
                             bool wantCollisionCallBack,
                             Beryll::CollisionFlags collFlag,
                             Beryll::CollisionGroups collGroup,
                             Beryll::CollisionGroups collMask,
                             Beryll::SceneObjectGroups sceneGroup,
                             float health)
                             : BaseEnemy(filePath,
                                         collisionMassKg,
                                         wantCollisionCallBack,
                                         collFlag,
                                         collGroup,
                                         collMask,
                                         sceneGroup,
                                         health)
    {
        unitState = UnitState::STAND_AIMING;
        m_isCanMove = false;
    }

    StaticEnemy::~StaticEnemy()
    {

    }

    void StaticEnemy::update(const glm::vec3& playerOrigin)
    {
        if(m_currentHP <= 0.0f)
        {
            die();
            return;
        }

        if(getIsAttacking())
        {
            //BR_INFO("%s", "StaticEnemy is attacking");
            // Do nothing. Attack animation should be playing now.
        }
        else if(getIsDelayBeforeFirstAttack())
        {
            //BR_INFO("%s", "StaticEnemy DelayBeforeFirstAttack");
            unitState = UnitState::STAND_AIMING;
            setCurrentAnimationByIndex(EnumsAndVars::AnimationIndexes::stand, false, false, true);
            rotateToPoint(playerOrigin, true);
        }
        else if(glm::distance(m_origin, playerOrigin) < attackDistance)
        {
            //BR_INFO("%s", "StaticEnemy IN_ATTACK_RADIUS");
            unitState = UnitState::IN_ATTACK_RADIUS;

            if(getIsTimeToAttack())
            {
                //BR_INFO("%s", "StaticEnemy if(getIsTimeToAttack())");
                // Check if enemy see player.
                Beryll::RayClosestHit rayEnv = Beryll::Physics::castRayClosestHit(m_origin,
                                                                                  playerOrigin,
                                                                                  Beryll::CollisionGroups::RAY_FOR_ENVIRONMENT,
                                                                                  Beryll::CollisionGroups::BUILDING | Beryll::CollisionGroups::GROUND);

                if(rayEnv)
                {
                    unitState = UnitState::STAND_AIMING;
                    m_prepareToFirstAttack = true;
                }
                else
                {
                    if(m_prepareToFirstAttack)
                    {
                        //BR_INFO("%s", "StaticEnemy prepareToFirstAttack");
                        m_prepareToFirstAttack = false;
                        m_prepareToFirstAttackStartTime = EnumsAndVars::mapPlayTimeSec;
                    }
                    else
                    {
                        //BR_INFO("%s", "StaticEnemy CAN_ATTACK");
                        unitState = UnitState::CAN_ATTACK;
                    }
                }
            }
        }
        else
        {
            unitState = UnitState::STAND_AIMING;
            setCurrentAnimationByIndex(EnumsAndVars::AnimationIndexes::stand, false, false, true);
            m_prepareToFirstAttack = true;
        }
    }

    void StaticEnemy::move()
    {
        // Does not need for StaticEnemy.
    }

    void StaticEnemy::die()
    {
        Sounds::playSoundEffect(dieSound);
        disableEnemy();
        freeStaticPosition();
    }

    void StaticEnemy::freeStaticPosition()
    {
        auto iter = std::find_if(StaticEnemy::staticPositions.begin(), StaticEnemy::staticPositions.end(),
                                 [this](StaticPosition& posAndRange) { return posAndRange.position == this->getOrigin(); });
        if(iter != StaticEnemy::staticPositions.end())
        {
            iter->isFreePosition = true;
            BR_INFO("%s", "StaticEnemy isFreePosition = true.");
        }
    }
}
