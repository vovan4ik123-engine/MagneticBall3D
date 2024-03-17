#include "Player.h"

namespace MagneticBall3D
{
    Player::Player(const char* filePath,
                   float collisionMassKg,
                   bool wantCollisionCallBack,
                   Beryll::CollisionFlags collFlag,
                   Beryll::CollisionGroups collGroup,
                   Beryll::CollisionGroups collMask,
                   Beryll::SceneObjectGroups sceneGroup)
                   : SimpleCollidingObject(filePath,
                                           collisionMassKg,
                                           wantCollisionCallBack,
                                           collFlag,
                                           collGroup,
                                           collMask,
                                           sceneGroup)
    {

    }

    Player::~Player()
    {

    }
}
