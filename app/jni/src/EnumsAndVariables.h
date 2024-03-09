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
    constexpr float cameraRotationMaxSpeed = glm::half_pi<float>() * 1.3f; // Radians in sec.
    constexpr float cameraZoomMaxSpeed = 30.0f; // Meters in sec.
    constexpr float minPlayerSpeedToCameraFollow = 7.0f;

    // Physics params.
    constexpr float playerMagneticRadius = 35.0f;
    constexpr float playerImpulseFactor = 0.25f;
    constexpr float playerTorqueFactor = 0.5f;
    constexpr float playerDamping = 0.2f;
    constexpr float playerFriction = 80.0f;
    constexpr float playerMass = 10.0f;
    constexpr glm::vec3 playerGravityOnAir{0.0f, -50.0f, 0.0f};
    constexpr glm::vec3 playerGravityOnGround{0.0f, -25.0f, 0.0f};
    constexpr glm::vec3 playerGravityOnBuilding{0.0f, -4.0f, 0.0f};

    constexpr float garbageDamping = 0.2f;
    constexpr float garbageMass = 0.001f;
    constexpr glm::vec3 garbageGravityDefault{0.0f, -20.0f, 0.0f};

    constexpr float staticEnvFriction = 40.0f;
}
