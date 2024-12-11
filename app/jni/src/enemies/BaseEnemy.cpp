#include "BaseEnemy.h"

namespace MagneticBall3D
{
    int BaseEnemy::m_activeEnemiesCount = 0;

    BaseEnemy::BaseEnemy(const char* filePath,
                         float collisionMassKg,
                         bool wantCollisionCallBack,
                         Beryll::CollisionFlags collFlag,
                         Beryll::CollisionGroups collGroup,
                         Beryll::CollisionGroups collMask,
                         Beryll::SceneObjectGroups sceneGroup,
                         float health)
                         : AnimatedCollidingCharacter(filePath,
                                                      collisionMassKg,
                                                      wantCollisionCallBack,
                                                      collFlag,
                                                      collGroup,
                                                      collMask,
                                                      sceneGroup),
                         m_maxHP(health), m_currentHP(health)
    {
        m_isEnabled = true;
        ++BaseEnemy::m_activeEnemiesCount;
        disableEnemy();
    }

    BaseEnemy::~BaseEnemy()
    {
        //BR_INFO("%s", "BaseEnemy::~BaseEnemy()");
        disableEnemy();
    }

    void BaseEnemy::enableEnemy()
    {
        if(m_isEnabled)
            return;

        enableDraw();
        enableUpdate();
        enableCollisionMesh();

        ++BaseEnemy::m_activeEnemiesCount;
        m_isEnabled = true;
        m_currentHP = m_maxHP;
    }

    void BaseEnemy::disableEnemy()
    {
        if(!m_isEnabled)
            return;

        disableDraw();
        disableUpdate();
        disableCollisionMesh();

        m_lastAttackTime = -999999.0f;
        m_prepareToFirstAttackStartTime = -999999.0f;
        m_prepareToFirstAttack = true;
        m_smashDamageLastTimeApplied = -999999.0f;
        m_shotDamageLastTimeApplied = -999999.0f;

        //if(BaseEnemy::m_activeEnemiesCount > 0)
            --BaseEnemy::m_activeEnemiesCount;
        m_isEnabled = false;
    }

    void BaseEnemy::attack(const glm::vec3& playerOrigin)
    {
        //BR_INFO("%s", "BaseEnemy::attack()");
        rotateToPoint(playerOrigin, true);
        setCurrentAnimationByIndex(EnumsAndVars::AnimationIndexes::attack, true, true);
        m_lastAttackTime = EnumsAndVars::mapPlayTimeSec;
        unitState = UnitState::ATTACKING;

        Sounds::playSoundEffect(attackSound);
        Sounds::playSoundEffect(attackHitSound);
    }

    bool BaseEnemy::takeSmashDamage(const float d)
    {
        if(m_smashDamageLastTimeApplied + m_smashDamageApplyDelay < EnumsAndVars::mapPlayTimeSec)
        {
            m_currentHP -= d;
            m_smashDamageLastTimeApplied = EnumsAndVars::mapPlayTimeSec;
            return true;
        }

        return false;
    }

    void BaseEnemy::takeShotDamage(const float d)
    {
        m_currentHP -= d;
        m_shotDamageLastTimeApplied = EnumsAndVars::mapPlayTimeSec;
    }
}
