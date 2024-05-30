#pragma once

#include "BaseEnemy.h"

namespace MagneticBall3D
{
    struct StaticPosition
    {
        StaticPosition(const glm::vec3& pos) : position(pos) {}

        glm::vec3 position{0.0f};
        bool isFreePosition = true;
    };

    class StaticEnemy : public BaseEnemy
    {
    public:
        StaticEnemy() = delete;
        StaticEnemy(const char* filePath,  // Common params.
               float collisionMassKg,    // Physics params.
               bool wantCollisionCallBack,
                    Beryll::CollisionFlags collFlag,
                    Beryll::CollisionGroups collGroup,
                    Beryll::CollisionGroups collMask,
                    Beryll::SceneObjectGroups sceneGroup);
        ~StaticEnemy() override;

        void update(const glm::vec3& playerOrigin) override;
        void attack(const glm::vec3& playerOrigin) override;
        void freeStaticPosition() override;

        static std::vector<StaticPosition> staticPositions;
        static float spawnTime; // Sec.
        static float spawnDelay; // Sec.

    protected:
        void move() override;
    };
}
