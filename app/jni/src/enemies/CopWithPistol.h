#pragma once

#include "BaseEnemy.h"

namespace MagneticBall3D
{
    class CopWithPistol : public BaseEnemy
    {
    public:
        CopWithPistol() = delete;
        CopWithPistol(const char* filePath,  // Common params.
                      float collisionMassKg,    // Physics params.
                      bool wantCollisionCallBack,
                      Beryll::CollisionFlags collFlag,
                      Beryll::CollisionGroups collGroup,
                      Beryll::CollisionGroups collMask,
                      Beryll::SceneObjectGroups sceneGroup);
        ~CopWithPistol() override;

        void toMakeClassAbstract() override {};
    };
}
