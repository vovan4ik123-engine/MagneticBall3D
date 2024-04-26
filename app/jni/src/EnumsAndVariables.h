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
    constexpr inline float swipePowerMultiplier = 2.2f;

    // Player.
    inline float playerMagneticRadius = 25.0f;
    inline float playerImpulseFactorOnGround = 0.09f; // Default 0.09f.
    inline float playerTorqueFactorOnGround = 0.07f; // Default 0.07f.
    inline float playerImpulseFactorOnBuilding = 0.025f;  // Default 0.025f.
    inline float playerTorqueFactorOnBuilding = 0.035f;  // Default 0.035f. Will multiplied by radius.
    constexpr inline float playerImpulseFactorOnAir = 0.07f;
    constexpr inline float playerTorqueFactorOnAir = 0.06f;
    constexpr inline float playerLinearDamping = 0.1f;
    constexpr inline float playerAngularDamping = 0.15f;
    constexpr inline float playerFriction = 80.0f;
    constexpr inline float playerMass = 10.0f;
    constexpr inline glm::vec3 playerGravityOnAir{0.0f, -70.0f, 0.0f};
    constexpr inline glm::vec3 playerGravityOnGround{0.0f, -30.0f, 0.0f};
    constexpr inline glm::vec3 playerGravityOnBuilding{0.0f, -8.0f, 0.0f};
    inline float playerMaxSpeedXZ = 80.0f; // Default 80.0f.
    constexpr inline float playerSpeedForMeteor = 100.0f;
    constexpr inline float playerLeftRightTurnPower = 0.023f;
    inline float playerRadiusToKillEnemies = 10.0f;
    inline float playerDamageTakenMultiplier = 1.0f;
    inline float playerSpeedReductionMultiplier = 1.0f;
    inline float playerRestoreHPAtNewLevel = 0.0f;
    inline float playerXPMultiplier = 1.0f;

    // Garbage.
    constexpr inline float garbageLinearDamping = 0.1f;
    constexpr inline float garbageAngularDamping = 0.3f;
    constexpr inline float garbageMass = 0.001f;
    constexpr inline float garbageMinGravityPower = 150.0f; // Magnetic power when player speed = 0.0f.
    constexpr inline float garbageMaxGravityPower = 700.0f;
    // If player speed > 0.0f increase gravity power linearly with player speed.
    constexpr inline float garbageGravityIncreasedByPlayerSpeed = 6.5f; // * by player speed and add to garbageMinGravityPower.
    constexpr inline glm::vec3 garbageGravityDefault{0.0f, -70.0f, 0.0f};
    inline int garbageMaxCountMagnetized = 80;
    inline int garbageCountMagnetized = 0;
    inline float garbageDamageTakenMultiplier = 1.0f;
    constexpr inline int garbageCommonMaxActive = 100; // Limit of active GarbageType::COMMON on map.
    constexpr inline float garbageCommonSpawnMinDistance = 0.0f;
    constexpr inline float garbageCommonSpawnMaxDistance = 150.0f;
    inline float garbageCommonSpawnTime = 0.0f; // Sec.
    inline float garbageCommonSpawnDelay = 2.0f; // Sec.
    inline int garbageCommonSpawnCount = 4.0f;

    constexpr inline float staticEnvFriction = 2.0f;
    constexpr inline float playerMassToGarbageMassRatio = 1.0f / (playerMass / garbageMass);

    // Enemies.
    inline int enemiesMaxActiveCountOnGround = 0;
    constexpr inline int enemiesMaxPathfindingInOneFrame = 10;
    inline int enemiesCurrentPathfindingIndex = 0;
    constexpr inline float enemiesMinDistanceToSpawn = 200.0f;
    constexpr inline float enemiesMaxDistanceToSpawn = 600.0f;

    // Pause.
    inline bool gameOnPause = false;
    inline bool improvementSystemOnScreen = false;

    // Jump pad.
    constexpr inline float jumpPadPower = 140.0f;

    // Map.
    inline float mapStartTimeSec = 0.0f;
    inline float mapPlayTimeSec = 0.0f;
    inline int mapSwipeCount = 0.0f;
}
