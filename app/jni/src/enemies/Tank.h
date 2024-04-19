#pragma once

#include "BaseEnemy.h"

namespace MagneticBall3D
{
    class Tank : public BaseEnemy
    {
    public:
        Tank() = delete;
        Tank(const char* filePath,  // Common params.
             float collisionMassKg,    // Physics params.
             bool wantCollisionCallBack,
             Beryll::CollisionFlags collFlag,
             Beryll::CollisionGroups collGroup,
             Beryll::CollisionGroups collMask,
             Beryll::SceneObjectGroups sceneGroup);
        ~Tank() override;

        void toMakeClassAbstract() override {};
    };
}
