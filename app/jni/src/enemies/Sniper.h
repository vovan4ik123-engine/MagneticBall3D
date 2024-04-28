#pragma once

#include "BaseEnemy.h"

namespace MagneticBall3D
{
    struct SniperPosAndRange
    {
        SniperPosAndRange(const glm::vec3& pos, const float range) : position(pos), attackRange(range) {}

        glm::vec3 position{0.0f};
        float attackRange = 0.0f;
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

        void toMakeClassAbstract() override {};

        static std::vector<SniperPosAndRange> sniperPositions;
    };
}
