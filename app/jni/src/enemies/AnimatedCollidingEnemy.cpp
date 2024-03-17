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
            m_unitState = UnitState::MOVE;
        }
        else
        {
            m_unitState = UnitState::ATTACK;
        }

        if(m_unitState == UnitState::MOVE)
        {
            // Dont use pathArray if enemy see player. Just move all time to player (currentPointToMove3DFloats set outside).
            getController().moveToPosition(currentPointToMove3DFloats, true, true, true);

            if(!getController().getIsMoving())
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
        else if(m_unitState == UnitState::ATTACK)
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
        m_isEnabled = true;
    }

    void AnimatedCollidingEnemy::disableEnemy()
    {
        disableDraw();
        disableUpdate();
        disableCollisionMesh();

        if(AnimatedCollidingEnemy::m_activeEnemiesCount > 0)
            --AnimatedCollidingEnemy::m_activeEnemiesCount;

        m_isEnabled = false;
    }
}
