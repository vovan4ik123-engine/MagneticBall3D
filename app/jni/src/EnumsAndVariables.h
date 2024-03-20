#pragma once

namespace EnumsAndVariables
{
    struct FontsPath
    {
        static const inline std::string ROBOTO = "fonts/roboto.ttf";
        static const inline std::string CREAMY = "fonts/creamy.ttf";
        static const inline std::string COUSINE = "fonts/cousine.ttf";
    };

    struct AnimationIndexes
    {
        static const inline int ATTACK = 0;
        static const inline int IDLE = 2;
        static const inline int RUN = 3;
    };

    // Camera.
    constexpr inline float cameraZoomMaxSpeed = 30.0f; // Meters in sec.
    constexpr inline float minPlayerSpeedToCameraFollow = 5.0f;
    constexpr inline float cameraYAccordingPlayerY = 0.025f; // Multiply by player.y and add to m_cameraOffset.y.

    // Player.
    constexpr inline float playerMagneticRadius = 35.0f;
    constexpr inline float playerImpulseFactor = 0.25f;
    constexpr inline float playerTorqueFactor = 0.5f;
    constexpr inline float playerDamping = 0.2f;
    constexpr inline float playerFriction = 80.0f;
    constexpr inline float playerMass = 10.0f;
    constexpr inline glm::vec3 playerGravityOnAir{0.0f, -60.0f, 0.0f};
    constexpr inline glm::vec3 playerGravityOnGround{0.0f, -25.0f, 0.0f};
    constexpr inline glm::vec3 playerGravityOnBuilding{0.0f, -4.0f, 0.0f};
    constexpr inline glm::vec3 playerGravityOnJumppad{0.0f, -10.0f, 0.0f};

    // Garbage.
    constexpr inline float garbageDamping = 0.2f;
    constexpr inline float garbageMass = 0.001f;
    constexpr inline glm::vec3 garbageGravityDefault{0.0f, -20.0f, 0.0f};
    constexpr inline int garbageMaxCountMagnetized = 350;
    inline int garbageCountMagnetized = 0;

    constexpr inline float staticEnvFriction = 40.0f;

    // Enemies.
    inline int maxActiveEnemiesCount = 500;
    constexpr inline int maxPathfindingInOneFrame = 10;
    inline int currentPathfindingEnemyIndex = 0;
    constexpr inline float minDistanceToSpawnEnemies = 150.0f;
    inline float radiusToKillEnemies = 15.0f; // Also amount of garbage to kill should be considered with that.
}
