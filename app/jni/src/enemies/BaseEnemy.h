#pragma once

#include "EngineHeaders.h"

namespace MagneticBall3D
{
    enum class UnitState
    {
        MOVE, CAN_ATTACK, IDLE
    };

    enum class UnitType
    {
        NONE,
        COP_WITH_PISTOL,
        COP_WITH_GRENADE_LAUNCHER,
        SNIPER
    };

    enum class AttackType
    {
        NONE,
        RANGE_DAMAGE_ONE,
        RANGE_DAMAGE_RADIUS
    };

    class BaseEnemy : public Beryll::AnimatedCollidingCharacter
    {
    public:
        BaseEnemy() = delete;
        BaseEnemy(const char* filePath,  // Common params.
                                   float collisionMassKg,    // Physics params.
                                   bool wantCollisionCallBack,
                  Beryll::CollisionFlags collFlag,
                  Beryll::CollisionGroups collGroup,
                  Beryll::CollisionGroups collMask,
                  Beryll::SceneObjectGroups sceneGroup);
        ~BaseEnemy() override;


        virtual void update(const glm::vec3& playerOrigin) = 0;
        virtual void attack(const glm::vec3& playerOrigin) = 0;
        void enableEnemy();
        void disableEnemy();
        bool getIsEnabled() { return m_isEnabled; }
        static int getActiveCount() { return BaseEnemy::m_activeEnemiesCount; }
        UnitType getUnitType() { return m_UnitType; }
        AttackType getAttackType() { return m_attackType; }
        int getDamage() { return m_damage; }
        float getAttackDistance() { return m_attackDistance; }
        float getDamageRadius() { return m_damageRadius; }
        int getGarbageAmountToDie() { return m_garbageAmountToDie; }
        float getPlayerSpeedReduceWhenDie() { return m_reducePlayerSpeedWhenDie; }
        bool getIsTimeToAttack() { return (m_lastAttackTime + m_timeBetweenAttacks) < Beryll::TimeStep::getSecFromStart(); }
        bool getIsAttacking() { return (m_lastAttackTime + m_timeBetweenAttacks) > Beryll::TimeStep::getSecFromStart(); };

        // Pathfinding.
        int indexInPathArray = 0;
        glm::vec3 currentPointToMove3DFloats{0.0f};
        glm::ivec2 currentPointToMove2DIntegers{0};
        std::vector<glm::ivec2> pathArray; // On XZ plane. INTEGER values.

        UnitState unitState = UnitState::MOVE;

    protected:
        static int m_activeEnemiesCount;
        bool m_isEnabled = false;
        UnitType m_UnitType = UnitType::NONE;
        AttackType m_attackType = AttackType::NONE;

        // Attack data.
        int m_damage = 0;
        float m_attackDistance = 0.0f;
        float m_damageRadius = 0.0f; // Use if unit AttackType::RANGE_DAMAGE_RADIUS.
        float m_lastAttackTime = 0.0f; // Sec.
        float m_timeBetweenAttacks = 0.0f; // Sec.
        float m_delayBeforeFirstAttack = 1.0f; // When entering in attack radius.
        float m_timeEnterInAttackRadius = 0.0f;
        bool m_enterInAttackRadius = false; // When was outside attack radius and enter inside attack radius.

        int m_garbageAmountToDie = 0; // Amount of garbage inside player magnetic radius to kill this unit by collision.
        float m_reducePlayerSpeedWhenDie = 0.0f;
    };
}
