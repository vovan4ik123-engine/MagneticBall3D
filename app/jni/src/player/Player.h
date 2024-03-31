#pragma once

#include "EngineHeaders.h"

namespace MagneticBall3D
{
    class Player
    {
    public:
        Player() = delete;
        Player(std::shared_ptr<Beryll::SimpleCollidingObject> so, float diam, int health);
        ~Player();


        void updateSpeed();
        void updateGravity();

        // Return factor after impulse apply in range 0...1.
        // 1 if full impulse was applied.
        // 0 if nothing was applied.
        float handleScreenSwipe(const glm::vec3& swipeForImpulse, const glm::vec3& swipeForTorque);
        void reduceSpeed(float speedToReduce);
        float getMoveSpeed() { return m_playerMoveSpeed; }
        const glm::vec3& getMoveDir() { return m_playerMoveDir; }
        bool getIsOnGround() { return m_isOnGround; }
        bool getIsOnBuilding() { return m_isOnBuilding; }
        bool getIsOnAir() { return m_isOnAir; }
        bool getIsMeteor() { return m_isMeteor; }
        void spamMeteorParticles();

        const std::shared_ptr<Beryll::SimpleCollidingObject>& getObj() { return m_obj; };
        void setObj(std::shared_ptr<Beryll::SimpleCollidingObject> so, float diam) { m_obj = std::move(so); m_diameter = diam; }

        int currentHP = 0;

    private:
        std::shared_ptr<Beryll::SimpleCollidingObject> m_obj;
        float m_diameter = 0.0f;
        const int m_maxHP = 0;

        // Move.
        glm::vec3 m_playerMoveDir{0.0f};
        float m_playerMoveSpeed = 0.0f;
        glm::vec3 m_playerLinearVelocity{0.0f};

        // Gravity.
        float m_lastTimeOnBuilding = 0.0f; // Sec.
        const float m_applyAirGravityDelay = 0.6f; // Sec. For player after he stop collide with buildings.
        bool m_isOnGround = false;
        bool m_isOnBuilding = false;
        bool m_isOnAir = false;

        // Meteor.
        bool m_isMeteor = false;
        uint64_t m_spamMeteorParticlesTime = 0; // Millisec.
        uint64_t m_spamMeteorParticlesDelay = 40; // Millisec.

        // Level.
        static constexpr int m_maxLevel = 10;
        int m_currentLevel = 0;
        int m_experienceOnCurrentLevel;
        const std::array<const int, m_maxLevel> m_expNeedPerLevel{10, 12, 14, 16, 18, 20, 22, 24, 26, 28};

    };
}
