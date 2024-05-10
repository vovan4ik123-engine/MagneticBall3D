#pragma once

#include "EngineHeaders.h"
#include "EnumsAndVariables.h"

namespace MagneticBall3D
{
    class Player
    {
    public:
        Player() = delete;
        Player(std::shared_ptr<Beryll::SimpleCollidingObject> so, float health);
        ~Player();

        void update();

        // Return factor after impulse apply in range 0...1.
        // 1 if full impulse was applied.
        // 0 if nothing was applied.
        float handleScreenSwipe(glm::vec3 swipeForImpulse, glm::vec3 swipeForTorque);
        void reduceSpeed(float speedToReduce);
        float getMoveSpeed() { return m_playerMoveSpeed; }
        float getMoveSpeedXZ() { return m_playerMoveSpeedXZ; }
        const glm::vec3& getMoveDir()
        {
            BR_ASSERT((!glm::isnan(m_playerMoveSpeed) && m_playerMoveSpeed > 0.0f), "%s", "You call getMoveDir() when m_playerMoveSpeed = 0.");
            return m_playerMoveDir;
        }
        const glm::vec3& getMoveDirXZ() { return m_playerMoveDirXZ; }
        bool getIsOnGround() { return m_isOnGround; }
        bool getIsOnBuildingRoof() { return m_isOnBuildingRoof; }
        bool getIsOnBuildingWall() { return m_isOnBuildingWall; }
        int getCollidingBuildingID() { return m_collidingBuildingID; }
        bool getIsOnAir() { return m_isOnAir; }
        bool getIsOnJumpPad() { return m_isOnJumpPad; }
        bool getIsMeteor() { return m_isMeteor; }
        void spamMeteorParticles();

        const std::shared_ptr<Beryll::SimpleCollidingObject>& getObj() { return m_obj; };
        void setObj(std::shared_ptr<Beryll::SimpleCollidingObject> so) { m_obj = std::move(so); }

        void addToExp(int exp) { if(exp > 0) { m_currentLevelExp += (exp * EnumsAndVariables::playerXPMultiplier); } }

        int getCurrentLevelExp() { return m_currentLevelExp; }
        int getCurrentLevelMaxExp() { return m_currentLevelMaxExp; }
        bool getIsNextLevelAchieved() { return m_nextLevelAchieved; }
        void handleLevelAchievement() { m_nextLevelAchieved = false; }

        void setAllModels(std::vector<std::shared_ptr<Beryll::SimpleCollidingObject>> models) { m_allModels = std::move(models); }
        void selectNextModel();
        void addToMaxHP(float value); // Will add to m_maxHP + recalculate m_currentHP.

        float getMaxHP() { return m_maxHP; }
        float getCurrentHP() { return m_currentHP; }
        void takeDamage(float d) { m_currentHP -= (d * EnumsAndVariables::playerDamageTakenMultiplier); }
        bool getIsDie() { return m_currentHP <= 0.0f; }
        void resurrect() { m_currentHP = m_maxHP; }

    private:
        void updateSpeed();

        std::shared_ptr<Beryll::SimpleCollidingObject> m_obj;
        std::vector<std::shared_ptr<Beryll::SimpleCollidingObject>> m_allModels;

        // Move.
        glm::vec3 m_playerMoveDir{0.0f};
        glm::vec3 m_playerMoveDirXZ{0.0f};
        float m_playerMoveSpeed = 0.0f;
        float m_playerMoveSpeedXZ = 0.0f;
        glm::vec3 m_playerLinearVelocity{0.0f};
        glm::vec3 m_playerLinearVelocityXZ{0.0f};

        // Gravity.
        float m_lastTimeOnBuilding = 0.0f; // Sec.
        float m_lastTimeOnGround = 0.0f; // Sec.
        float m_lastTimeOnJumpPad = 0.0f; // Sec.
        const float m_applyAirGravityDelay = 0.5f; // Sec. For player after he stop collide with buildings.
        bool m_isOnGround = false;
        bool m_isOnBuildingRoof = false;
        bool m_isOnBuildingWall = false;
        bool m_isOnAir = false;
        bool m_isOnJumpPad = false;
        int m_collidingBuildingID = 0;

        // Meteor.
        bool m_isMeteor = false;
        uint64_t m_spamMeteorParticlesTime = 0; // Millisec.
        uint64_t m_spamMeteorParticlesDelay = 50; // Millisec.

        // HP.
        float m_maxHP = 0.0f;
        float m_currentHP = 0.0f;

        // Level. XP.
        int m_currentLevel = 0;
        int m_currentLevelExp = 0;
        const std::array<int, 20> m_expPerLevel{100, 200, 300, 400, 500, 600, 700, 800, 900, 1000,
                                                1100, 1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000};

        int m_currentLevelMaxExp = 100;
        bool m_nextLevelAchieved = false;
    };
}
