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

    const inline std::string dataBaseName = "MagneticBall3D.sqlite";

    const inline std::string selectSettings = "SELECT FPSLimit FROM Settings LIMIT 1;";

    const inline std::string createTableSettings = "CREATE TABLE IF NOT EXISTS "
                                                   "Settings( "
                                                   "ID INTEGER PRIMARY KEY NOT NULL, "
                                                   "FPSLimit INTEGER);";

    const inline std::string deleteAllSettings = "DELETE FROM Settings;";

    //const inline std::string insertIntoBattlesProgress = "INSERT INTO BattlesProgress(ID, MaxAvailableBattle, AllBattlesFinished) "
    //                                                     "VALUES(NULL, :battleNumber, :allFinished);";

    const inline std::string insertFirstRowToSettings = "INSERT INTO Settings(ID, FPSLimit) VALUES(NULL, NULL);";

    const inline std::string updateFPSLimit = "UPDATE Settings SET FPSLimit = :FPS;";

    void readSettings();
    bool getIsSettingsTableEmpty();
    void storeFPSLimit(int value);
}
