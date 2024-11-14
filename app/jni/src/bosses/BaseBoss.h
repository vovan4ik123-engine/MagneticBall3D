#pragma once

#include "EngineHeaders.h"
#include "EnumsAndVariables.h"

namespace MagneticBall3D
{
    enum class BossState
    {
        MOVE, IN_ATTACK_RADIUS, STAND_AIMING, ATTACKING, CAN_ATTACK
    };

    enum class BossType
    {
        NONE,
        TANK_WITH_COMMANDER
    };

    class BaseBoss : public Beryll::AnimatedCollidingCharacter
    {
    public:
        BaseBoss() = delete;
        BaseBoss(const char* filePath,  // Common params.
                 float collisionMassKg,    // Physics params.
                 bool wantCollisionCallBack,
                 Beryll::CollisionFlags collFlag,
                 Beryll::CollisionGroups collGroup,
                 Beryll::CollisionGroups collMask,
                 Beryll::SceneObjectGroups sceneGroup,
                 float health);
        ~BaseBoss() override;

        virtual void update(const glm::vec3& playerOrigin);
        virtual void attack(const glm::vec3& playerOrigin);

        void enableBoss();
        void disableBoss();

        bool getIsEnabled() { return m_isEnabled; }
        BossType getBossType() { return m_bossType; }
        bool getIsCanMove() { return m_isCanMove; }
        bool getIsTimeToAttack() { return (m_lastAttackTime + timeBetweenAttacks) < EnumsAndVars::mapPlayTimeSec; }
        bool getIsAttacking() { return (m_lastAttackTime + timeBetweenAttacks) > EnumsAndVars::mapPlayTimeSec; }
        bool getIsDelayBeforeFirstAttack() { return (m_prepareToFirstAttackStartTime + timeBetweenAttacks) > EnumsAndVars::mapPlayTimeSec; }

        float getMaxHP() { return m_maxHP; }
        float getCurrentHP() { return m_currentHP; }
        void takeDamage(const float d)
        {
            if(m_takeDamageTime + m_takeDamageDelay < EnumsAndVars::mapPlayTimeSec)
            {
                m_takeDamageTime = EnumsAndVars::mapPlayTimeSec;
                m_currentHP -= d;
            }
        }
        bool getIsDie() { return m_currentHP <= 0.0f; }

        // Pathfinding.
        std::vector<glm::ivec2> pathArray; // On XZ plane. INTEGER values.
        int pathArrayIndexToMove = 0;
        glm::vec3 currentPointToMove3DFloats{0.0f};
        glm::ivec2 currentPointToMove2DIntegers{0};

        BossState bossState = BossState::MOVE;

        float damage = 0.0f;
        float attackDistance = 10.0f;
        float timeBetweenAttacks = 0.0f; // Sec.

    protected:
        void move();

        bool m_isEnabled = true;
        BossType m_bossType = BossType::NONE;
        bool m_isCanMove = true;

        // Attack data.
        float m_lastAttackTime = -999999.0f; // Sec.
        float m_prepareToFirstAttackStartTime = -999999.0f;
        bool m_prepareToFirstAttack = true; // When was outside attack radius and enter inside attack radius.

        // HP.
        float m_maxHP = 0.0f;
        float m_currentHP = 0.0f;

        // Take damage.
        float m_takeDamageTime = 0.0f;
        float m_takeDamageDelay = 0.0f;
    };
}
