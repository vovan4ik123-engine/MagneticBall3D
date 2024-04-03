#include "Player.h"
#include "EnumsAndVariables.h"

namespace MagneticBall3D
{
    Player::Player(std::shared_ptr<Beryll::SimpleCollidingObject> so, float diam, int health)
    : m_obj(std::move(so)), m_diameter(diam), m_maxHP(health)
    {

    }

    Player::~Player()
    {

    }

    void Player::updateSpeed()
    {
        // Prefer check m_playerMoveSpeed > 0.0f before use m_playerMoveDir.
        m_playerLinearVelocity = m_obj->getLinearVelocity();
        m_playerMoveSpeed = glm::length(m_playerLinearVelocity);
        if(glm::isnan(m_playerMoveSpeed) || m_playerMoveSpeed == 0.0f)
        {
            m_playerMoveSpeed = 0.0f;
            m_playerMoveDir = glm::vec3{0.0f};
        }
        else
        {
            m_playerMoveDir = glm::normalize(m_playerLinearVelocity);
        }

        // Handle meteor.
//        if(m_playerMoveSpeed > EnumsAndVariables::playerSpeedForMeteor)
//            m_isMeteor = true;
//        else
//            m_isMeteor = false;
    }

    void Player::updateGravity()
    {
        if(Beryll::Physics::getIsCollisionWithGroup(m_obj->getID(), Beryll::CollisionGroups::BUILDING))
        {
            m_isOnGround = false;
            m_isOnBuilding = true;
            m_isOnAir = false;
            m_lastTimeOnBuilding = Beryll::TimeStep::getSecFromStart();
            m_obj->setGravity(EnumsAndVariables::playerGravityOnBuilding);
        }
        else if(Beryll::Physics::getIsCollisionWithGroup(m_obj->getID(), Beryll::CollisionGroups::GROUND))
        {
            m_isOnGround = true;
            m_isOnBuilding = false;
            m_isOnAir = false;
            m_obj->setGravity(EnumsAndVariables::playerGravityOnGround);
        }
//        else if(Beryll::Physics::getIsCollisionWithGroup(m_player->getID(), Beryll::CollisionGroups::JUMPPAD))
//        {
//            m_player->setGravity(EnumsAndVariables::playerGravityOnAir);
//        }
        else if(m_lastTimeOnBuilding + m_applyAirGravityDelay < Beryll::TimeStep::getSecFromStart())
        {
            m_isOnGround = false;
            m_isOnBuilding = false;
            m_isOnAir = true;
            m_obj->setGravity(EnumsAndVariables::playerGravityOnAir);
        }
    }

    float Player::handleScreenSwipe(glm::vec3 swipeForImpulse, const glm::vec3& swipeForTorque)
    {
        glm::vec3 playerVelocityXZ = m_playerLinearVelocity;
        playerVelocityXZ.y = 0.0f;
        float currentPlayerSpeedXZ = glm::length(playerVelocityXZ);
        //BR_INFO("currentPlayerSpeedXZ %f", currentPlayerSpeedXZ);

        // Swipe should control only XZ speed.
        //if(currentPlayerSpeedXZ >= EnumsAndVariables::playerMaxSpeedXZ)
        //    return 0.0f;

        float impulseFirstLength = glm::length(swipeForImpulse);

        if(m_playerMoveSpeed > 1.0f && m_isOnGround)
        {
            float angleMoveToImpulse = BeryllUtils::Common::getAngleInRadians(m_playerMoveDir, glm::normalize(swipeForImpulse));
            //BR_INFO("angleMoveToImpulse %f", angleMoveToImpulse);
            swipeForImpulse += swipeForImpulse * (angleMoveToImpulse * 2.4f);
        }

        //m_linearVelocity = m_linearVelocity + impulse * m_linearFactor * m_inverseMass;
        glm::vec3 newLinearVelocityXZ = m_playerLinearVelocity + (swipeForImpulse * (1.0f / m_obj->getCollisionMass()));
        newLinearVelocityXZ.y = 0.0f;
        float newPlayerSpeedXZ = glm::length(newLinearVelocityXZ);

        float impulseReduceFactor = 1.0f;
        if(newPlayerSpeedXZ > EnumsAndVariables::playerMaxSpeedXZ)
        {
            // Apply less impulse because speed limit was exceeded.
            float newSpeedLimit = EnumsAndVariables::playerMaxSpeedXZ - currentPlayerSpeedXZ;
            float newSpeedAdded = newPlayerSpeedXZ - currentPlayerSpeedXZ;
            impulseReduceFactor = newSpeedLimit / newSpeedAdded;
            swipeForImpulse *= impulseReduceFactor;
        }

        float impulseLastLength = glm::length(swipeForImpulse);
        m_obj->applyCentralImpulse(swipeForImpulse);
        updateSpeed();

        // We applied less than initial impulse and already reach max speed. Dont need apply torque.
        if(impulseReduceFactor >= 1.0f)
        {
            //BR_INFO("%s", "apply torque");
            // Also apply torque on screen swipe.
            // Torque applied along right/left vector from impulse.
            glm::vec3 impulseLeft = glm::cross(BeryllConstants::worldUp, glm::normalize(swipeForTorque));
            impulseLeft = glm::normalize(impulseLeft) * glm::length(swipeForTorque);
            m_obj->applyTorqueImpulse(impulseLeft);
        }

        return impulseLastLength / impulseFirstLength;
    }

    void Player::reduceSpeed(float speedToReduce)
    {
        if(speedToReduce <= 0.0f)
            return;

        // When player move as meteor his speed will reduced 50% less from normal reduction.
        // That mean stop meteor is 2 times harder than stop normal player.
        if(m_isMeteor)
            speedToReduce *= 0.5f;

        if(speedToReduce >= m_playerMoveSpeed)
        {
            m_playerMoveSpeed = 0.0f;
            m_playerMoveDir = glm::vec3{0.0f};
            m_playerLinearVelocity = glm::vec3{0.0f};

            m_obj->setLinearVelocity(m_playerLinearVelocity);
        }
        else
        {
            m_playerMoveSpeed = m_playerMoveSpeed - speedToReduce;
            // m_playerMoveDir = glm::normalize(m_playerLinearVelocity); should be same.
            m_playerLinearVelocity = m_playerMoveDir * m_playerMoveSpeed;

            m_obj->setLinearVelocity(m_playerLinearVelocity);
        }
    }

    void Player::spamMeteorParticles()
    {
        if(m_spamMeteorParticlesTime + m_spamMeteorParticlesDelay >= Beryll::TimeStep::getMilliSecFromStart())
            return;

        m_spamMeteorParticlesTime = Beryll::TimeStep::getMilliSecFromStart();

        // Spawn fire before ball.
        glm::vec3 orig{m_obj->getOrigin() + (m_playerMoveDir * m_diameter)};
        float sizeBegin = m_diameter + EnumsAndVariables::garbageCountMagnetized * 0.014f;

        Beryll::ParticleSystem::EmitCubesFromCenter(5, 1, sizeBegin, sizeBegin * 0.5f,
                                                    {0.98f, 0.75f, 0.0f, 1.0f}, {0.5f, 0.066f, 0.0f, 0.0f},
                                                    orig, glm::vec3{0.0f, 100.0f, 0.0f}, 5.0f);
    }
}
