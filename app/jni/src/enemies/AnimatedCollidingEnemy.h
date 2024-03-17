#pragma once

#include "EngineHeaders.h"

namespace MagneticBall3D
{
    enum class UnitState
    {
        MOVE, ATTACK, IDLE
    };

    enum class UnitType
    {
        NONE, MOVING_UNIT, STATIC_UNIT
    };

    enum class AttackType
    {
        NONE,
        RANGE_DAMAGE_ONE,
        RANGE_DAMAGE_RADIUS
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
        void enableEnemy();
        void disableEnemy();
        bool getIsEnabled() { return m_isEnabled; }
        static int getActiveCount() { return AnimatedCollidingEnemy::m_activeEnemiesCount; }
        UnitState getUnitState() { return m_unitState; }
        UnitType getUnitType() { return m_UnitType; }

        int indexInPathArray = 0;
        glm::vec3 currentPointToMove3DFloats{0.0f};
        glm::ivec2 currentPointToMove2DIntegers{0};
        std::vector<glm::ivec2> pathArray; // On XZ plane. INTEGER values.
        float attackDistance = 0.0f;
        float damageRadius = 0.0f; // Use if unit AttackType::RANGE_DAMAGE_RADIUS
        bool seePlayer = true;

    private:
        static int m_activeEnemiesCount;
        bool m_isEnabled = false;
        UnitState m_unitState = UnitState::MOVE;
        UnitType m_UnitType = UnitType::NONE; // Initialise in subclass ???
        AttackType m_attackType = AttackType::NONE;
    };
}
