#pragma once

#include "BaseEnemy.h"

namespace MagneticBall3D
{
    class Sniper : public BaseEnemy
    {
    public:
        Sniper() = delete;
        Sniper(const char* filePath,  // Common params.
               float collisionMassKg,    // Physics params.
               bool wantCollisionCallBack,
               Beryll::CollisionFlags collFlag,
               Beryll::CollisionGroups collGroup,
               Beryll::CollisionGroups collMask,
               Beryll::SceneObjectGroups sceneGroup);
        ~Sniper() override;

        void toMakeClassAbstract() override {};
    };
}
