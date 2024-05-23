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
    // Tables:      Settings            |   CurrencyBalance     |   PlayerTalents
    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // Columns:     ID                  |   ID                  |
    //              FPSLimit            |   Crystals            |
    //              BackgroundMusic     |
    //

    const inline std::string dataBaseName = "MagneticBall3D.sqlite";
    void prepareDatabase(); // Always call at app launch.
    // const inline std::string deleteAllSettings = "DELETE FROM Settings;";
    //const inline std::string insertIntoBattlesProgress = "INSERT INTO BattlesProgress(ID, MaxAvailableBattle, AllBattlesFinished) "
    //                                                     "VALUES(NULL, :battleNumber, :allFinished);";

    // Settings.
    const inline std::string createTableSettings = "CREATE TABLE IF NOT EXISTS "
                                                   "Settings( "
                                                   "ID INTEGER PRIMARY KEY NOT NULL, "
                                                   "FPSLimit INTEGER, "
                                                   "BackgroundMusic INTEGER "
                                                   ");";

    const inline std::string insertFirstRowSettings = "INSERT INTO Settings(ID, FPSLimit, BackgroundMusic) VALUES(NULL, NULL, NULL);";
    const inline std::string selectSettingsAll = "SELECT * FROM Settings LIMIT 1;";
    const inline std::string updateSettingsFPSLimit = "UPDATE Settings SET FPSLimit = :FPS;";
    const inline std::string updateSettingsBackgroundMusic = "UPDATE Settings SET BackgroundMusic = :playMusic;";

    bool getIsSettingsTableEmpty();
    void readSettings();
    void storeSettingsFPSLimit(int value);
    void storeSettingsBackgroundMusic(bool playMusic);

    // CurrencyBalance.
    const inline std::string createTableCurrencyBalance = "CREATE TABLE IF NOT EXISTS "
                                                          "CurrencyBalance( "
                                                          "ID INTEGER PRIMARY KEY NOT NULL, "
                                                          "Crystals INTEGER);";

    const inline std::string insertFirstRowCurrencyBalance = "INSERT INTO CurrencyBalance(ID, Crystals) VALUES(NULL, NULL);";
    const inline std::string selectCurrencyBalanceCrystals = "SELECT Crystals FROM CurrencyBalance LIMIT 1;";
    const inline std::string updateCurrencyBalanceCrystals = "UPDATE CurrencyBalance SET Crystals = :crystals;";

    void readCurrencyBalance();
    void storeCurrencyBalanceCrystals(int value);
}
