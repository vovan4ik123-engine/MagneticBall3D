#pragma once

namespace EnumsAndVariables
{
    struct FontsPath
    {
        static const inline std::string ROBOTO = "fonts/roboto.ttf";
        static const inline std::string CREAMY = "fonts/creamy.ttf";
        static const inline std::string COUSINE = "fonts/cousine.ttf";
    };

    // Camera.
    constexpr inline float cameraRotationMaxSpeed = glm::half_pi<float>() * 0.8f; // Radians in sec.
    constexpr inline float cameraZoomMaxSpeed = 30.0f; // Meters in sec.
    constexpr inline float minPlayerSpeedToCameraFollow = 7.0f;

    // Physics params.
    constexpr inline float playerMagneticRadius = 35.0f;
    constexpr inline float playerImpulseFactor = 0.25f;
    constexpr inline float playerTorqueFactor = 0.5f;
    constexpr inline float playerDamping = 0.2f;
    constexpr inline float playerFriction = 80.0f;
    constexpr inline float playerMass = 10.0f;
    constexpr inline glm::vec3 playerGravityOnAir{0.0f, -60.0f, 0.0f};
    constexpr inline glm::vec3 playerGravityOnGround{0.0f, -25.0f, 0.0f};
    constexpr inline glm::vec3 playerGravityOnBuilding{0.0f, -4.0f, 0.0f};

    constexpr inline float garbageDamping = 0.2f;
    constexpr inline float garbageMass = 0.001f;
    constexpr inline glm::vec3 garbageGravityDefault{0.0f, -20.0f, 0.0f};

    constexpr inline float staticEnvFriction = 40.0f;

    // Enemies.
    inline int maxActiveEnemiesCount = 1;
    constexpr inline int maxPathfindingInOneFrame = 20;
    inline int currentPathfindingEnemyIndex = 0;
}
