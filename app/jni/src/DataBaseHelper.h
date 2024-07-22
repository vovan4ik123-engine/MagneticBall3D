#pragma once

#include "EngineHeaders.h"

namespace DataBaseHelper
{
    // Data types:
    //NULL - The value is a NULL value.
    //INTEGER - The value is a signed integer, stored in 0, 1, 2, 3, 4, 6, or 8 bytes depending on the magnitude of the value.
    //REAL - The value is a floating point value, stored as an 8-byte IEEE floating point number.
    //TEXT - The value is a text string, stored using the database encoding (UTF-8, UTF-16BE or UTF-16LE).
    //BLOB - The value is a blob of data, stored exactly as it was input.
    //Use INTEGER 0 or 1 For bool. Sqlite don't store bool.

    // Database schema.
    // Tables:      Settings            |   CurrencyBalance     |   MapsProgress        |   EnergySystem            |   DatabaseMigrations  | Shop
    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // Columns:     ID                  |   ID                  |   ID                  |   ID                      |   LastScriptApplied   | Item1FirstBuy
    //              FPSLimit            |   Crystals            |   CurrentMapIndex     |   CurrentAmount           |                       | Item2FirstBuy
    //              BackgroundMusic     |                       |   LastOpenedMapIndex  |   LastSecUpdated          |                       | Item3FirstBuy
    //              MeteorParticles     |                       |                       |   LastSecOneEnergyRestored|                       | Item4FirstBuy
    //                                                                                                              |                       | Item5FirstBuy
    //                                                                                                              |                       | Item6FirstBuy


    // Common data.
    const inline std::string dataBaseName = "MagneticBall3D.sqlite";
    void prepareDatabase(); // Always call at app launch.
    // const inline std::string deleteAllSettings = "DELETE FROM Settings;";
    //const inline std::string insertIntoBattlesProgress = "INSERT INTO BattlesProgress(ID, MaxAvailableBattle, AllBattlesFinished) "
    //                                                     "VALUES(NULL, :battleNumber, :allFinished);";

    void executeSqlWithLongLongParam(const std::string& sql, const char* paramName, long long int value);

    // Settings.
    const inline std::string createTableSettings = "CREATE TABLE IF NOT EXISTS "
                                                   "Settings( "
                                                   "ID INTEGER PRIMARY KEY NOT NULL, "
                                                   "FPSLimit INTEGER, "
                                                   "BackgroundMusic INTEGER, "
                                                   "MeteorParticles INTEGER "
                                                   ");";

    const inline std::string insertFirstRowSettings = "INSERT INTO Settings(ID, FPSLimit, BackgroundMusic, MeteorParticles) VALUES(NULL, NULL, NULL, NULL);";
    const inline std::string selectSettingsAll = "SELECT * FROM Settings LIMIT 1;";
    const inline std::string updateSettingsFPSLimit = "UPDATE Settings SET FPSLimit = :FPS;";
    const inline std::string updateSettingsBackgroundMusic = "UPDATE Settings SET BackgroundMusic = :playMusic;";
    const inline std::string updateSettingsMeteorParticles = "UPDATE Settings SET MeteorParticles = :showParticles;";

    bool getIsSettingsTableEmpty();
    void readSettings();
    inline void storeSettingsFPSLimit(long long int value)
    {
        executeSqlWithLongLongParam(updateSettingsFPSLimit, ":FPS", value);
    }
    inline void storeSettingsBackgroundMusic(bool playMusic)
    {
        long long int intValue = 0;
        if(playMusic)
            intValue = 1;

        executeSqlWithLongLongParam(updateSettingsBackgroundMusic, ":playMusic", intValue);
    }
    inline void storeSettingsMeteorParticles(bool showParticles)
    {
        long long int intValue = 0;
        if(showParticles)
            intValue = 1;

        executeSqlWithLongLongParam(updateSettingsMeteorParticles, ":showParticles", intValue);
    }

    // CurrencyBalance.
    const inline std::string createTableCurrencyBalance = "CREATE TABLE IF NOT EXISTS "
                                                          "CurrencyBalance( "
                                                          "ID INTEGER PRIMARY KEY NOT NULL, "
                                                          "Crystals INTEGER "
                                                          ");";

    const inline std::string insertFirstRowCurrencyBalance = "INSERT INTO CurrencyBalance(ID, Crystals) VALUES(NULL, NULL);";
    const inline std::string selectCurrencyBalanceCrystals = "SELECT * FROM CurrencyBalance LIMIT 1;";
    const inline std::string updateCurrencyBalanceCrystals = "UPDATE CurrencyBalance SET Crystals = :crystals;";

    void readCurrencyBalance();
    inline void storeCurrencyBalanceCrystals(long long int value)
    {
        if(value < 0)
            value = 0;

        executeSqlWithLongLongParam(updateCurrencyBalanceCrystals, ":crystals", value);
    }

    // MapsProgress.
    const inline std::string createTableMapsProgress = "CREATE TABLE IF NOT EXISTS "
                                                       "MapsProgress( "
                                                       "ID INTEGER PRIMARY KEY NOT NULL, "
                                                       "CurrentMapIndex INTEGER, "
                                                       "LastOpenedMapIndex INTEGER "
                                                       ");";

    const inline std::string insertFirstRowMapsProgress = "INSERT INTO MapsProgress(ID, CurrentMapIndex, LastOpenedMapIndex) VALUES(NULL, NULL, NULL);";
    const inline std::string selectMapsProgressAll = "SELECT * FROM MapsProgress LIMIT 1;";
    const inline std::string updateMapsProgressCurrentMapIndex = "UPDATE MapsProgress SET CurrentMapIndex = :currentMapIndex;";
    const inline std::string updateMapsProgressLastOpenedMapIndex = "UPDATE MapsProgress SET LastOpenedMapIndex = :lastOpenedIndex;";

    void readMapsProgress();
    inline void storeMapsProgressCurrentMapIndex(long long int value)
    {
        executeSqlWithLongLongParam(updateMapsProgressCurrentMapIndex, ":currentMapIndex", value);
    }
    inline void storeMapsProgressLastOpenedMapIndex(long long int value)
    {
        executeSqlWithLongLongParam(updateMapsProgressLastOpenedMapIndex, ":lastOpenedIndex", value);
    }

    // EnergySystem.
    const inline std::string createTableEnergySystem = "CREATE TABLE IF NOT EXISTS "
                                                       "EnergySystem( "
                                                       "ID INTEGER PRIMARY KEY NOT NULL, "
                                                       "CurrentAmount INTEGER, "
                                                       "LastSecUpdated INTEGER, "
                                                       "LastSecOneEnergyRestored INTEGER "
                                                       ");";

    const inline std::string insertFirstRowEnergySystem = "INSERT INTO EnergySystem(ID, CurrentAmount, LastSecUpdated, LastSecOneEnergyRestored) "
                                                          "VALUES(NULL, NULL, NULL, NULL);";
    const inline std::string selectEnergySystemAll = "SELECT * FROM EnergySystem LIMIT 1;";
    const inline std::string updateEnergySystemCurrentAmount = "UPDATE EnergySystem SET CurrentAmount = :amount;";
    const inline std::string updateEnergySystemLastSecUpdated = "UPDATE EnergySystem SET LastSecUpdated = :lastSecUpdated;";
    const inline std::string updateEnergySystemLastSecRestored = "UPDATE EnergySystem SET LastSecOneEnergyRestored = :lastSecOneEnergyRestored;";

    void readEnergySystem();
    inline void storeEnergySystemCurrentAmount(long long int value)
    {
        executeSqlWithLongLongParam(updateEnergySystemCurrentAmount, ":amount", value);
    }
    inline void storeEnergySystemLastSecUpdated(long long int value)
    {
        executeSqlWithLongLongParam(updateEnergySystemLastSecUpdated, ":lastSecUpdated", value);
    }
    inline void storeEnergySystemLastSecRestored(long long int value)
    {
        executeSqlWithLongLongParam(updateEnergySystemLastSecRestored, ":lastSecOneEnergyRestored", value);
    }

    // Shop.
    const inline std::string createTableShop = "CREATE TABLE IF NOT EXISTS "
                                               "Shop( "
                                               "ID INTEGER PRIMARY KEY NOT NULL, "
                                               "Item1FirstBuy INTEGER, "
                                               "Item2FirstBuy INTEGER, "
                                               "Item3FirstBuy INTEGER, "
                                               "Item4FirstBuy INTEGER, "
                                               "Item5FirstBuy INTEGER, "
                                               "Item6FirstBuy INTEGER "
                                               ");";

    const inline std::string insertFirstRowShop = "INSERT INTO Shop(ID, Item1FirstBuy, Item2FirstBuy, Item3FirstBuy, Item4FirstBuy, Item5FirstBuy, Item6FirstBuy) "
                                                  "VALUES(NULL, NULL, NULL, NULL, NULL, NULL, NULL);";
    const inline std::string selectShopAll = "SELECT * FROM Shop LIMIT 1;";
    const inline std::string updateShopItem1FirstBuy = "UPDATE Shop SET Item1FirstBuy = :item1FirstBuy;";
    const inline std::string updateShopItem2FirstBuy = "UPDATE Shop SET Item2FirstBuy = :item2FirstBuy;";
    const inline std::string updateShopItem3FirstBuy = "UPDATE Shop SET Item3FirstBuy = :item3FirstBuy;";
    const inline std::string updateShopItem4FirstBuy = "UPDATE Shop SET Item4FirstBuy = :item4FirstBuy;";
    const inline std::string updateShopItem5FirstBuy = "UPDATE Shop SET Item5FirstBuy = :item5FirstBuy;";
    const inline std::string updateShopItem6FirstBuy = "UPDATE Shop SET Item6FirstBuy = :item6FirstBuy;";

    void readShop();
    inline void storeShopItem1FirstBuy(long long int value)
    {
        executeSqlWithLongLongParam(updateShopItem1FirstBuy, ":item1FirstBuy", value);
    }
    inline void storeShopItem2FirstBuy(long long int value)
    {
        executeSqlWithLongLongParam(updateShopItem2FirstBuy, ":item2FirstBuy", value);
    }
    inline void storeShopItem3FirstBuy(long long int value)
    {
        executeSqlWithLongLongParam(updateShopItem3FirstBuy, ":item3FirstBuy", value);
    }
    inline void storeShopItem4FirstBuy(long long int value)
    {
        executeSqlWithLongLongParam(updateShopItem4FirstBuy, ":item4FirstBuy", value);
    }
    inline void storeShopItem5FirstBuy(long long int value)
    {
        executeSqlWithLongLongParam(updateShopItem5FirstBuy, ":item5FirstBuy", value);
    }
    inline void storeShopItem6FirstBuy(long long int value)
    {
        executeSqlWithLongLongParam(updateShopItem6FirstBuy, ":item6FirstBuy", value);
    }

    // DatabaseMigrations.
    const inline std::string createTableDatabaseMigrations = "CREATE TABLE IF NOT EXISTS "
                                                             "DatabaseMigrations( "
                                                             "ID INTEGER PRIMARY KEY NOT NULL, "
                                                             "LastScriptApplied INTEGER "
                                                             ");";

    const inline std::string insertFirstRowDatabaseMigrations = "INSERT INTO DatabaseMigrations(ID, LastScriptApplied) VALUES(NULL, NULL);";
    const inline std::string selectDatabaseMigrationsAll = "SELECT * FROM DatabaseMigrations LIMIT 1;";
    const inline std::string updateDatabaseMigrationsLastScriptApplied = "UPDATE DatabaseMigrations SET LastScriptApplied = :lastScript;";

    void checkDatabaseMigrations();
    inline void storeDatabaseMigrationsLastScriptApplied(long long int value)
    {
        executeSqlWithLongLongParam(updateDatabaseMigrationsLastScriptApplied, ":lastScript", value);
    }
}
