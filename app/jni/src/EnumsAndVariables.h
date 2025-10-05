#pragma once
#include "DataBaseHelper.h"

namespace EnumsAndVars
{
    struct FontsPath
    {
        static const inline std::string roboto = "fonts/roboto.ttf";
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
        static inline bool backgroundMusic = true;
        static inline bool meteorParticles = false;
        static inline bool interfaceGUI = true; // Disable GUI elements in play state. (HP/XP bars, counters, pause button, improvements, joystick).
        // Not stored in DB.
        static inline float cameraHorizontalSpeed = 432.0f;
        static inline float cameraVerticalSpeed = 155.7f;
        static inline float cameraSpeedThresholdToAccelerate = 50.0f;
    };

    struct CurrencyBalance
    {
        // Stored in DB.
        static inline int crystals = 35;
        // Not stored in DB.
        // ...
    };

    struct MapsProgress
    {
        // Stored in DB.
        static inline int currentMapIndex = 0; // Index of selected map on start screen.
        static inline int lastOpenedMapIndex = 0; // Last index of map available for select and play.
        // Not stored in DB.
        static constexpr inline int maxMapIndex = 5; // Last possible map index (opened or not). Must be hardcoded before release.
        static inline int mapIndexWhenMapStart = 0;
    };

    struct EnergySystem
    {
        // Stored in DB.
        static inline int currentAmount = 25;
        static inline uint64_t lastSecUpdated = 0; // Time in sec since epoch (1.1.1970).
        static inline uint64_t lastSecOneEnergyRestored = 0; // Time in sec since epoch (1.1.1970).
        // Not stored in DB.
        static constexpr inline int secToRestoreOneEnergy = 10;
        static constexpr inline int maxLimitToRestore = 25; // Max limit that can be restored by time. If player buy energy for real money dont apply any limit.
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
        static inline bool adsOnMapsDisabled = false; // Ads in play state.
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
        const int firstLevelPriceCrystals;

        void improveLevel(PlayerTalentCurrency currency)
        {
            BR_ASSERT((currentLevel < maxLevel), "%s", "improveLevel(): currentLevel must be less than maxLevel.");
            if(currentLevel >= maxLevel)
                return;

            if(currency == PlayerTalentCurrency::CRYSTALS)
            {
                BR_ASSERT((getCurrentLevelPriceCrystals() <= CurrencyBalance::crystals), "%s", "improveLevel(): not enough crystals. Check it before call.");
                if(getCurrentLevelPriceCrystals() > CurrencyBalance::crystals)
                    return;

                // Update currency after subtract price.
                CurrencyBalance::crystals -= getCurrentLevelPriceCrystals();
                DataBaseHelper::storeCurrencyBalanceCrystals(CurrencyBalance::crystals);
            }

            // Update talent.
            ++currentLevel;
            DataBaseHelper::updatePlayerTalent(name, currentLevel);
        }

        float getPercentsToImprove() const // Percents to add to default value.
        {
            return float(currentLevel) * increasePerLevel;
        }

        int getCurrentLevelPriceCrystals() const
        {
            return firstLevelPriceCrystals + currentLevel;
        }
    };
    inline std::vector<PlayerTalentData> allPlayerTalents{{"MaxSpeed", 0, "Increase\nspeed limit.", 20,
                                                           5.0f, "+5%", true, 10},
                                                          {"MagneticRadius", 0, "Increase\nmagnetic radius.", 80,
                                                           5.0f, "+5%", true, 1},
                                                          {"GarbageAmount", 0, "Increase amount of\nmagnetized items.", 20,
                                                           5.0f, "+5%", true, 10},
                                                          {"Accelerate", 0, "Increase\nacceleration.", 10,
                                                           3.0f, "+3%", true, 15},
                                                          {"Protection", 0, "Increase ball and\nitems protection.", 100,
                                                           5.0f, "+5%", true, 1},
                                                          {"Resurrection", 0, "Increase number\nof resurrections.", 3,
                                                           100.0f, "+1", false, 90},
                                                          {"SmashDamage", 0, "Increase smash damage\nper one item.", 100,
                                                           5.0f, "+5%", true, 1},
                                                          {"ShotDamage", 0, "Increase\nshot damage.", 100,
                                                           5.0f, "+5%", true, 1}
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
        static constexpr inline int rewardedAdTimeDelay = 90; // Use it if you need pause between rewarded ads.
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
    constexpr inline float cameraUpOffsetMaxSpeed = 1.5f; // Meters in sec.

    // Joystick.
    constexpr inline float joystickPowerInOneSec = 100.0f;

    // Player.
    constexpr inline float playerMagneticRadiusDefault = 50.0f;
    inline float playerMagneticRadius = playerMagneticRadiusDefault;
    constexpr inline float playerImpulseFactorOnGroundDefault = 1.5f;
    inline float playerImpulseFactorOnGround = playerImpulseFactorOnGroundDefault;
    constexpr inline float playerTorqueFactorOnGroundDefault = 1.4f;
    inline float playerTorqueFactorOnGround = playerTorqueFactorOnGroundDefault;
    constexpr inline float playerImpulseFactorOnBuildingRoofDefault = 1.3f;
    inline float playerImpulseFactorOnBuildingRoof = playerImpulseFactorOnBuildingRoofDefault;
    constexpr inline float playerTorqueFactorOnBuildingRoofDefault = 1.2f;
    inline float playerTorqueFactorOnBuildingRoof = playerTorqueFactorOnBuildingRoofDefault;
    constexpr inline float playerImpulseFactorOnBuildingWall = 8.0f;
    constexpr inline float playerTorqueFactorOnBuildingWallDefault = 8.0f;
    inline float playerTorqueFactorOnBuildingWall = playerTorqueFactorOnBuildingWallDefault;
    constexpr inline float playerImpulseFactorOnAirDefault = 3.5f;
    inline float playerImpulseFactorOnAir = playerImpulseFactorOnAirDefault;
    constexpr inline float playerTorqueFactorOnAir = 0.02f;
    constexpr inline float playerDamping = 0.05f;
    constexpr inline float playerFriction = 80.0f;
    constexpr inline float playerMass = 10.0f;
    constexpr inline glm::vec3 playerGravityOnAir{0.0f, -60.0f, 0.0f};
    constexpr inline glm::vec3 playerGravityOnGround{0.0f, -30.0f, 0.0f};
    constexpr inline glm::vec3 playerGravityOnBuildingRoof{0.0f, -30.0f, 0.0f};
    constexpr inline glm::vec3 playerGravityOnBuildingWall{0.0f, -8.0f, 0.0f};
    constexpr inline float playerMaxSpeedXZDefault = 80.0f;
    inline float playerMaxSpeedXZ = playerMaxSpeedXZDefault;
    inline float playerCurrentSpeed = 0;
    constexpr inline float playerSpeedForMeteor = 90.0f;
    constexpr inline float playerDamageTakenMultiplierDefault = 0.8f;
    inline float playerDamageTakenMultiplier = playerDamageTakenMultiplierDefault;
    constexpr inline float playerSpeedReductionMultiplierDefault = 1.0f;
    inline float playerSpeedReductionMultiplier = playerSpeedReductionMultiplierDefault;
    constexpr inline float playerXPMultiplierDefault = 1.0f;
    inline float playerXPMultiplier = playerXPMultiplierDefault;
    constexpr inline float playerStartHPDefault = 500.0f;
    inline float playerStartHP = playerStartHPDefault;
    constexpr inline int playerResurrectionAttemptsDefault = 1;
    inline int playerResurrectionAttempts = playerResurrectionAttemptsDefault;
    constexpr inline int playerCostOfResurrectionCrystals = 5;
    constexpr inline float playerDamageGroundRadiusAfterFallDefault = 0.0f;
    inline float playerDamageGroundRadiusAfterFall = playerDamageGroundRadiusAfterFallDefault;
    inline float playerResurrectTime = -999999.0f;

    // Garbage.
    constexpr inline float garbageLinearDamping = 0.1f;
    constexpr inline float garbageAngularDamping = 0.2f;
    constexpr inline float garbageMass = 0.001f;
    constexpr inline float garbageMinGravityPower = 15.0f; // Magnetic power when player speed = 0.0f.
    // If player speed > 0.0f increase gravity power linearly with player speed.
    constexpr inline float garbageGravityIncreasedByPlayerSpeed = 3.5f; // * by player speed and add to garbageMinGravityPower.
    constexpr inline glm::vec3 garbageGravityDefault{0.0f, -30.0f, 0.0f};
    constexpr inline int garbageMaxCountMagnetizedDefault = 60;
    inline int garbageMaxCountMagnetized = garbageMaxCountMagnetizedDefault;
    constexpr inline int garbageCountMagnetizedDefault = 0;
    inline int garbageCountMagnetized = garbageCountMagnetizedDefault;
    constexpr inline float garbageDamageTakenMultiplierDefault = 1.0f;
    inline float garbageDamageTakenMultiplier = garbageDamageTakenMultiplierDefault;
    constexpr inline float garbageCommonSpawnMinDistance = 80.0f;
    constexpr inline float garbageCommonSpawnMaxDistance = 400.0f;
    constexpr inline float garbageCommonSpawnTimeDefault = -999999.0f; // Sec.
    inline float garbageCommonSpawnTime = garbageCommonSpawnTimeDefault; // Sec.
    constexpr inline float garbageCommonSpawnDelay = 1.0f; // Sec.
    constexpr inline int garbageCommonSpawnCountDefault = 3;
    inline int garbageCommonSpawnCount = garbageCommonSpawnCountDefault;
    constexpr inline int garbageCommonMaxCountOnMapDefault = 130.0f;
    inline int garbageCommonMaxCountOnMap = garbageCommonMaxCountOnMapDefault;
    constexpr inline float garbageStartHPDefault = 40.0f;
    inline float garbageStartHP = garbageStartHPDefault;

    constexpr inline float staticEnvFriction = 2.0f;
    constexpr inline float playerMassToGarbageMassRatio = 1.0f / (playerMass / garbageMass);

    // Damage.
    constexpr inline float damageSmashDefault = 1.0f; // Smash damage per ball/one magnetized garbage.
    inline float damageSmash = damageSmashDefault;
    constexpr inline float damageShotDefault = 8.0f; // Damage of shot garbage.
    inline float damageShot = damageShotDefault;
    constexpr inline float damageShotReloadTimeDefault = 0.5f;
    inline float damageShotReloadTime = damageShotReloadTimeDefault;
    inline float damageLastShotTime = -999999.0f;
    constexpr inline float damageShotPower = 0.35f;
    inline std::vector<int> garbageAsBulletsIDs;

    // Enemies.
    constexpr inline int enemiesMaxPathfindingInOneFrame = 10;
    constexpr inline int enemiesCurrentPathfindingIndexDefault = 0;
    inline int enemiesCurrentPathfindingIndex = enemiesCurrentPathfindingIndexDefault;
    constexpr inline float enemiesMinDistanceToSpawn = 200.0f;
    constexpr inline float enemiesMaxDistanceToSpawn = 450.0f;
    constexpr inline float enemiesRespawnDistance = 451.0f;
    inline int enemiesKilledCount = 0;

    // Jump pad.
    constexpr inline float jumpPadPowerDefault = 110.0f * playerMass; // Power for player.
    inline float jumpPadPower = jumpPadPowerDefault;

    // Pause.
    inline bool gameOnPause = false;
    inline bool improvementSystemOnScreen = false;

    // Map.
    inline float mapPlayTimeSec = 0.0f;
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
        playerTorqueFactorOnGround = playerTorqueFactorOnGroundDefault * (1.0f + allPlayerTalents[3].getPercentsToImprove() / 100.0f);
        playerImpulseFactorOnBuildingRoof = playerImpulseFactorOnBuildingRoofDefault * (1.0f + allPlayerTalents[3].getPercentsToImprove() / 100.0f);
        playerTorqueFactorOnBuildingRoof = playerTorqueFactorOnBuildingRoofDefault * (1.0f + allPlayerTalents[3].getPercentsToImprove() / 100.0f);
        playerTorqueFactorOnBuildingWall = playerTorqueFactorOnBuildingWallDefault; // Does not changed by talents.
        playerImpulseFactorOnAir = playerImpulseFactorOnAirDefault * (1.0f + allPlayerTalents[3].getPercentsToImprove() / 100.0f);
        playerMaxSpeedXZ = playerMaxSpeedXZDefault * (1.0f + allPlayerTalents[0].getPercentsToImprove() / 100.0f);
        playerCurrentSpeed = 0;
        playerDamageTakenMultiplier = playerDamageTakenMultiplierDefault;
        playerSpeedReductionMultiplier = playerSpeedReductionMultiplierDefault;
        playerXPMultiplier = playerXPMultiplierDefault;
        playerStartHP = playerStartHPDefault * (1.0f + allPlayerTalents[4].getPercentsToImprove() / 100.0f);
        playerResurrectionAttempts = playerResurrectionAttemptsDefault * int(1.0f + std::roundf(allPlayerTalents[5].getPercentsToImprove() / 100.0f));
        playerDamageGroundRadiusAfterFall = playerDamageGroundRadiusAfterFallDefault;
        playerResurrectTime = -999999.0f;

        // Garbage.
        garbageMaxCountMagnetized = garbageMaxCountMagnetizedDefault * (1.0f + allPlayerTalents[2].getPercentsToImprove() / 100.0f);
        garbageCountMagnetized = garbageCountMagnetizedDefault;
        garbageDamageTakenMultiplier = garbageDamageTakenMultiplierDefault;
        garbageCommonSpawnTime = garbageCommonSpawnTimeDefault;
        garbageCommonSpawnCount = garbageCommonSpawnCountDefault;
        garbageCommonMaxCountOnMap = garbageCommonMaxCountOnMapDefault;
        garbageStartHP = garbageStartHPDefault * (1.0f + allPlayerTalents[4].getPercentsToImprove() / 100.0f);

        // Damage.
        damageSmash = damageSmashDefault * (1.0f + allPlayerTalents[6].getPercentsToImprove() / 100.0f);
        damageShot = damageShotDefault * (1.0f + allPlayerTalents[7].getPercentsToImprove() / 100.0f);
        damageShotReloadTime = damageShotReloadTimeDefault;
        damageLastShotTime = -999999.0f;
        garbageAsBulletsIDs.clear();

        // Enemies.
        enemiesCurrentPathfindingIndex = enemiesCurrentPathfindingIndexDefault;
        enemiesKilledCount = 0;

        // Jump pad.
        jumpPadPower = jumpPadPowerDefault;

        // Pause.
        gameOnPause = false;
        improvementSystemOnScreen = false;
        
        // Map.
        mapPlayTimeSec = 0.0f;
        mapPlayerWin = false;

        // Enemies last wave.
        enemiesLastWavePhase = false;
        // Boss.
        mapHasBossPhase = false;
        bossPhase = false;
    }
}
