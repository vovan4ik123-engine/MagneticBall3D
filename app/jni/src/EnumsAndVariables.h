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
        static const inline int run = 0;
        static const inline int stand = 1;
        static const inline int attack = 2;
    };

    struct SettingsMenu
    {
        static inline int FPSLimit = 60;
    };

    struct CurrencyBalance
    {
        static inline int crystals = 300;
    };

    // Camera.
    constexpr inline float cameraZoomMaxSpeed = 150.0f; // Meters in sec.
    constexpr inline float minPlayerSpeedToCameraFollow = 5.0f;
    constexpr inline float cameraYAccordingPlayerY = 0.005f; // Multiply by player.y and add to m_cameraOffset.y.
    constexpr inline float cameraRotateDelay = 0.02f;
    inline float cameraRotateTime = 0.0f; // Sec.

    // Swipe.
    constexpr inline float swipePowerMultiplier = 2.1f;

    // Player.
    constexpr inline float playerMagneticRadius = 35.0f;
    constexpr inline float playerImpulseFactorOnGround = 0.15f;
    constexpr inline float playerTorqueFactorOnGround = 0.13f;
    constexpr inline float playerImpulseFactorOnBuilding = 0.13f;
    constexpr inline float playerTorqueFactorOnBuilding = 0.15f; // Will multiplied by radius.
    constexpr inline float playerImpulseFactorOnAir = 0.08f;
    constexpr inline float playerTorqueFactorOnAir = 0.1f;
    constexpr inline float playerLinearDamping = 0.1f;
    constexpr inline float playerAngularDamping = 0.1f;
    constexpr inline float playerFriction = 80.0f;
    constexpr inline float playerMass = 10.0f;
    constexpr inline glm::vec3 playerGravityOnAir{0.0f, -60.0f, 0.0f};
    constexpr inline glm::vec3 playerGravityOnGround{0.0f, -25.0f, 0.0f};
    constexpr inline glm::vec3 playerGravityOnBuilding{0.0f, -8.0f, 0.0f};
    constexpr inline glm::vec3 playerGravityOnJumppad{0.0f, -10.0f, 0.0f};
    inline float playerMaxSpeedXZ = 100.0f;
    constexpr inline float playerSpeedForMeteor = 70.0f;
    constexpr inline float playerLeftRightTurnPower = 0.03f;

    // Garbage.
    constexpr inline float garbageDamping = 0.2f;
    constexpr inline float garbageMass = 0.001f;
    constexpr inline float garbageMinGravityPower = 150.0f; // Magnetic power when player speed = 0.0f.
    constexpr inline float garbageMaxGravityPower = 700.0f;
    // If player speed > 0.0f increase gravity power linearly with player speed.
    constexpr inline float garbageGravityIncreasedByPlayerSpeed = 4.0f; // * by player speed and add to garbageMinGravityPower.
    constexpr inline glm::vec3 garbageGravityDefault{0.0f, -30.0f, 0.0f};
    constexpr inline int garbageCommonMaxActive = 400; // Limit of active GarbageType::COMMON on map.
    constexpr inline int garbageMaxCountMagnetized = 300;
    inline int garbageCountMagnetized = 0;
    constexpr inline float minDistanceToSpawnGarbage = 0.0f;
    constexpr inline float maxDistanceToSpawnGarbage = 300.0f;
    inline float lastTimeSpawnGarbage = 0.0f; // Sec.
    constexpr inline  float spawnGarbageDelay = 3.0f; // Sec.

    constexpr inline float staticEnvFriction = 40.0f;
    constexpr inline float playerMassToGarbageMassRatio = 1.0f / (playerMass / garbageMass);

    // Enemies.
    inline int maxActiveEnemiesCountOnGround = 0;
    constexpr inline int maxPathfindingInOneFrame = 10;
    inline int currentPathfindingEnemyIndex = 0;
    constexpr inline float minDistanceToSpawnEnemies = 200.0f;
    constexpr inline float maxDistanceToSpawnEnemies = 600.0f;
    inline float radiusToKillEnemies = 15.0f; // Also amount of garbage to kill should be considered with that.

    // Pause.
    inline bool gameOnPause = false;
    inline bool improvementSystemOnScreen = false;

    // Jump pad
    constexpr inline float jumpPadPower = 120.0f;
}
