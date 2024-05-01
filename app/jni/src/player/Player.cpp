#include "Player.h"

namespace MagneticBall3D
{
    Player::Player(std::shared_ptr<Beryll::SimpleCollidingObject> so, float health)
    : m_obj(std::move(so)), m_maxHP(health), m_currentHP(health)
    {
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

        // Update gravity.
        if(Beryll::Physics::getIsCollisionWithGroup(m_obj->getID(), Beryll::CollisionGroups::JUMPPAD))
        {
            if(m_lastTimeOnJumpPad + 1.0f < EnumsAndVariables::mapPlayTimeSec)
            {
                m_obj->setGravity(EnumsAndVariables::playerGravityOnAir);
                m_isOnJumpPad = true;
            }
            else
            {
                m_isOnJumpPad = false;
            }

            m_isOnGround = false;
            m_isOnBuildingRoof = false;
            m_isOnBuildingWall = false;
            m_isOnAir = false;
            m_lastTimeOnJumpPad = EnumsAndVariables::mapPlayTimeSec;
        }
        else if(Beryll::Physics::getIsCollisionWithGroup(m_obj->getID(), Beryll::CollisionGroups::BUILDING))
        {
            bool collisionWithWall = false;
            std::vector<const int> buildingsID = Beryll::Physics::getAllCollisionsForIDWithGroup(m_obj->getID(), Beryll::CollisionGroups::BUILDING);

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
                m_isOnBuildingRoof = false;
                m_isOnBuildingWall = true;

                m_obj->setGravity(EnumsAndVariables::playerGravityOnBuildingWall);
                m_obj->setDamping(EnumsAndVariables::playerLinearDamping, EnumsAndVariables::playerAngularDamping);
            }
            else
            {
                m_isOnBuildingRoof = true;
                m_isOnBuildingWall = false;

                m_obj->setGravity(EnumsAndVariables::playerGravityOnBuildingRoof);
                m_obj->setDamping(EnumsAndVariables::playerLinearDamping, EnumsAndVariables::playerAngularDamping);
            }

            if(m_isOnBuildingWall &&
               m_lastTimeOnGround + 0.8f < EnumsAndVariables::mapPlayTimeSec &&
               m_lastTimeOnBuilding + 0.8f < EnumsAndVariables::mapPlayTimeSec) // Collision with wall after being in air for 0.85 sec.
            {
                m_obj->resetVelocities();
                m_obj->applyCentralImpulse(BeryllConstants::worldUp * 200.0f);
            }

            m_isOnGround = false;
            m_isOnAir = false;
            m_isOnJumpPad = false;
            m_lastTimeOnBuilding = EnumsAndVariables::mapPlayTimeSec;
        }
        else if(Beryll::Physics::getIsCollisionWithGroup(m_obj->getID(), Beryll::CollisionGroups::GROUND))
        {
            if(!m_isOnGround)
            {
                m_obj->setGravity(EnumsAndVariables::playerGravityOnGround);
                m_obj->setDamping(EnumsAndVariables::playerLinearDamping, EnumsAndVariables::playerAngularDamping);
            }

            m_isOnGround = true;
            m_isOnBuildingRoof = false;
            m_isOnBuildingWall = false;
            m_isOnAir = false;
            m_isOnJumpPad = false;
            m_lastTimeOnGround = EnumsAndVariables::mapPlayTimeSec;
        }
        else
        {
            if(m_lastTimeOnBuilding + m_applyAirGravityDelay < EnumsAndVariables::mapPlayTimeSec)
            {
                m_obj->setGravity(EnumsAndVariables::playerGravityOnAir);
                m_obj->setDamping(EnumsAndVariables::playerLinearDamping, 0.3f);
            }

            m_isOnGround = false;
            m_isOnBuildingRoof = false;
            m_isOnBuildingWall = false;
            m_isOnAir = true;
            m_isOnJumpPad = false;
        }

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
            BR_INFO("Got level: %d. Healed for: %f", m_currentLevel, EnumsAndVariables::playerRestoreHPAtNewLevel);
            m_nextLevelAchieved = true; // Achievement should be handled by Improvements class.
            m_currentHP += EnumsAndVariables::playerRestoreHPAtNewLevel;
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

        // Handle meteor.
        if(m_playerMoveSpeed > EnumsAndVariables::playerSpeedForMeteor)
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
        glm::vec3 newVelocityXZ = m_playerLinearVelocity + (swipeForImpulse * (1.0f / m_obj->getCollisionMass()));
        newVelocityXZ.y = 0.0f;
        float newPlayerSpeedXZ = glm::length(newVelocityXZ);

        float impulseReduceFactor = 1.0f;
        if(newPlayerSpeedXZ > EnumsAndVariables::playerMaxSpeedXZ)
        {
            // Apply less impulse because speed limit was exceeded.
            newVelocityXZ = glm::normalize(newVelocityXZ) * EnumsAndVariables::playerMaxSpeedXZ;

            float newSpeedLimit = std::max(EnumsAndVariables::playerMaxSpeedXZ - m_playerMoveSpeedXZ, 0.0f);
            float newSpeedAdded = newPlayerSpeedXZ - m_playerMoveSpeedXZ;
            impulseReduceFactor = newSpeedLimit / newSpeedAdded;
        }

        newVelocityXZ.y = m_playerLinearVelocity.y;
        m_obj->setLinearVelocity(newVelocityXZ);
        updateSpeed();
        //BR_INFO("Speed after swipe %f", m_playerMoveSpeed);

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

        speedToReduce *= EnumsAndVariables::playerSpeedReductionMultiplier;

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
        float radius = std::max(4.0f, m_obj->getXZRadius());
        glm::vec3 orig{m_obj->getOrigin() + (m_playerMoveDir * radius)};
        float sizeBegin = radius + EnumsAndVariables::garbageCountMagnetized * 0.017f;

        Beryll::ParticleSystem::EmitCubesFromCenter(5, 1, sizeBegin, sizeBegin * 0.3f,
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
