#pragma once

#include "EngineHeaders.h"

namespace MagneticBall3D
{
    enum class EnemyState
    {
        MOVE, ATTACK
    };

    class AnimatedCollidingEnemy : public Beryll::AnimatedCollidingCharacter
    {
    public:
        AnimatedCollidingEnemy() = delete;
        AnimatedCollidingEnemy(const char* filePath,  // Common params.
                                   float collisionMassKg,    // Physics params.
                                   bool wantCollisionCallBack,
                                   Beryll::CollisionFlags collFlag,
                                   Beryll::CollisionGroups collGroup,
                                   Beryll::CollisionGroups collMask,
                                   Beryll::SceneObjectGroups sceneGroup);
        ~AnimatedCollidingEnemy() override;


        void update(const glm::vec3& playerOrigin);
        EnemyState getState() { return m_state; }
        void enableEnemy();
        void disableEnemy();
        static int getActiveCount() { return AnimatedCollidingEnemy::m_activeEnemiesCount; }

        int indexInPathArray = 0;
        glm::vec3 currentPointToMove3DFloats{0.0f};
        glm::ivec2 currentPointToMove2DIntegers{0};
        std::vector<glm::ivec2> pathArray; // On XZ plane. INTEGER values.
        float attackDistance = 0.0f;
        bool seePlayer = true;

    private:
        EnemyState m_state = EnemyState::MOVE;
        static int m_activeEnemiesCount;
    };
}
