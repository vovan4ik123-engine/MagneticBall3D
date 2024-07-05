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
                         Beryll::SceneObjectGroups sceneGroup)
                         : AnimatedCollidingCharacter(filePath,
                                                      collisionMassKg,
                                                      wantCollisionCallBack,
                                                      collFlag,
                                                      collGroup,
                                                      collMask,
                                                      sceneGroup)
    {
        disableEnemy();
    }

    BaseEnemy::~BaseEnemy()
    {
        //BR_INFO("%s", "BaseEnemy::~BaseEnemy()");
        disableEnemy();
    }

    void BaseEnemy::enableEnemy()
    {
        enableDraw();
        enableUpdate();
        enableCollisionMesh();

        ++BaseEnemy::m_activeEnemiesCount;
        m_isEnabled = true;
    }

    void BaseEnemy::disableEnemy()
    {
        disableDraw();
        disableUpdate();
        disableCollisionMesh();

        if(BaseEnemy::m_activeEnemiesCount > 0)
            --BaseEnemy::m_activeEnemiesCount;

        m_isEnabled = false;
        m_lastAttackTime = -9999.0f;
        m_prepareToFirstAttackStartTime = -9999.0f;
        m_prepareToFirstAttack = true;
    }
}
