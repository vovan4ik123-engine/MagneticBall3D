#pragma once

#include "BaseEnemy.h"

namespace MagneticBall3D
{
    class CopWithGrenadeLauncher : public BaseEnemy
    {
    public:
        CopWithGrenadeLauncher() = delete;
        CopWithGrenadeLauncher(const char* filePath,  // Common params.
                               float collisionMassKg,    // Physics params.
                               bool wantCollisionCallBack,
                               Beryll::CollisionFlags collFlag,
                               Beryll::CollisionGroups collGroup,
                               Beryll::CollisionGroups collMask,
                               Beryll::SceneObjectGroups sceneGroup);
        ~CopWithGrenadeLauncher() override;

        void freeSniperPosition() override {};
    };
}
