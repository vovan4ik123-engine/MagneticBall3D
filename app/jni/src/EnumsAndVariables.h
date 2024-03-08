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
    constexpr float minPlayerSpeedToCameraFollow = 7.0f;

    // Physics params.
    constexpr float playerMagneticRadius = 35.0f;
    constexpr float playerImpulseFactor = 0.25f;
    constexpr float playerTorqueFactor = 0.5f;
    constexpr float playerDamping = 0.2f;
    constexpr float playerFriction = 40.0f;
    constexpr float playerMass = 10.0f;

    constexpr float garbageDamping = 0.2f;
    constexpr float garbageMass = 0.001f;

    constexpr float staticEnvFriction = 60.0f;
}
