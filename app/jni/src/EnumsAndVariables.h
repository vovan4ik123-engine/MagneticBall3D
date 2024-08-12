#pragma once
#include "DataBaseHelper.h"

namespace EnumsAndVars
{
    struct FontsPath
    {
        static const inline std::string roboto = "fonts/roboto.ttf";
        static const inline std::string creamy = "fonts/creamy.ttf";
        static const inline std::string cousine = "fonts/cousine.ttf";
    };

    struct AnimationIndexes
    {
        static constexpr inline int run = 0;    // 1_Run    - names in blender.
        static constexpr inline int stand = 1;  // 2_Stand
        static constexpr inline int attack = 2; // 3_Attack
    };

    // Database tables.
    struct SettingsMenu
    {
        // Stored in DB.
        static inline int FPSLimit = 60;
        static inline bool backgroundMusic = false;
        static inline bool meteorParticles = false;
        // Not stored in DB.
        // ...
    };

    struct CurrencyBalance
    {
        // Stored in DB.
        static inline int crystals = 115;
        // Not stored in DB.
        // ...
    };

    struct MapsProgress
    {
        // Stored in DB.
        static inline int currentMapIndex = 3; // Index of selected map on start screen.
        static inline int lastOpenedMapIndex = 3; // Last index of map available for select and play.
        // Not stored in DB.
        static constexpr inline int maxMapIndex = 3; // Last possible map index (opened or not). Must be hardcoded before release.
    };

    struct EnergySystem
    {
        // Stored in DB.
        static inline int currentAmount = 25;
        static inline uint64_t lastSecUpdated = 0; // Time in sec since epoch (1.1.1970).
        static inline uint64_t lastSecOneEnergyRestored = 0; // Time in sec since epoch (1.1.1970).
        // Not stored in DB.
        static constexpr inline int secToRestoreOneEnergy = 10;
        static constexpr inline int maxLimitToRestore = 25; // In update method when restore energy by time passes.
                                                            // If player buy energy for real money dont apply any limit.
        static constexpr inline int playCost = 5;
    };

    struct Shop
    {
        // Stored in DB.
        static inline bool item1FirstBuy = true;
        static inline bool item2FirstBuy = true;
        static inline bool item3FirstBuy = true;
        static inline bool item4FirstBuy = true;
        static inline bool item5FirstBuy = true;
        static inline bool item6FirstBuy = true;
    };

    enum class PlayerTalentCurrency
    {
        AD,
        CRYSTALS
    };
    struct PlayerTalentData
    {
        // Stored in DB.
        const std::string name;
        int currentLevel = 0;
        // Not stored in DB.
        const std::string description;
        const int maxLevel = 0;
        const float increasePerLevel = 0; // Percents in range 0...100 added per level.
        const std::string increasePerLevelText; // To show for user.
        const bool canBeImprovedByAd;
        const int priceCrystals;

        void improveLevel(PlayerTalentCurrency currency)
        {
            BR_ASSERT((currentLevel < maxLevel), "%s", "improveLevel(): currentLevel must be less than maxLevel.");
            if(currentLevel >= maxLevel)
                return;

            if(currency == PlayerTalentCurrency::CRYSTALS)
            {
                BR_ASSERT((priceCrystals <= CurrencyBalance::crystals), "%s", "improveLevel(): not enough crystals. Check it before call.");
                if(priceCrystals > CurrencyBalance::crystals)
                    return;

                // Update currency after subtract price.
                CurrencyBalance::crystals -= priceCrystals;
                DataBaseHelper::storeCurrencyBalanceCrystals(CurrencyBalance::crystals);
            }

            // Update talent.
            ++currentLevel;
            DataBaseHelper::updatePlayerTalent(name, currentLevel);
        }

        float getPercentsToImprove() // Percents to add to default value.
        {
            return float(currentLevel) * increasePerLevel;
        }
    };
    inline std::vector<PlayerTalentData> allPlayerTalents{{"MaxSpeed", 0, "Increase max\nspeed limit.", 20,
                                                           5.0f, "+5%", true, 20},
                                                          {"MagneticRadius", 0, "Increase\nmagnetic radius.", 80,
                                                           5.0f, "+5%", true, 10},
                                                          {"GarbageAmount", 0, "Increase amount of\nmagnetized garbage.", 20,
                                                           5.0f, "+5%", true, 20},
                                                          {"Accelerate", 0, "Increase\nacceleration.", 10,
                                                           3.0f, "+3%", true, 30},
                                                          {"Protection", 0, "Increase ball and\ngarbage protection.", 100,
                                                           5.0f, "+5%", true, 10},
                                                          {"Resurrection", 0, "Increase number\nof resurrections.", 3,
                                                           100.0f, "+1", false, 100}
                                                          };
    struct GameDifficulty
    {
        // Stored in DB.
        static inline int level = 0;
    };

    struct Ads
    {
        // Stored in DB.
        static inline uint64_t rewardedAdTime = 0; // Time in sec since epoch (1.1.1970) of last rewarded ad shown.
        // Not stored in DB.
        static constexpr inline int rewardedAdTimeDelay = 240; // 240 sec between rewarded ads.
    };

    struct DailyReward
    {
        // Stored in DB.
        static inline uint64_t tookTime = 0; // Time in sec since epoch (1.1.1970) of last daily reward took.
        static inline int tookDay = 1; // Must be 1 - 7. Represent day of week.
        // Not stored in DB.
        static constexpr inline int day1to6Reward = 10; // Crystals for 1 - 6 day.
        static constexpr inline int day7Reward = 20; // Crystals for 7 day.
    };

    // Database tables end.

    // Camera.
    constexpr inline float cameraZoomMaxSpeed = 14.0f; // Meters in sec.
    constexpr inline float cameraUpOffsetMaxSpeed = 2.6f; // Meters in sec.
    constexpr inline float minPlayerSpeedToCameraFollow = 10.0f;
    constexpr inline float minFPSForCameraRotation = 20.0f; // Camera rotation speed calculated based on this.

    // Swipe.
    constexpr inline float swipePowerMultiplier = 2.1f;

    // Player.
    constexpr inline float playerMagneticRadiusDefault = 30.0f;
    inline float playerMagneticRadius = playerMagneticRadiusDefault;
    constexpr inline float playerImpulseFactorOnGroundDefault = 0.1f;
    inline float playerImpulseFactorOnGround = playerImpulseFactorOnGroundDefault;
    constexpr inline float playerTorqueFactorOnGroundDefault = 0.09f;
    inline float playerTorqueFactorOnGround = playerTorqueFactorOnGroundDefault;
    constexpr inline float playerImpulseFactorOnBuildingRoofDefault = 0.08f;
    inline float playerImpulseFactorOnBuildingRoof = playerImpulseFactorOnBuildingRoofDefault;
    constexpr inline float playerTorqueFactorOnBuildingRoofDefault = 0.07f;
    inline float playerTorqueFactorOnBuildingRoof = playerTorqueFactorOnBuildingRoofDefault;
    constexpr inline float playerImpulseFactorOnBuildingWall = 0.12f;
    constexpr inline float playerTorqueFactorOnBuildingWallDefault = 0.34f;
    inline float playerTorqueFactorOnBuildingWall = playerTorqueFactorOnBuildingWallDefault;
    constexpr inline float playerImpulseFactorOnAir = 0.1f;
    constexpr inline float playerTorqueFactorOnAir = 0.02f;
    constexpr inline float playerLinearDamping = 0.08f;
    constexpr inline float playerAngularDamping = 0.08f;
    constexpr inline float playerFriction = 80.0f;
    constexpr inline float playerMass = 10.0f;
    constexpr inline glm::vec3 playerGravityOnAir{0.0f, -70.0f, 0.0f};
    constexpr inline glm::vec3 playerGravityOnGround{0.0f, -30.0f, 0.0f};
    constexpr inline glm::vec3 playerGravityOnBuildingRoof{0.0f, -30.0f, 0.0f};
    constexpr inline glm::vec3 playerGravityOnBuildingWall{0.0f, -9.0f, 0.0f};
    constexpr inline float playerMaxSpeedXZDefault = 80.0f;
    inline float playerMaxSpeedXZ = playerMaxSpeedXZDefault;
    inline int playerCurrentSpeed = 0;
    constexpr inline float playerSpeedForMeteor = 90.0f;
    constexpr inline float playerLeftRightTurnPower = 0.02f;
    constexpr inline float playerDamageTakenMultiplierDefault = 1.0f;
    inline float playerDamageTakenMultiplier = playerDamageTakenMultiplierDefault;
    constexpr inline float playerSpeedReductionMultiplierDefault = 1.0f;
    inline float playerSpeedReductionMultiplier = playerSpeedReductionMultiplierDefault;
    constexpr inline float playerRestoreHPAtNewLevelDefault = 0.0f;
    inline float playerRestoreHPAtNewLevel = playerRestoreHPAtNewLevelDefault;
    constexpr inline float playerXPMultiplierDefault = 1.0f;
    inline float playerXPMultiplier = playerXPMultiplierDefault;
    constexpr inline float playerStartHPDefault = 500.0f;
    inline float playerStartHP = playerStartHPDefault;
    constexpr inline int playerResurrectionAttemptsDefault = 1;
    inline int playerResurrectionAttempts = playerResurrectionAttemptsDefault;
    constexpr inline int playerCostOfResurrectionCrystals = 5;
    constexpr inline float playerDamageGroundRadiusAfterFallDefault = 0.0f;
    inline float playerDamageGroundRadiusAfterFall = playerDamageGroundRadiusAfterFallDefault;

    // Garbage.
    constexpr inline float garbageLinearDamping = 0.1f;
    constexpr inline float garbageAngularDamping = 0.2f;
    constexpr inline float garbageMass = 0.001f;
    constexpr inline float garbageMinGravityPower = 10.0f; // Magnetic power when player speed = 0.0f.
    // If player speed > 0.0f increase gravity power linearly with player speed.
    constexpr inline float garbageGravityIncreasedByPlayerSpeed = 3.2f; // * by player speed and add to garbageMinGravityPower.
    constexpr inline glm::vec3 garbageGravityDefault{0.0f, -30.0f, 0.0f};
    constexpr inline int garbageMaxCountMagnetizedDefault = 60;
    inline int garbageMaxCountMagnetized = garbageMaxCountMagnetizedDefault;
    constexpr inline int garbageCountMagnetizedDefault = 0;
    inline int garbageCountMagnetized = garbageCountMagnetizedDefault;
    constexpr inline float garbageDamageTakenMultiplierDefault = 1.0f;
    inline float garbageDamageTakenMultiplier = garbageDamageTakenMultiplierDefault;
    constexpr inline float garbageCommonSpawnMinDistance = 80.0f;
    constexpr inline float garbageCommonSpawnMaxDistance = 400.0f;
    constexpr inline float garbageCommonSpawnTimeDefault = -9999.0f; // Sec.
    inline float garbageCommonSpawnTime = garbageCommonSpawnTimeDefault; // Sec.
    constexpr inline float garbageCommonSpawnDelay = 1.0f; // Sec.
    constexpr inline int garbageCommonSpawnCountDefault = 2.0f;
    inline int garbageCommonSpawnCount = garbageCommonSpawnCountDefault;
    constexpr inline int garbageCommonMaxCountOnMapDefault = 130.0f;
    inline int garbageCommonMaxCountOnMap = garbageCommonMaxCountOnMapDefault;
    constexpr inline float garbageStartHPDefault = 40.0f;
    inline float garbageStartHP = garbageStartHPDefault;

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
    inline int enemiesKilledCount = 0;
    inline float enemiesPauseAllActionsTime = -9999.0f; // After player resurrection pause enemies for some sec
    inline float enemiesPauseAllActionsDelay = 0.0f;    // to give player time to collect garbage and prepare to fight.

    // Jump pad.
    constexpr inline float jumpPadPowerDefault = 120.0f * playerMass; // Power for player.
    inline float jumpPadPower = jumpPadPowerDefault;

    // Pause.
    inline bool gameOnPause = false;
    inline bool improvementSystemOnScreen = false;

    // Map.
    inline float mapPlayTimeSec = 0.0f;
    inline int mapSwipeCount = 0.0f;
    inline bool mapPlayerWin = false; // true = player win.

    // Enemies last wave.
    inline bool enemiesLastWavePhase = false;
    // Boss.
    inline bool mapHasBossPhase = false;
    inline bool bossPhase = false;

    inline void reset()
    {
        // Player.
        playerMagneticRadius = playerMagneticRadiusDefault * (1.0f + allPlayerTalents[1].getPercentsToImprove() / 100.0f);
        playerImpulseFactorOnGround = playerImpulseFactorOnGroundDefault * (1.0f + allPlayerTalents[3].getPercentsToImprove() / 100.0f);
        playerTorqueFactorOnGround = playerTorqueFactorOnGroundDefault *  (1.0f + allPlayerTalents[3].getPercentsToImprove() / 100.0f);
        playerImpulseFactorOnBuildingRoof = playerImpulseFactorOnBuildingRoofDefault *  (1.0f + allPlayerTalents[3].getPercentsToImprove() / 100.0f);
        playerTorqueFactorOnBuildingRoof = playerTorqueFactorOnBuildingRoofDefault *  (1.0f + allPlayerTalents[3].getPercentsToImprove() / 100.0f);
        playerTorqueFactorOnBuildingWall = playerTorqueFactorOnBuildingWallDefault; // Does not changed by talents.
        playerMaxSpeedXZ = playerMaxSpeedXZDefault * (1.0f + allPlayerTalents[0].getPercentsToImprove() / 100.0f);
        playerCurrentSpeed = 0;
        playerDamageTakenMultiplier = playerDamageTakenMultiplierDefault;
        playerSpeedReductionMultiplier = playerSpeedReductionMultiplierDefault;
        playerRestoreHPAtNewLevel = playerRestoreHPAtNewLevelDefault;
        playerXPMultiplier = playerXPMultiplierDefault;
        playerStartHP = playerStartHPDefault * (1.0f + allPlayerTalents[4].getPercentsToImprove() / 100.0f);
        playerResurrectionAttempts = playerResurrectionAttemptsDefault * int(1.0f + std::roundf(allPlayerTalents[5].getPercentsToImprove() / 100.0f));
        playerDamageGroundRadiusAfterFall = playerDamageGroundRadiusAfterFallDefault;

        // Garbage.
        garbageMaxCountMagnetized = garbageMaxCountMagnetizedDefault * (1.0f + allPlayerTalents[2].getPercentsToImprove() / 100.0f);
        garbageCountMagnetized = garbageCountMagnetizedDefault;
        garbageDamageTakenMultiplier = garbageDamageTakenMultiplierDefault;
        garbageCommonSpawnTime = garbageCommonSpawnTimeDefault;
        garbageCommonSpawnCount = garbageCommonSpawnCountDefault;
        garbageCommonMaxCountOnMap = garbageCommonMaxCountOnMapDefault;
        garbageStartHP = garbageStartHPDefault * (1.0f + allPlayerTalents[4].getPercentsToImprove() / 100.0f);

        // Enemies.
        enemiesMaxActiveCountOnGround = enemiesMaxActiveCountOnGroundDefault;
        enemiesCurrentPathfindingIndex = enemiesCurrentPathfindingIndexDefault;
        enemiesKilledCount = 0;
        enemiesPauseAllActionsTime = -9999.0f;
        enemiesPauseAllActionsDelay = 0.0f;

        // Jump pad.
        jumpPadPower = jumpPadPowerDefault;

        // Pause.
        gameOnPause = false;
        improvementSystemOnScreen = false;
        
        // Map.
        mapPlayTimeSec = 0.0f;
        mapSwipeCount = 0.0f;
        mapPlayerWin = false;

        // Enemies last wave.
        enemiesLastWavePhase = false;
        // Boss.
        mapHasBossPhase = false;
        bossPhase = false;
    }
}
