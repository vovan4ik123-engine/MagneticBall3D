#pragma once

#include "EngineHeaders.h"

namespace MagneticBall3D
{
    class Player : public Beryll::SimpleCollidingObject
    {
    public:
        Player() = delete;
        Player(const char* filePath,  // Common params.
               float collisionMassKg,    // Physics params.
               bool wantCollisionCallBack,
               Beryll::CollisionFlags collFlag,
               Beryll::CollisionGroups collGroup,
               Beryll::CollisionGroups collMask,
               Beryll::SceneObjectGroups sceneGroup);
        ~Player();


        
    };
}
