#include "Player.h"

namespace MagneticBall3D
{
    Player::Player(std::shared_ptr<Beryll::SimpleCollidingObject> so, float health)
    : m_obj(std::move(so)), m_maxHP(health), m_currentHP(health)
    {
        BR_ASSERT((m_expPerLevel.empty() == false), "%s", "m_expPerLevel can not be empty.")

        m_obj->enableCollisionMesh();
        m_obj->enableUpdate();
        m_obj->enableDraw();

        m_currentLevelMaxExp = m_expPerLevel[m_currentLevel];
    }

    Player::~Player()
    {

    }

    void Player::update()
    {
        // Update speed.
        updateSpeed();

        // Update collisions.
        m_isOnGround = false;
        m_isOnBuildingRoof = false;
        m_isOnBuildingWall = false;
        m_isOnAir = false;
        m_isOnJumpPad = false;
        m_touchGroundAfterFall = false;
        m_fallDistance = 0.0f;

        if(Beryll::Physics::getIsCollisionWithGroup(m_obj->getID(), Beryll::CollisionGroups::JUMPPAD))
        {
            if(m_lastTimeOnJumpPad + 1.0f < EnAndVars::mapPlayTimeSec)
            {
                m_obj->setGravity(EnAndVars::playerGravityOnAir);
                m_isOnJumpPad = true;
            }

            m_lastTimeOnJumpPad = EnAndVars::mapPlayTimeSec;
        }
        else if(Beryll::Physics::getIsCollisionWithGroup(m_obj->getID(), Beryll::CollisionGroups::BUILDING))
        {
            bool collisionWithWall = false;
            std::vector<const int> buildingsID = Beryll::Physics::getAllCollisionsForIDWithGroup(m_obj->getID(), Beryll::CollisionGroups::BUILDING);
            m_collidingBuildingID = buildingsID.back();

            std::vector<std::pair<glm::vec3, glm::vec3>> allCollisionPoints = Beryll::Physics::getAllCollisionPoints(m_obj->getID(), buildingsID);
            for(const std::pair<glm::vec3, glm::vec3>& point : allCollisionPoints)
            {
                float buildingNormalAngle = BeryllUtils::Common::getAngleInRadians(point.second, BeryllConstants::worldUp);
                if(buildingNormalAngle > 0.87f && buildingNormalAngle < 2.27f) // > 50 && < 130 degrees.
                {
                    collisionWithWall = true;
                    break;
                }
            }

            if(collisionWithWall)
            {
                m_isOnBuildingWall = true;
                m_lastTimeOnBuildingWall = EnAndVars::mapPlayTimeSec;

                m_obj->setGravity(EnAndVars::playerGravityOnBuildingWall);
                m_obj->setDamping(EnAndVars::playerLinearDamping, EnAndVars::playerAngularDamping);
            }
            else
            {
                m_isOnBuildingRoof = true;

                m_obj->setGravity(EnAndVars::playerGravityOnBuildingRoof);
                m_obj->setDamping(EnAndVars::playerLinearDamping, EnAndVars::playerAngularDamping);
            }

            if(m_isOnBuildingWall &&
               m_lastTimeOnGround + 1.1f < EnAndVars::mapPlayTimeSec &&
               m_lastTimeOnBuilding + 1.1f < EnAndVars::mapPlayTimeSec) // Collision with wall after being in air for 1.1 sec.
            {
                m_obj->resetVelocities();
                m_obj->applyCentralImpulse(BeryllConstants::worldUp * 150.0f);
            }

            m_lastTimeOnBuilding = EnAndVars::mapPlayTimeSec;
        }
        else if(Beryll::Physics::getIsCollisionWithGroup(m_obj->getID(), Beryll::CollisionGroups::GROUND))
        {
            m_obj->setGravity(EnAndVars::playerGravityOnGround);
            m_obj->setDamping(EnAndVars::playerLinearDamping, EnAndVars::playerAngularDamping);

            if(m_falling)
            {
                m_touchGroundAfterFall = true;
                m_fallDistance = glm::distance(m_startFallingHeight, m_obj->getOrigin().y - m_obj->getFromOriginToBottom());
            }

            m_isOnGround = true;
            m_lastTimeOnGround = EnAndVars::mapPlayTimeSec;
        }
        else
        {
            if(m_lastTimeOnBuilding + m_applyAirGravityDelay < EnAndVars::mapPlayTimeSec)
            {
                m_obj->setGravity(EnAndVars::playerGravityOnAir);
                m_obj->setDamping(0.2f, 0.3f);
            }

            m_isOnAir = true;
        }

        if(m_isOnAir && m_previousYPos > m_obj->getOrigin().y) // Ball falling.
        {
            if(!m_startFalling)
            {
                m_startFalling = true;
                m_startFallingHeight = m_obj->getOrigin().y - m_obj->getFromOriginToBottom(); // Bottom Y position.
            }

            m_falling = true;
            m_fallDistance = glm::distance(m_startFallingHeight, m_obj->getOrigin().y - m_obj->getFromOriginToBottom());
        }
        else
        {
            m_falling = false;
            m_startFalling = false;
        }

        m_previousYPos = m_obj->getOrigin().y;

        if(m_isMeteor)
            spamMeteorParticles();

        // Update HP.
        if(m_currentHP <= 0.0f)
        {
            //BR_INFO("%s", "Reset HP");
            m_currentHP = m_maxHP;
        }

        // Update XP.
        if(m_currentLevelExp >= m_currentLevelMaxExp)
        {
            ++m_currentLevel;
            BR_INFO("Got level: %d. Healed for: %f", m_currentLevel, EnAndVars::playerRestoreHPAtNewLevel);
            m_nextLevelAchieved = true; // Achievement should be handled by Improvements class.
            m_currentHP += EnAndVars::playerRestoreHPAtNewLevel;
            if(m_currentHP > m_maxHP)
                m_currentHP = m_maxHP;

            int levelExpOverflow = m_currentLevelExp - m_currentLevelMaxExp;
            m_currentLevelExp = std::max(0, levelExpOverflow);
            BR_INFO("Reset level exp to: %d", m_currentLevelExp);

            if(m_currentLevel < m_expPerLevel.size())
                m_currentLevelMaxExp = m_expPerLevel[m_currentLevel];
            else
                m_currentLevelMaxExp = m_expPerLevel.back();
        }
    }

    void Player::updateSpeed()
    {
        // Prefer check m_playerMoveSpeed > 0.0f before use m_playerMoveDir.
        m_playerLinearVelocity = m_obj->getLinearVelocity();
        m_playerLinearVelocityXZ = glm::vec3(m_playerLinearVelocity.x, 0.0f, m_playerLinearVelocity.z);
        m_playerMoveSpeed = glm::length(m_playerLinearVelocity);
        m_playerMoveSpeedXZ = glm::length(m_playerLinearVelocityXZ);
        if(glm::isnan(m_playerMoveSpeed) || m_playerMoveSpeed == 0.0f)
        {
            m_playerMoveSpeed = 0.0f;
            m_playerMoveSpeedXZ = 0.0f;
            m_playerMoveDir = glm::vec3{0.0f};
            m_playerMoveDirXZ = glm::vec3{0.0f};
        }
        else
        {
            m_playerMoveDir = glm::normalize(m_playerLinearVelocity);
            m_playerMoveDirXZ = glm::normalize(m_playerLinearVelocityXZ);
        }

        EnAndVars::playerCurrentSpeed = m_playerMoveSpeed;

        //BR_INFO("m_playerMoveSpeed %f", m_playerMoveSpeed);

        // Handle meteor.
        if(m_playerMoveSpeed > EnAndVars::playerSpeedForMeteor)
            m_isMeteor = true;
        else
            m_isMeteor = false;
    }

    float Player::handleScreenSwipe(glm::vec3 swipeForImpulse, glm::vec3 swipeForTorque)
    {
        //BR_INFO("Speed before swipe %f", m_playerMoveSpeed);
        // Swipe should control only XZ speed.
        // Y speed controlled by gravity.
        // linear velocity = m_linearVelocity + impulse * m_linearFactor * m_inverseMass;
        glm::vec3 newVelocityXZ = m_playerLinearVelocity + (swipeForImpulse * m_obj->getLinearFactor() * (1.0f / m_obj->getCollisionMass()));
        newVelocityXZ.y = 0.0f;
        float newPlayerSpeedXZ = glm::length(newVelocityXZ);

        float applyImpulseFactor = 1.0f;
        if(newPlayerSpeedXZ > EnAndVars::playerMaxSpeedXZ)
        {
            // Apply less impulse because speed limit was exceeded.
            newVelocityXZ = glm::normalize(newVelocityXZ) * EnAndVars::playerMaxSpeedXZ;

            float newSpeedLimit = std::max(EnAndVars::playerMaxSpeedXZ - m_playerMoveSpeedXZ, 0.0f);
            float newSpeedAdded = newPlayerSpeedXZ - m_playerMoveSpeedXZ;
            applyImpulseFactor = newSpeedLimit / newSpeedAdded;
        }

        newVelocityXZ.y = m_playerLinearVelocity.y;
        m_obj->setLinearVelocity(newVelocityXZ);
        updateSpeed();
        //BR_INFO("Speed after swipe %f", m_playerMoveSpeed);

        if(applyImpulseFactor == 1.0f)
        {
            // Impulse was not enough to reach max speed. We have limit to apply torque.
            // Torque applied along right/left vector from impulse.
            glm::vec3 impulseLeft = glm::cross(BeryllConstants::worldUp, glm::normalize(swipeForImpulse));
            impulseLeft = glm::normalize(impulseLeft) * glm::length(swipeForTorque);
            m_obj->applyTorqueImpulse(impulseLeft);
        }

        return applyImpulseFactor;
    }

    void Player::reduceSpeed(float speedToReduce)
    {
        if(speedToReduce <= 0.0f || EnAndVars::playerSpeedReductionMultiplier <= 0.0f)
            return;

        speedToReduce *= EnAndVars::playerSpeedReductionMultiplier;

        // When player move as meteor his speed will reduced 50% less from normal reduction.
        if(m_isMeteor)
            speedToReduce *= 0.5f;

        if(speedToReduce >= m_playerMoveSpeed)
        {
            m_obj->setLinearVelocity(glm::vec3{0.0f});
        }
        else
        {
            m_obj->setLinearVelocity(m_playerMoveDir * (m_playerMoveSpeed - speedToReduce));
        }

        updateSpeed();
    }

    void Player::spamMeteorParticles()
    {
        if(m_spamMeteorParticlesTime + m_spamMeteorParticlesDelay >= Beryll::TimeStep::getMilliSecFromStart())
            return;

        m_spamMeteorParticlesTime = Beryll::TimeStep::getMilliSecFromStart();

        // Spawn fire before ball.
        float radius = std::max(6.0f, m_obj->getXZRadius());
        glm::vec3 orig{m_obj->getOrigin() + (m_playerMoveDir * radius)};
        float sizeBegin = radius + EnAndVars::garbageCountMagnetized * 0.015f;

        Beryll::ParticleSystem::EmitCubesFromCenter(3, 0.7f, sizeBegin, sizeBegin * 0.3f,
                                                    {0.98f, 0.75f, 0.0f, 0.6f}, {0.5f, 0.066f, 0.0f, 0.0f},
                                                    orig, glm::vec3{0.0f, 100.0f, 0.0f}, 5.0f);
    }

    void Player::selectNextModel()
    {
        for(const auto& item : m_allModels)
        {
            if(item->getXZRadius() > m_obj->getXZRadius())
            {
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

    void Player::addToMaxHP(float value)
    {
        if(value <= 0.0f)
            return;

        float oldMaxHP = m_maxHP;
        float newMaxHP = m_maxHP + value;

        float ratio = newMaxHP / oldMaxHP;

        m_maxHP = m_maxHP * ratio;
        m_currentHP = m_currentHP * ratio;
    }
}
