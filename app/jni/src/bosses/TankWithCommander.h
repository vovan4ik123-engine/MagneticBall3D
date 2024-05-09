#pragma once

#include "BaseBoss.h"

namespace MagneticBall3D
{
    class TankWithCommander : public BaseBoss
    {
    public:
        TankWithCommander() = delete;
        TankWithCommander(const char* filePath,  // Common params.
                          float collisionMassKg,    // Physics params.
                          bool wantCollisionCallBack,
                          Beryll::CollisionFlags collFlag,
                          Beryll::CollisionGroups collGroup,
                          Beryll::CollisionGroups collMask,
                          Beryll::SceneObjectGroups sceneGroup,
                          float health);
        ~TankWithCommander() override;
    };
}
