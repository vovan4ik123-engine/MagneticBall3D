#pragma once

#include "EngineHeaders.h"
#include "EnumsAndVariables.h"

namespace MagneticBall3D
{
    enum class UnitState
    {
        MOVE, IN_ATTACK_RADIUS, IDLE, ATTACKING, CAN_ATTACK
    };

    enum class UnitType
    {
        NONE,
        COP_WITH_PISTOL,
        COP_WITH_PISTOL_SHIELD,
        COP_WITH_GRENADE_LAUNCHER,
        SNIPER,
        TANK,
        BOSS_TANK
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


        virtual void toMakeClassAbstract() = 0;

        // Good fit for movable range enemies. For other types you will probably override these two methods.
        virtual void update(const glm::vec3& playerOrigin);
        virtual void attack(const glm::vec3& playerOrigin);

        void enableEnemy();
        void disableEnemy();
        bool getIsEnabled() { return m_isEnabled; }
        static int getActiveCount() { return BaseEnemy::m_activeEnemiesCount; }
        UnitType getUnitType() { return m_UnitType; }
        AttackType getAttackType() { return m_attackType; }
        float getDamage() { return m_damage; }
        float getAttackDistance() { return m_attackDistance; }
        float getDamageRadius() { return m_damageRadius; }
        int getGarbageAmountToDie() { return m_garbageAmountToDie; }
        float getPlayerSpeedReduceWhenDie() { return m_reducePlayerSpeedWhenDie; }
        bool getIsTimeToAttack() { return (m_lastAttackTime + m_timeBetweenAttacks) < EnumsAndVariables::mapPlayTimeSec; }
        bool getIsAttacking() { return (m_lastAttackTime + m_timeBetweenAttacks) > EnumsAndVariables::mapPlayTimeSec; }
        bool getIsDelayBeforeFirstAttack() { return (m_prepareToFirstAttackStartTime + m_timeBetweenAttacks) > EnumsAndVariables::mapPlayTimeSec; }
        int getExperienceWhenDie() { return m_experienceWhenDie; }

        // Pathfinding.
        int indexInPathArray = 0;
        glm::vec3 currentPointToMove3DFloats{0.0f};
        glm::ivec2 currentPointToMove2DIntegers{0};
        std::vector<glm::ivec2> pathArray; // On XZ plane. INTEGER values.
        bool getIsCanMove() { return m_isCanMove; }

        UnitState unitState = UnitState::MOVE;
        bool isSeePlayerOrGarbage = false;

        bool isCanBeSpawned = false;

    protected:
        void move();

        static int m_activeEnemiesCount;
        bool m_isEnabled = false;
        UnitType m_UnitType = UnitType::NONE;
        AttackType m_attackType = AttackType::NONE;
        bool m_isCanMove = true; // false for SNIPER.

        // Attack data.
        float m_damage = 0.0f;
        float m_attackDistance = 10.0f;
        float m_damageRadius = 0.0f; // Use if unit AttackType::RANGE_DAMAGE_RADIUS.
        float m_lastAttackTime = 0.0f; // Sec.
        float m_timeBetweenAttacks = 0.0f; // Sec.
        float m_prepareToFirstAttackStartTime = 0.0f;
        bool m_prepareToFirstAttack = true; // When was outside attack radius and enter inside attack radius.

        // Death.
        int m_garbageAmountToDie = 0; // Amount of garbage inside player magnetic radius to kill this unit by collision.
        float m_reducePlayerSpeedWhenDie = 0.0f;
        int m_experienceWhenDie = 0;
    };
}
