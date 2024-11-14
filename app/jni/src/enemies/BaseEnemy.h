#pragma once

#include "EngineHeaders.h"
#include "EnumsAndVariables.h"
#include "Sounds.h"
#include "garbage/Garbage.h"

namespace MagneticBall3D
{
    enum class UnitState
    {
        MOVE, IN_ATTACK_RADIUS, STAND_AIMING, ATTACKING, CAN_ATTACK
    };

    enum class UnitType
    {
        NONE,
        ENEMY_MELEE,
        ENEMY_GUN1,
        ENEMY_GUN2,
        ENEMY_GUN_SHIELD,
        ENEMY_SIT_WHEN_SHOOT,
        ENEMY_SNIPER,
        ENEMY_TANK,
        ENEMY_MAGNET
    };

    enum class AttackType
    {
        NONE,
        MELEE_DAMAGE_ONE,
        RANGE_DAMAGE_ONE,
        RANGE_DAMAGE_RADIUS,
        MAGNETIZE_GARBAGE,
        IMPULSE_UP_GARBAGE,
        DISAPPEAR_GARBAGE
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
                  Beryll::SceneObjectGroups sceneGroup,
                  float health);
        ~BaseEnemy() override;
        
        virtual void update(const glm::vec3& playerOrigin) = 0;
        void attack(const glm::vec3& playerOrigin);
        virtual void freeStaticPosition() = 0; // Implement for StaticEnemy.
        virtual void setPathArray(std::vector<glm::ivec2> pathArray, const int indexToMove) = 0; // Implement for MovableEnemy.

        void enableEnemy();
        void disableEnemy();
        bool getIsEnabled() { return m_isEnabled; }
        bool getIsCanMove() { return m_isCanMove; }
        static int getActiveCount() { return BaseEnemy::m_activeEnemiesCount; }
        bool getIsTimeToAttack() { return (m_lastAttackTime + timeBetweenAttacks) < EnumsAndVars::mapPlayTimeSec; }
        bool getIsAttacking() { return (m_lastAttackTime + timeBetweenAttacks) > EnumsAndVars::mapPlayTimeSec; }
        bool getIsDelayBeforeFirstAttack() { return (m_prepareToFirstAttackStartTime + timeBetweenAttacks) > EnumsAndVars::mapPlayTimeSec; }

        float getMaxHP() { return m_maxHP; }
        float getCurrentHP() { return m_currentHP; }
        bool getIsNeedShowHPBar()
        {
            return m_smashDamageLastTimeApplied + 2.0f > EnumsAndVars::mapPlayTimeSec ||
                   m_shotDamageLastTimeApplied + 2.0f > EnumsAndVars::mapPlayTimeSec;
        }
        bool takeSmashDamage(const float d);
        void takeShotDamage(const float d);

        bool castRayToFindYPos = false;
        glm::ivec2 getCurrentPointToMove2DInt() { return m_currentPointToMove2DIntegers; };
        const glm::vec3& getStartPointMoveFrom() { return m_startPointMoveFrom; }

        UnitState unitState = UnitState::MOVE;
        UnitType unitType = UnitType::NONE;
        AttackType attackType = AttackType::NONE;
        SoundType attackSound = SoundType::NONE;
        SoundType attackHitSound = SoundType::NONE;
        glm::vec3 attackParticlesColor{0.5f};
        float attackParticlesSize = 0.2f;
        SoundType dieSound = SoundType::NONE;
        GarbageType dieGarbageType = GarbageType::NONE; // Spawn garbage when die.

        bool isCanBeSpawned = false;

        float damage = 0.0f;
        float attackDistance = 10.0f;
        float damageRadius = 0.0f; // Use if unit AttackType::RANGE_DAMAGE_RADIUS.
        float timeBetweenAttacks = 0.0f; // Sec.

        float reducePlayerSpeedWhenTakeSmashDamage = 0.0f;
        int experienceWhenDie = 0;

    protected:
        virtual void move() = 0;
        virtual void die() = 0;

        bool m_isEnabled = true;
        bool m_isCanMove = true; // False for StaticUnit.

        // Hp.
        const float m_maxHP = 0.0f;
        float m_currentHP = 0.0f;
        float m_smashDamageLastTimeApplied = -999999.0f;
        float m_shotDamageLastTimeApplied = -999999.0f;
        const float m_smashDamageApplyDelay = 1.0f; // Smash damage can be applied once per sec.

        // Attack data.
        float m_lastAttackTime = -999999.0f; // Sec.
        float m_prepareToFirstAttackStartTime = -999999.0f;
        bool m_prepareToFirstAttack = true; // When was outside attack radius and enter inside attack radius.

        // Pathfinding.
        std::vector<glm::ivec2> m_pathArray; // On XZ plane. INTEGER values.
        int m_pathArrayIndexToMove = 0;
        glm::ivec2 m_currentPointToMove2DIntegers{0};
        glm::vec3 m_currentPointToMove3DFloats{0.0f};
        glm::vec3 m_startPointMoveFrom{0.0f};

    private:
        static int m_activeEnemiesCount;
    };
}
