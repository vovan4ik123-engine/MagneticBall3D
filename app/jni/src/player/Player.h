#pragma once

#include "EngineHeaders.h"

namespace MagneticBall3D
{
    class Player : public Beryll::SimpleCollidingObject
    {
    public:
        Player() = delete;
        Player(const char* filePath,  // Common params.
               float collisionMassKg,    // Physics params.
               bool wantCollisionCallBack,
               Beryll::CollisionFlags collFlag,
               Beryll::CollisionGroups collGroup,
               Beryll::CollisionGroups collMask,
               Beryll::SceneObjectGroups sceneGroup);
        ~Player();


        void updateSpeed();
        void updateGravity();

        // Return factor after impulse apply in range 0...1.
        // 1 if full impulse was applied.
        // 0 if nothing was applied.
        float handleScreenSwipe(const glm::vec3& swipeForImpulse, const glm::vec3& swipeForTorque);
        void reduceSpeed(const float speedToReduce);
        float getMoveSpeed() { return m_playerMoveSpeed; }
        const glm::vec3& getMoveDir() { return m_playerMoveDir; }

    private:
        glm::vec3 m_playerMoveDir{0.0f};
        float m_playerMoveSpeed{0.0f};
        glm::vec3 m_playerLinearVelocity{0.0f};
        float m_lastTimeOnBuilding = 0.0f; // Sec.
        const float m_applyGravityDelay = 0.25f; // Sec. For player after he stop collide with buildings.
    };
}
