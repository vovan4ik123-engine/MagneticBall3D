#include "Player.h"
#include "EnumsAndVariables.h"

namespace MagneticBall3D
{
    Player::Player(std::shared_ptr<Beryll::SimpleCollidingObject> so, int health)
    : m_obj(std::move(so)), maxHP(health), currentHP(health)
    {
        m_obj->enableCollisionMesh();
        m_obj->enableUpdate();
        m_obj->enableDraw();

        //BR_ASSERT((m_maxLevel == m_expPerLevel.size()), "%s", "m_maxLevel != m_expNeedPerLevel.size()");
    }

    Player::~Player()
    {

    }

    void Player::update()
    {
        // Update speed.
        updateSpeed();

        // Update gravity.
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

        // Update HP.
        if(currentHP <= 0)
        {
            BR_INFO("%s", "Reset HP");
            currentHP = maxHP;
        }

        // Update XP.
        if(m_currentLevel < m_maxLevel && m_currentLevelExp >= m_currentLevelMaxExp)
        {
            ++m_currentLevel;
            BR_INFO("Got exp limit. Handle level: %d", m_currentLevel);
            m_nextLevelAchieved = true; // Achievement should be handled by Improvements class.

            if(m_currentLevel < m_maxLevel)
            {
                int levelExpOverflow = m_currentLevelExp - m_currentLevelMaxExp;
                m_currentLevelExp = std::max(0, levelExpOverflow);
                BR_INFO("Reset level exp to: %d", m_currentLevelExp);
            }
        }
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

    float Player::handleScreenSwipe(glm::vec3 swipeForImpulse, glm::vec3 swipeForTorque)
    {
        BR_INFO("before swipe m_playerMoveSpeed %f", m_playerMoveSpeed);
        // Swipe should control only XZ speed.
        // Y speed controlled by gravity.
        glm::vec3 currentVelocityXZ = m_playerLinearVelocity;
        currentVelocityXZ.y = 0.0f;
        float currentPlayerSpeedXZ = glm::length(currentVelocityXZ);

        //m_linearVelocity = m_linearVelocity + impulse * m_linearFactor * m_inverseMass;
        glm::vec3 newVelocityXZ = m_playerLinearVelocity + (swipeForImpulse * (1.0f / m_obj->getCollisionMass()));
        newVelocityXZ.y = 0.0f;
        float newPlayerSpeedXZ = glm::length(newVelocityXZ);

        float impulseReduceFactor = 1.0f;
        if(newPlayerSpeedXZ > EnumsAndVariables::playerMaxSpeedXZ)
        {
            // Apply less impulse because speed limit was exceeded.
            newVelocityXZ = glm::normalize(newVelocityXZ) * EnumsAndVariables::playerMaxSpeedXZ;

            float newSpeedLimit = std::max(EnumsAndVariables::playerMaxSpeedXZ - currentPlayerSpeedXZ, 0.0f);
            float newSpeedAdded = newPlayerSpeedXZ - currentPlayerSpeedXZ;
            impulseReduceFactor = newSpeedLimit / newSpeedAdded;
        }

        newVelocityXZ.y = m_playerLinearVelocity.y;
        m_obj->setLinearVelocity(newVelocityXZ);
        updateSpeed();

        if(impulseReduceFactor == 1.0f)
        {
            // Impulse was not enough to reach max speed. We have limit to apply torque.
            // Torque applied along right/left vector from impulse.
            glm::vec3 impulseLeft = glm::cross(BeryllConstants::worldUp, glm::normalize(swipeForImpulse));
            impulseLeft = glm::normalize(impulseLeft) * glm::length(swipeForTorque);
            m_obj->applyTorqueImpulse(impulseLeft);
        }

        return impulseReduceFactor;
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
        float diameter = m_obj->getXZRadius() * 2.0f;
        glm::vec3 orig{m_obj->getOrigin() + (m_playerMoveDir * diameter)};
        float sizeBegin = diameter + EnumsAndVariables::garbageCountMagnetized * 0.014f;

        Beryll::ParticleSystem::EmitCubesFromCenter(5, 1, sizeBegin, sizeBegin * 0.5f,
                                                    {0.98f, 0.75f, 0.0f, 1.0f}, {0.5f, 0.066f, 0.0f, 0.0f},
                                                    orig, glm::vec3{0.0f, 100.0f, 0.0f}, 5.0f);
    }

    void Player::selectNextModel()
    {
        for(const auto& item : m_allModels)
        {
            if(item->getXZRadius() > m_obj->getXZRadius())
            {
                BR_INFO("Set radius %f", item->getXZRadius());
                item->enableCollisionMesh();
                item->enableUpdate();
                item->enableDraw();
                item->setOrigin(m_obj->getOrigin());
                item->setLinearVelocity(m_obj->getLinearVelocity());
                item->setAngularVelocity(m_obj->getAngularVelocity());

                m_obj->disableForEver();
                m_obj = item;

                break;
            }
        }
    }
}
