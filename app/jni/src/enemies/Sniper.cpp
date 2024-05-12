#include "Sniper.h"

namespace MagneticBall3D
{
    std::vector<SniperPosAndAttackDist> Sniper::sniperPositions{};
    float Sniper::spawnTime = 0.0f;
    float Sniper::spawnDelay = 20.0f;

    Sniper::Sniper(const char* filePath,
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
        unitState = UnitState::STAND_AIMING;
        m_unitType = UnitType::SNIPER;
        m_attackType = AttackType::RANGE_DAMAGE_ONE;

        damage = 15.0f;
        attackDistance = 0.0f; // Will set outside class. During spawn.
        timeBetweenAttacks = 2.5f + Beryll::RandomGenerator::getFloat() * 0.1f;

        garbageAmountToDie = 10;
        reducePlayerSpeedWhenDie = 10.0f;
        experienceWhenDie = 30;

        m_lastAttackTime = 0.0f;
        m_isCanMove = false;
    }

    Sniper::~Sniper()
    {

    }

    void Sniper::update(const glm::vec3& playerOrigin)
    {
        if(getIsAttacking())
        {
            //BR_INFO("%s", "Sniper is attacking");
            // Do nothing. Attack animation should be playing now.
        }
        else if(getIsDelayBeforeFirstAttack())
        {
            //BR_INFO("%s", "Sniper DelayBeforeFirstAttack");
            unitState = UnitState::STAND_AIMING;
            setCurrentAnimationByIndex(EnAndVars::AnimationIndexes::stand, false, false);
            rotateToPoint(playerOrigin, true);
        }
        else if(glm::distance(m_origin, playerOrigin) < attackDistance)
        {
            //BR_INFO("%s", "Sniper IN_ATTACK_RADIUS");
            unitState = UnitState::IN_ATTACK_RADIUS;

            if(getIsTimeToAttack())
            {
                //BR_INFO("%s", "Sniper if(getIsTimeToAttack())");
                // Check if enemy see player.
                Beryll::RayClosestHit rayBuildingHit = Beryll::Physics::castRayClosestHit(m_origin,
                                                                                          playerOrigin,
                                                                                          Beryll::CollisionGroups::RAY_FOR_BUILDING_CHECK,
                                                                                          Beryll::CollisionGroups::BUILDING);

                if(rayBuildingHit)
                {
                    unitState = UnitState::STAND_AIMING;
                    m_prepareToFirstAttack = true;
                }
                else
                {
                    if(m_prepareToFirstAttack)
                    {
                        //BR_INFO("%s", "Sniper prepareToFirstAttack");
                        m_prepareToFirstAttack = false;
                        m_prepareToFirstAttackStartTime = EnAndVars::mapPlayTimeSec;
                    }
                    else
                    {
                        //BR_INFO("%s", "Sniper CAN_ATTACK");
                        unitState = UnitState::CAN_ATTACK;
                    }
                }
            }
        }
        else
        {
            unitState = UnitState::STAND_AIMING;
            setCurrentAnimationByIndex(EnAndVars::AnimationIndexes::stand, false, false);
            m_prepareToFirstAttack = true;
        }
    }

    void Sniper::freeSniperPosition()
    {
        auto iter = std::find_if(Sniper::sniperPositions.begin(), Sniper::sniperPositions.end(),
                                 [this](SniperPosAndAttackDist& posAndRange) { return posAndRange.position == this->getOrigin(); });
        if(iter != Sniper::sniperPositions.end())
        {
            iter->isFreePosition = true;
            BR_INFO("%s", "Sniper isFreePosition = true.");
        }
    }
}
