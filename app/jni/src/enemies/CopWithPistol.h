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

        void update(const glm::vec3& playerOrigin) override;
        void attack(const glm::vec3& playerOrigin) override;
    };
}
