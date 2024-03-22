#include "Player.h"
#include "EnumsAndVariables.h"

namespace MagneticBall3D
{
    Player::Player(const char* filePath,
                   float collisionMassKg,
                   bool wantCollisionCallBack,
                   Beryll::CollisionFlags collFlag,
                   Beryll::CollisionGroups collGroup,
                   Beryll::CollisionGroups collMask,
                   Beryll::SceneObjectGroups sceneGroup)
                   : SimpleCollidingObject(filePath,
                                           collisionMassKg,
                                           wantCollisionCallBack,
                                           collFlag,
                                           collGroup,
                                           collMask,
                                           sceneGroup)
    {

    }

    Player::~Player()
    {

    }

    void Player::updateSpeed()
    {
        // Prefer check m_playerMoveSpeed > 0.0f before use m_playerMoveDir.
        m_playerLinearVelocity = getLinearVelocity();
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

        if(m_playerMoveSpeed > EnumsAndVariables::playerSpeedForMeteor)
            m_isMeteor = true;
        else
            m_isMeteor = false;
    }

    void Player::updateGravity()
    {
        if(Beryll::Physics::getIsCollisionWithGroup(m_ID, Beryll::CollisionGroups::BUILDING))
        {
            m_isOnGround = false;
            m_isOnBuilding = true;
            m_isOnAir = false;
            m_lastTimeOnBuilding = Beryll::TimeStep::getSecFromStart();
            setGravity(EnumsAndVariables::playerGravityOnBuilding);
        }
        else if(Beryll::Physics::getIsCollisionWithGroup(m_ID, Beryll::CollisionGroups::GROUND))
        {
            m_isOnGround = true;
            m_isOnBuilding = false;
            m_isOnAir = false;
            setGravity(EnumsAndVariables::playerGravityOnGround);
        }
//        else if(Beryll::Physics::getIsCollisionWithGroup(m_player->getID(), Beryll::CollisionGroups::JUMPPAD))
//        {
//            m_player->setGravity(EnumsAndVariables::playerGravityOnAir);
//        }
        else if(m_lastTimeOnBuilding + m_applyGravityDelay < Beryll::TimeStep::getSecFromStart())
        {
            m_isOnGround = false;
            m_isOnBuilding = false;
            m_isOnAir = true;
            setGravity(EnumsAndVariables::playerGravityOnAir);
        }
    }

    float Player::handleScreenSwipe(const glm::vec3& swipeForImpulse, const glm::vec3& swipeForTorque)
    {
        glm::vec3 playerVelocityXZ = m_playerLinearVelocity;
        playerVelocityXZ.y = 0.0f;
        float currentPlayerSpeedXZ = glm::length(playerVelocityXZ);
        BR_INFO("currentPlayerSpeedXZ %f", currentPlayerSpeedXZ);

        // Swipe should control only XZ speed.
        if(currentPlayerSpeedXZ >= EnumsAndVariables::playerMaxSpeedXZ)
            return 0.0f;

        //m_linearVelocity = m_linearVelocity + impulse * m_linearFactor * m_inverseMass;
        glm::vec3 newLinearVelocityXZ = m_playerLinearVelocity + (swipeForImpulse * (1.0f / m_collisionMass));
        newLinearVelocityXZ.y = 0.0f;
        float newPlayerSpeedXZ = glm::length(newLinearVelocityXZ);

        float impulseFactor = 1.0f;
        if(newPlayerSpeedXZ > EnumsAndVariables::playerMaxSpeedXZ)
        {
            // Apply less impulse because speed limit was exceeded.
            float newSpeedLimit = EnumsAndVariables::playerMaxSpeedXZ - currentPlayerSpeedXZ;
            float newSpeedAdded = newPlayerSpeedXZ - currentPlayerSpeedXZ;
            impulseFactor = newSpeedLimit / newSpeedAdded;
        }

        applyCentralImpulse(swipeForImpulse * impulseFactor);
        updateSpeed();

        // We applied less than initial impulse and already reach max speed. Dont need apply torque.
        if(impulseFactor < 1.0f)
            return impulseFactor;

        BR_INFO("%s", "apply torque");
        // Also apply torque on screen swipe.
        // Torque applied along right/left vector from impulse.
        glm::vec3 impulseLeft = glm::cross(BeryllConstants::worldUp, glm::normalize(swipeForTorque));
        impulseLeft = glm::normalize(impulseLeft) * glm::length(swipeForTorque);
        applyTorqueImpulse(impulseLeft);

        return impulseFactor;
    }

    void Player::reduceSpeed(float speedToReduce)
    {
        if(speedToReduce == 0.0f)
            return;

        // When player move as meteor his speed will reduced 50% less from normal reduction.
        // That mean stop meteor is 2 times harder than stop normal player.
        //if(m_isMeteor)
        //    speedToReduce *= 0.5f;

        if(speedToReduce >= m_playerMoveSpeed)
        {
            m_playerMoveSpeed = 0.0f;
            m_playerMoveDir = glm::vec3{0.0f};
            m_playerLinearVelocity = glm::vec3{0.0f};

            setLinearVelocity(m_playerLinearVelocity);
        }
        else
        {
            m_playerMoveSpeed = m_playerMoveSpeed - speedToReduce;
            // m_playerMoveDir = glm::normalize(m_playerLinearVelocity); should be same.
            m_playerLinearVelocity = m_playerMoveDir * m_playerMoveSpeed;

            setLinearVelocity(m_playerLinearVelocity);
        }
    }
}
