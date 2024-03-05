#pragma once

namespace EnumsAndVariables
{
    struct FontsPath
    {
        static const inline std::string ROBOTO = "fonts/roboto.ttf";
        static const inline std::string CREAMY = "fonts/creamy.ttf";
        static const inline std::string COUSINE = "fonts/cousine.ttf";
    };

    // Physics params.
    constexpr float playerMaxSpeed = 70.0f;
    constexpr float cameraRotationMaxSpeed = glm::half_pi<float>() * 1.3f; // Radians in sec.
    constexpr float minPlayerSpeedToCameraFollow = 7.0f;
    constexpr float playerImpulseFactor = 0.1f;
    constexpr float playerTorqueFactor = 0.6f;
    constexpr float playerDamping = 0.0f;
    constexpr float playerFriction = 100.0f;
}
