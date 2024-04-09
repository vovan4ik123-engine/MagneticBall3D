#pragma once

#include "BaseEnemy.h"

namespace MagneticBall3D
{
    class CopWithPistolShield : public BaseEnemy
    {
    public:
        CopWithPistolShield() = delete;
        CopWithPistolShield(const char* filePath,  // Common params.
                            float collisionMassKg,    // Physics params.
                            bool wantCollisionCallBack,
                            Beryll::CollisionFlags collFlag,
                            Beryll::CollisionGroups collGroup,
                            Beryll::CollisionGroups collMask,
                            Beryll::SceneObjectGroups sceneGroup);
        ~CopWithPistolShield() override;

        void toMakeClassAbstract() override {};
    };
}
