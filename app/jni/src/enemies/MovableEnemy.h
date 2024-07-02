#pragma once

#include "BaseEnemy.h"

namespace MagneticBall3D
{
    class MovableEnemy : public BaseEnemy
    {
    public:
        MovableEnemy() = delete;
        MovableEnemy(const char* filePath,  // Common params.
                      float collisionMassKg,    // Physics params.
                      bool wantCollisionCallBack,
                     Beryll::CollisionFlags collFlag,
                     Beryll::CollisionGroups collGroup,
                     Beryll::CollisionGroups collMask,
                     Beryll::SceneObjectGroups sceneGroup);
        ~MovableEnemy() override;

        void update(const glm::vec3& playerOrigin) override;
        void attack(const glm::vec3& playerOrigin) override;
        void freeStaticPosition() override
        {
            BR_ASSERT(false, "%s", "Dont call freeStaticPosition() for MovableEnemy. It has not StaticPosition.")
        };
        void setPathArray(std::vector<glm::ivec2> pathArray, const int indexInPathArray) override;

    protected:
        void move() override;
    };
}
