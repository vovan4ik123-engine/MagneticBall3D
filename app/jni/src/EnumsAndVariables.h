#pragma once

namespace EnAndVars
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
        static inline bool backgroundMusic = true;
    };

    struct CurrencyBalance
    {
        static inline int crystals = 70;
    };

    // Camera.
    constexpr inline float cameraZoomMaxSpeed = 20.0f; // Meters in sec.
    constexpr inline float minPlayerSpeedToCameraFollow = 5.0f;
    constexpr inline float cameraRotateDelay = 0.02f;
    inline float cameraRotateTime = 0.0f; // Sec.

    // Swipe.
    constexpr inline float swipePowerMultiplier = 2.2f;
    constexpr inline float playerSpeedToPlayEngineSound = 110.0f;

    // Player.
    constexpr inline float playerMagneticRadiusDefault = 30.0f;
    inline float playerMagneticRadius = playerMagneticRadiusDefault;
    constexpr inline float playerImpulseFactorOnGroundDefault = 0.09f;
    inline float playerImpulseFactorOnGround = playerImpulseFactorOnGroundDefault;
    constexpr inline float playerTorqueFactorOnGroundDefault = 0.08f;
    inline float playerTorqueFactorOnGround = playerTorqueFactorOnGroundDefault;
    constexpr inline float playerImpulseFactorOnBuildingRoofDefault = 0.08f;
    inline float playerImpulseFactorOnBuildingRoof = playerImpulseFactorOnBuildingRoofDefault;
    constexpr inline float playerTorqueFactorOnBuildingRoofDefault = 0.07f;
    inline float playerTorqueFactorOnBuildingRoof = playerTorqueFactorOnBuildingRoofDefault;
    constexpr inline float playerImpulseFactorOnBuildingWall = 0.22f;
    constexpr inline float playerTorqueFactorOnBuildingWallDefault = 0.2f;
    inline float playerTorqueFactorOnBuildingWall = playerTorqueFactorOnBuildingWallDefault;
    constexpr inline float playerImpulseFactorOnAir = 0.07f;
    constexpr inline float playerTorqueFactorOnAir = 0.06f;
    constexpr inline float playerLinearDamping = 0.1f;
    constexpr inline float playerAngularDamping = 0.1f;
    constexpr inline float playerFriction = 80.0f;
    constexpr inline float playerMass = 10.0f;
    constexpr inline glm::vec3 playerGravityOnAir{0.0f, -70.0f, 0.0f};
    constexpr inline glm::vec3 playerGravityOnGround{0.0f, -30.0f, 0.0f};
    constexpr inline glm::vec3 playerGravityOnBuildingRoof{0.0f, -30.0f, 0.0f};
    constexpr inline glm::vec3 playerGravityOnBuildingWall{0.0f, -10.0f, 0.0f};
    constexpr inline float playerMaxSpeedXZDefault = 90.0f;
    inline float playerMaxSpeedXZ = playerMaxSpeedXZDefault;
    constexpr inline float playerSpeedForMeteor = 100.0f;
    constexpr inline float playerLeftRightTurnPower = 0.013f;
    constexpr inline float playerDamageTakenMultiplierDefault = 1.0f;
    inline float playerDamageTakenMultiplier = playerDamageTakenMultiplierDefault;
    constexpr inline float playerSpeedReductionMultiplierDefault = 1.0f;
    inline float playerSpeedReductionMultiplier = playerSpeedReductionMultiplierDefault;
    constexpr inline float playerRestoreHPAtNewLevelDefault = 0.0f;
    inline float playerRestoreHPAtNewLevel = playerRestoreHPAtNewLevelDefault;
    constexpr inline float playerXPMultiplierDefault = 1.0f;
    inline float playerXPMultiplier = playerXPMultiplierDefault;
    constexpr inline float playerStartHP = 500.0f;
    constexpr inline int playerResurrectionAttemptsDefault = 2;
    inline int playerResurrectionAttempts = playerResurrectionAttemptsDefault;
    constexpr inline int playerCostOfResurrectionCrystals = 10;

    // Garbage.
    constexpr inline float garbageLinearDamping = 0.1f;
    constexpr inline float garbageAngularDamping = 0.3f;
    constexpr inline float garbageMass = 0.001f;
    constexpr inline float garbageMinGravityPower = 10.0f; // Magnetic power when player speed = 0.0f.
    constexpr inline float garbageMaxGravityPower = 750.0f;
    // If player speed > 0.0f increase gravity power linearly with player speed.
    constexpr inline float garbageGravityIncreasedByPlayerSpeed = 7.0f; // * by player speed and add to garbageMinGravityPower.
    constexpr inline glm::vec3 garbageGravityDefault{0.0f, -30.0f, 0.0f};
    constexpr inline int garbageMaxCountMagnetizedDefault = 70;
    inline int garbageMaxCountMagnetized = garbageMaxCountMagnetizedDefault;
    constexpr inline int garbageCountMagnetizedDefault = 0;
    inline int garbageCountMagnetized = garbageCountMagnetizedDefault;
    constexpr inline float garbageDamageTakenMultiplierDefault = 1.0f;
    inline float garbageDamageTakenMultiplier = garbageDamageTakenMultiplierDefault;
    constexpr inline float garbageCommonSpawnMinDistance = 80.0f;
    constexpr inline float garbageCommonSpawnMaxDistance = 400.0f;
    constexpr inline float garbageCommonSpawnTimeDefault = 0.0f; // Sec.
    inline float garbageCommonSpawnTime = garbageCommonSpawnTimeDefault; // Sec.
    constexpr inline float garbageCommonSpawnDelayDefault = 1.0f; // Sec.
    inline float garbageCommonSpawnDelay = garbageCommonSpawnDelayDefault; // Sec.
    constexpr inline int garbageCommonSpawnCountDefault = 2.0f;
    inline int garbageCommonSpawnCount = garbageCommonSpawnCountDefault;

    constexpr inline float staticEnvFriction = 2.0f;
    constexpr inline float playerMassToGarbageMassRatio = 1.0f / (playerMass / garbageMass);

    // Enemies.
    constexpr inline int enemiesMaxActiveCountOnGroundDefault = 0;
    inline int enemiesMaxActiveCountOnGround = enemiesMaxActiveCountOnGroundDefault;
    constexpr inline int enemiesMaxPathfindingInOneFrame = 8;
    constexpr inline int enemiesCurrentPathfindingIndexDefault = 0;
    inline int enemiesCurrentPathfindingIndex = enemiesCurrentPathfindingIndexDefault;
    constexpr inline float enemiesMinDistanceToSpawn = 200.0f;
    constexpr inline float enemiesMaxDistanceToSpawn = 400.0f;

    // Pause.
    inline bool gameOnPause = false;
    inline bool improvementSystemOnScreen = false;

    // Jump pad.
    constexpr inline float jumpPadPower = 130.0f * playerMass; // Power for player.

    // Map.
    inline float mapStartTimeSec = 0.0f;
    inline float mapPlayTimeSec = 0.0f;
    inline int mapSwipeCount = 0.0f;
    inline int mapCurrentNumber = 0;
    inline bool mapPlayerWin = false; // true = player win.
    constexpr inline int mapWinCrystalsPrize = 12;

    // Boss.
    inline bool prepareToBossPhase = false;
    inline bool bossPhase = false;

    inline void reset()
    {
        // Camera.
        cameraRotateTime = 0.0f; // Sec.

        // Player.
        playerMagneticRadius = playerMagneticRadiusDefault;
        playerImpulseFactorOnGround = playerImpulseFactorOnGroundDefault;
        playerTorqueFactorOnGround = playerTorqueFactorOnGroundDefault;
        playerImpulseFactorOnBuildingRoof = playerImpulseFactorOnBuildingRoofDefault;
        playerTorqueFactorOnBuildingRoof = playerTorqueFactorOnBuildingRoofDefault;
        playerTorqueFactorOnBuildingWall = playerTorqueFactorOnBuildingWallDefault;
        playerMaxSpeedXZ = playerMaxSpeedXZDefault;
        playerDamageTakenMultiplier = playerDamageTakenMultiplierDefault;
        playerSpeedReductionMultiplier = playerSpeedReductionMultiplierDefault;
        playerRestoreHPAtNewLevel = playerRestoreHPAtNewLevelDefault;
        playerXPMultiplier = playerXPMultiplierDefault;
        playerResurrectionAttempts = playerResurrectionAttemptsDefault;

        // Garbage.
        garbageMaxCountMagnetized = garbageMaxCountMagnetizedDefault;
        garbageCountMagnetized = garbageCountMagnetizedDefault;
        garbageDamageTakenMultiplier = garbageDamageTakenMultiplierDefault;
        garbageCommonSpawnTime = garbageCommonSpawnTimeDefault;
        garbageCommonSpawnDelay = garbageCommonSpawnDelayDefault;
        garbageCommonSpawnCount = garbageCommonSpawnCountDefault;

        // Enemies.
        enemiesMaxActiveCountOnGround = enemiesMaxActiveCountOnGroundDefault;
        enemiesCurrentPathfindingIndex = enemiesCurrentPathfindingIndexDefault;

        // Pause.
        gameOnPause = false;
        improvementSystemOnScreen = false;
        
        // Map.
        mapStartTimeSec = 0.0f;
        mapPlayTimeSec = 0.0f;
        mapSwipeCount = 0.0f;
        mapCurrentNumber = 0;
        mapPlayerWin = false;

        // Boss.
        prepareToBossPhase = false;
        bossPhase = false;
    }
}
