#include "Player.h"

namespace MagneticBall3D
{
    Player::Player(std::shared_ptr<Beryll::SimpleCollidingObject> so, float health)
    : m_obj(std::move(so)), m_maxHP(health), m_currentHP(health)
    {
        BR_ASSERT((m_expPerLevel.empty() == false), "%s", "m_expPerLevel can not be empty.");

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
        m_isOnBuilding = false;
        m_isOnAir = false;
        m_isOnJumpPad = false;
        m_touchGroundAfterFall = false;
        m_fallDistance = 0.0f;

        if(Beryll::Physics::getIsCollisionWithGroup(m_obj->getID(), Beryll::CollisionGroups::JUMPPAD))
        {
            if(m_lastTimeOnJumpPad + 1.0f < EnumsAndVars::mapPlayTimeSec)
            {
                m_obj->setGravity(EnumsAndVars::playerGravityOnAir);
                m_obj->setDamping(EnumsAndVars::playerLinearDamping, EnumsAndVars::playerAngularDamping);
                m_isOnJumpPad = true;
            }

            m_lastTimeOnJumpPad = EnumsAndVars::mapPlayTimeSec;
        }
        else if(Beryll::Physics::getIsCollisionWithGroup(m_obj->getID(), Beryll::CollisionGroups::BUILDING))
        {
            std::vector<const int> buildingsID = Beryll::Physics::getAllCollisionsForIDWithGroup(m_obj->getID(), Beryll::CollisionGroups::BUILDING);
            m_buildingCollisionID = buildingsID.back();

            std::vector<std::pair<glm::vec3, glm::vec3>> allCollisionPoints = Beryll::Physics::getAllCollisionPoints(m_obj->getID(), buildingsID);
            for(const std::pair<glm::vec3, glm::vec3>& point : allCollisionPoints)
            {
                // First store any normal.
                m_buildingCollisionNormal = point.second;
                // Next check if that normal of vertical surface.
                m_buildingNormalAngle = BeryllUtils::Common::getAngleInRadians(m_buildingCollisionNormal, BeryllConstants::worldUp);
                if(m_buildingNormalAngle > glm::half_pi<float>())
                    m_buildingNormalAngle = BeryllUtils::Common::getAngleInRadians(m_buildingCollisionNormal, -BeryllConstants::worldUp);

                if(m_buildingNormalAngle > 1.396f) // > 80 degrees. Consider it like vertical surface (wall).
                    // Keep normal of vertical surface (wall) if we have. They have more priority among all surfaces.
                    break;
            }

            // buildingNormalAngle = glm::half_pi<float>() if player on vertical wall. angleFactor will = 1.
            // buildingNormalAngle = 0 if player on horizontal roof. angleFactor will = 0.
            const float angleFactor = m_buildingNormalAngle / glm::half_pi<float>();

            const glm::vec3 gravityDiff = EnumsAndVars::playerGravityOnBuildingRoof - EnumsAndVars::playerGravityOnBuildingWall;
            const glm::vec3 newGravity = EnumsAndVars::playerGravityOnBuildingRoof - (gravityDiff * angleFactor);
            m_obj->setGravity(newGravity);
            m_obj->setDamping(EnumsAndVars::playerLinearDamping, EnumsAndVars::playerAngularDamping);

            if(m_buildingNormalAngle > 1.48f && m_buildingNormalAngle < 1.658f && // > 85 && < 95 degrees.
               m_lastTimeOnGround + 1.0f < EnumsAndVars::mapPlayTimeSec &&
               m_lastTimeOnBuilding + 1.0f < EnumsAndVars::mapPlayTimeSec) // Collision with wall after being in air for 1.0 sec.
            {
                BR_INFO(" %s", "Player resetVelocities()");
                m_obj->resetVelocities();
                m_obj->applyCentralImpulse(BeryllConstants::worldUp * 80.0f);
            }

            m_isOnBuilding = true;
            m_lastTimeOnBuilding = EnumsAndVars::mapPlayTimeSec;
        }
        else if(Beryll::Physics::getIsCollisionWithGroup(m_obj->getID(), Beryll::CollisionGroups::GROUND))
        {
            m_obj->setGravity(EnumsAndVars::playerGravityOnGround);
            m_obj->setDamping(EnumsAndVars::playerLinearDamping, EnumsAndVars::playerAngularDamping);

            if(m_falling)
            {
                m_touchGroundAfterFall = true;
                m_fallDistance = glm::distance(m_startFallingHeight, m_obj->getOrigin().y - m_obj->getFromOriginToBottom());
            }

            m_isOnGround = true;
            m_lastTimeOnGround = EnumsAndVars::mapPlayTimeSec;
        }
        else
        {
            if(m_lastTimeOnBuilding + m_applyAirGravityDelay < EnumsAndVars::mapPlayTimeSec)
                m_obj->setGravity(EnumsAndVars::playerGravityOnAir);

            m_obj->setDamping(EnumsAndVars::playerLinearDamping, 0.5f);

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

        if(m_isMeteor && EnumsAndVars::SettingsMenu::meteorParticles)
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
            BR_INFO("Got level: %d. Healed for: %f", m_currentLevel, EnumsAndVars::playerRestoreHPAtNewLevel);
            m_nextLevelAchieved = true; // Achievement should be handled by Improvements class.
            m_currentHP += EnumsAndVars::playerRestoreHPAtNewLevel;
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
            m_playerMoveDir = glm::vec3{0.0f};
        }
        else
        {
            m_playerMoveDir = glm::normalize(m_playerLinearVelocity);
        }

        if(glm::isnan(m_playerMoveSpeedXZ) || m_playerMoveSpeedXZ == 0.0f)
        {
            m_playerMoveSpeedXZ = 0.0f;
            m_playerMoveDirXZ = glm::vec3{0.0f};
        }
        else
        {
            m_playerMoveDirXZ = glm::normalize(m_playerLinearVelocityXZ);
        }

        EnumsAndVars::playerCurrentSpeed = m_playerMoveSpeed;

        //BR_INFO("m_playerMoveSpeed %f", m_playerMoveSpeed);

        // Handle meteor.
        if(m_playerMoveSpeed > EnumsAndVars::playerSpeedForMeteor)
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
        const float speedLimitXZ = EnumsAndVars::playerMaxSpeedXZ + 1.0f;
        if(newPlayerSpeedXZ > speedLimitXZ)
        {
            // Apply less impulse because speed limit was exceeded.
            newVelocityXZ = glm::normalize(newVelocityXZ) * speedLimitXZ;

            float newSpeedLimit = std::max(speedLimitXZ - m_playerMoveSpeedXZ, 0.0f);
            float newSpeedAdded = newPlayerSpeedXZ - m_playerMoveSpeedXZ;
            applyImpulseFactor = newSpeedLimit / newSpeedAdded;
        }

        newVelocityXZ.y = m_playerLinearVelocity.y;

        if(glm::any(glm::isnan(newVelocityXZ)))
            return 0;

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
        if(m_playerMoveSpeed < 1.0f || speedToReduce <= 0.0f || EnumsAndVars::playerSpeedReductionMultiplier <= 0.0f)
            return;

        speedToReduce *= EnumsAndVars::playerSpeedReductionMultiplier;

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
        float sizeBegin = radius + EnumsAndVars::garbageCountMagnetized * 0.015f;

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
