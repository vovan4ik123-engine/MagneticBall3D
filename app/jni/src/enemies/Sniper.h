#pragma once

#include "BaseEnemy.h"

namespace MagneticBall3D
{
    struct SniperPosAndAttackDist
    {
        SniperPosAndAttackDist(const glm::vec3& pos, const float dist) : position(pos), attackDist(dist) {}

        glm::vec3 position{0.0f};
        float attackDist = 0.0f;
        bool isFreePosition = true;
    };

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

        void update(const glm::vec3& playerOrigin) override;
        void freeSniperPosition() override;

        static std::vector<SniperPosAndAttackDist> sniperPositions;
        static float spawnTime; // Sec.
        static float spawnDelay; // Sec
    };
}
