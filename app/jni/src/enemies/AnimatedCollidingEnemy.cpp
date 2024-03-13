#include "AnimatedCollidingEnemy.h"

namespace MagneticBall3D
{
    int AnimatedCollidingEnemy::m_activeEnemiesCount = 0;

    AnimatedCollidingEnemy::AnimatedCollidingEnemy(const char* filePath,
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

    AnimatedCollidingEnemy::~AnimatedCollidingEnemy()
    {

    }

    void AnimatedCollidingEnemy::update(const glm::vec3& playerOrigin)
    {
        if(glm::distance(m_origin, playerOrigin) > attackDistance)
        {
            m_state = EnemyState::MOVE;
        }
        else
        {
            m_state = EnemyState::ATTACK;
        }

        if(m_state == EnemyState::MOVE)
        {
            // Dont use pathArray if enemy see player. Just move all time to player (currentPointToMove3DFloats set outside).
            getController().moveToPosition(currentPointToMove3DFloats, true, true, true);

            if(!getController().getIsMoving() && !seePlayer)
            {
                //BR_INFO("%s", "NOT   moving");

                // Go to next point if exists.
                if(indexInPathArray < pathArray.size() - 1)
                {
                    ++indexInPathArray;

                    currentPointToMove2DIntegers = pathArray[indexInPathArray];
                    currentPointToMove3DFloats = glm::vec3(currentPointToMove2DIntegers.x,
                                                           getController().getFromOriginToBottom(),
                                                           currentPointToMove2DIntegers.y);
                }
            }
        }
        else if(m_state == EnemyState::ATTACK)
        {
            // Handle outside class. On scene.
        }
    }

    void AnimatedCollidingEnemy::enableEnemy()
    {
        enableDraw();
        enableUpdate();
        enableCollisionMesh();

        ++AnimatedCollidingEnemy::m_activeEnemiesCount;
    }

    void AnimatedCollidingEnemy::disableEnemy()
    {
        disableDraw();
        disableUpdate();
        disableCollisionMesh();

        if(AnimatedCollidingEnemy::m_activeEnemiesCount > 0)
            --AnimatedCollidingEnemy::m_activeEnemiesCount;
    }
}
