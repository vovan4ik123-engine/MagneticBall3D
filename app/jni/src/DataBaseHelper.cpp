#include "DataBaseHelper.h"
#include "EnumsAndVariables.h"

namespace DataBaseHelper
{
    bool getIsSettingsTableEmpty()
    {
        try
        {
            Beryll::DataBase::openDataBase(dataBaseName);
            Beryll::DataBase::setSqlQuery(selectSettingsAll);
            auto rows = Beryll::DataBase::executeSelectQuery();

            return rows.empty();
        }
        catch(const Beryll::DataBaseException& e)
        {
            std::string what = e.what();
            BR_ERROR("DataBaseException %s", what.c_str());
        }
        catch(const std::exception& e)
        {
            std::string what = e.what();
            BR_ERROR("std::exception %s", what.c_str());
        }

        return true;
    }

    void prepareDatabase()
    {
        // Table Settings is main table.
        // If it does not exist app is launched first time on device.
        // Create all tables we need for app.
        if(getIsSettingsTableEmpty())
        {
            BR_INFO("%s", "Create database tables + first rows. Looks like first app launch.");

            try
            {
                // Create table Settings and insert 1 row.
                executeSql(createTableSettings);
                executeSql(insertSettings);
                storeSettingsFPSLimit(EnumsAndVars::SettingsMenu::FPSLimit);
                storeSettingsBackgroundMusic(EnumsAndVars::SettingsMenu::backgroundMusic);
                storeSettingsMeteorParticles(EnumsAndVars::SettingsMenu::meteorParticles);

                // Create table CurrencyBalance and insert 1 row.
                executeSql(createTableCurrencyBalance);
                executeSql(insertCurrencyBalance);
                storeCurrencyBalanceCrystals(EnumsAndVars::CurrencyBalance::crystals);

                // MapsProgress.
                executeSql(createTableMapsProgress);
                executeSql(insertMapsProgress);
                storeMapsProgressCurrentMapIndex(EnumsAndVars::MapsProgress::currentMapIndex);
                storeMapsProgressLastOpenedMapIndex(EnumsAndVars::MapsProgress::lastOpenedMapIndex);

                // EnergySystem.
                executeSql(createTableEnergySystem);
                executeSql(insertEnergySystem);
                storeEnergySystemCurrentAmount(EnumsAndVars::EnergySystem::currentAmount);
                storeEnergySystemLastSecUpdated(EnumsAndVars::EnergySystem::lastSecUpdated);
                storeEnergySystemLastSecRestored(EnumsAndVars::EnergySystem::lastSecOneEnergyRestored);

                // Shop.
                executeSql(createTableShop);
                executeSql(insertShop);
                storeShopItem1FirstBuy(1);
                storeShopItem2FirstBuy(1);
                storeShopItem3FirstBuy(1);
                storeShopItem4FirstBuy(1);
                storeShopItem5FirstBuy(1);
                storeShopItem6FirstBuy(1);

                // DatabaseMigrations.
                executeSql(createTableDatabaseMigrations);
                executeSql(insertDatabaseMigrations);
                storeDatabaseMigrationsLastScriptApplied(0);

                // Player talents.
                executeSql(createTablePlayerTalents);
                for(const auto& talent : EnumsAndVars::allPlayerTalents)
                {
                    insertPlayerTalentsTalent(talent.name, talent.currentLevel);
                }

                // Game difficulty.
                executeSql(createTableGameDifficulty);
                executeSql(insertGameDifficulty);
                storeGameDifficultyLevel(EnumsAndVars::GameDifficulty::level);

                // Ads.
                executeSql(createTableAds);
                executeSql(insertAds);
                storeAdsRewardedAdTime(EnumsAndVars::Ads::rewardedAdTime);
            }
            catch(const Beryll::DataBaseException& e)
            {
                std::string what = e.what();
                BR_ASSERT(false, "DataBaseException %s", what.c_str());
            }
            catch(const std::exception& e)
            {
                std::string what = e.what();
                BR_ASSERT(false, "std::exception %s", what.c_str());
            }
        }
        else
        {
            BR_INFO("%s", "Read database tables at app launch.");

            checkDatabaseMigrations();

            readSettings();
            readCurrencyBalance();
            readMapsProgress();
            readEnergySystem();
            readShop();
            readPlayerTalents();
            readGameDifficulty();
            readAds();
        }
    }

    void readSettings()
    {
        try
        {
            Beryll::DataBase::setSqlQuery(selectSettingsAll);
            std::vector<std::vector<std::variant<long long int, double, std::string, Beryll::SqliteNULL>>> rows = Beryll::DataBase::executeSelectQuery();
            BR_ASSERT((!rows.empty() && !rows[0].empty()), "%s", "readSettings() rows are empty.");

            BR_INFO("readSettings() rows: %d columns: %d", rows.size(), rows[0].size());

            BR_ASSERT((std::holds_alternative<long long int>(rows[0][0])), "%s", "ID INTEGER PRIMARY KEY contains wrong data.");

            BR_ASSERT((std::holds_alternative<long long int>(rows[0][1])), "%s", "FPSLimit contains wrong data.");
            if(std::holds_alternative<long long int>(rows[0][1]))
                EnumsAndVars::SettingsMenu::FPSLimit = std::get<long long int>(rows[0][1]);
            BR_INFO("FPSLimit after read: %d", EnumsAndVars::SettingsMenu::FPSLimit);

            BR_ASSERT((std::holds_alternative<long long int>(rows[0][2])), "%s", "BackgroundMusic contains wrong data.");
            if(std::holds_alternative<long long int>(rows[0][2]))
                EnumsAndVars::SettingsMenu::backgroundMusic = std::get<long long int>(rows[0][2]) == 1; // True if column contains 1.
            BR_INFO("backgroundMusic after read: %d", int(EnumsAndVars::SettingsMenu::backgroundMusic));

            BR_ASSERT((std::holds_alternative<long long int>(rows[0][3])), "%s", "MeteorParticles contains wrong data.");
            if(std::holds_alternative<long long int>(rows[0][3]))
                EnumsAndVars::SettingsMenu::meteorParticles = std::get<long long int>(rows[0][3]) == 1; // True if column contains 1.
            BR_INFO("meteorParticles after read: %d", int(EnumsAndVars::SettingsMenu::meteorParticles));
        }
        catch(const Beryll::DataBaseException& e)
        {
            std::string what = e.what();
            BR_ERROR("DataBaseException %s", what.c_str());
        }
        catch(const std::exception& e)
        {
            std::string what = e.what();
            BR_ERROR("std::exception %s", what.c_str());
        }
    }

    void readCurrencyBalance()
    {
        try
        {
            Beryll::DataBase::setSqlQuery(selectCurrencyBalanceCrystals);
            std::vector<std::vector<std::variant<long long int, double, std::string, Beryll::SqliteNULL>>> rows = Beryll::DataBase::executeSelectQuery();
            BR_ASSERT((!rows.empty() && !rows[0].empty()), "%s", "readCurrencyBalance() rows are empty.");

            BR_INFO("readCurrencyBalance() rows: %d columns: %d", rows.size(), rows[0].size());

            BR_ASSERT((std::holds_alternative<long long int>(rows[0][0])), "%s", "ID INTEGER PRIMARY KEY contains wrong data.");

            BR_ASSERT((std::holds_alternative<long long int>(rows[0][1])), "%s", "Result of selectCurrencyBalanceCrystals contains wrong data.");
            if(std::holds_alternative<long long int>(rows[0][1]))
                EnumsAndVars::CurrencyBalance::crystals = std::get<long long int>(rows[0][1]);

            BR_INFO("crystals after read: %d", EnumsAndVars::CurrencyBalance::crystals);
        }
        catch(const Beryll::DataBaseException& e)
        {
            std::string what = e.what();
            BR_ERROR("DataBaseException %s", what.c_str());
        }
        catch(const std::exception& e)
        {
            std::string what = e.what();
            BR_ERROR("std::exception %s", what.c_str());
        }
    }

    void readMapsProgress()
    {
        try
        {
            Beryll::DataBase::setSqlQuery(selectMapsProgressAll);
            std::vector<std::vector<std::variant<long long int, double, std::string, Beryll::SqliteNULL>>> rows = Beryll::DataBase::executeSelectQuery();
            BR_ASSERT((!rows.empty() && !rows[0].empty()), "%s", "readMapsProgress() rows are empty.");

            BR_INFO("readMapsProgress() rows: %d columns: %d", rows.size(), rows[0].size());

            BR_ASSERT((std::holds_alternative<long long int>(rows[0][0])), "%s", "ID INTEGER PRIMARY KEY contains wrong data.");

            BR_ASSERT((std::holds_alternative<long long int>(rows[0][1])), "%s", "CurrentMapIndex contains wrong data.");
            if(std::holds_alternative<long long int>(rows[0][1]))
                EnumsAndVars::MapsProgress::currentMapIndex = std::get<long long int>(rows[0][1]);
            BR_INFO("currentMapIndex after read: %d", EnumsAndVars::MapsProgress::currentMapIndex);

            BR_ASSERT((std::holds_alternative<long long int>(rows[0][2])), "%s", "LastOpenedMapIndex contains wrong data.");
            if(std::holds_alternative<long long int>(rows[0][2]))
                EnumsAndVars::MapsProgress::lastOpenedMapIndex = std::get<long long int>(rows[0][2]);
            BR_INFO("lastOpenedMapIndex after read: %d", EnumsAndVars::MapsProgress::lastOpenedMapIndex);

            // EnumsAndVars::MapsProgress::maxMapIndex is hardcoded and const. Should not be changed during game.
        }
        catch(const Beryll::DataBaseException& e)
        {
            std::string what = e.what();
            BR_ERROR("DataBaseException %s", what.c_str());
        }
        catch(const std::exception& e)
        {
            std::string what = e.what();
            BR_ERROR("std::exception %s", what.c_str());
        }
    }

    void readEnergySystem()
    {
        try
        {
            Beryll::DataBase::setSqlQuery(selectEnergySystemAll);
            std::vector<std::vector<std::variant<long long int, double, std::string, Beryll::SqliteNULL>>> rows = Beryll::DataBase::executeSelectQuery();
            BR_ASSERT((!rows.empty() && !rows[0].empty()), "%s", "readEnergySystem() rows are empty.");

            BR_INFO("readEnergySystem() rows: %d columns: %d", rows.size(), rows[0].size());

            BR_ASSERT((std::holds_alternative<long long int>(rows[0][0])), "%s", "ID INTEGER PRIMARY KEY contains wrong data.");

            BR_ASSERT((std::holds_alternative<long long int>(rows[0][1])), "%s", "CurrentAmount contains wrong data.");
            if(std::holds_alternative<long long int>(rows[0][1]))
                EnumsAndVars::EnergySystem::currentAmount = std::get<long long int>(rows[0][1]);
            BR_INFO("currentAmount after read: %d", EnumsAndVars::EnergySystem::currentAmount);

            BR_ASSERT((std::holds_alternative<long long int>(rows[0][2])), "%s", "LastSecUpdated contains wrong data.");
            if(std::holds_alternative<long long int>(rows[0][2]))
                EnumsAndVars::EnergySystem::lastSecUpdated = std::get<long long int>(rows[0][2]);
            BR_INFO("lastSecUpdated after read: %d", EnumsAndVars::EnergySystem::lastSecUpdated);

            BR_ASSERT((std::holds_alternative<long long int>(rows[0][3])), "%s", "LastSecOneEnergyRestored contains wrong data.");
            if(std::holds_alternative<long long int>(rows[0][3]))
                EnumsAndVars::EnergySystem::lastSecOneEnergyRestored = std::get<long long int>(rows[0][3]);
            BR_INFO("lastSecOneEnergyRestored after read: %d", EnumsAndVars::EnergySystem::lastSecOneEnergyRestored);
        }
        catch(const Beryll::DataBaseException& e)
        {
            std::string what = e.what();
            BR_ERROR("DataBaseException %s", what.c_str());
        }
        catch(const std::exception& e)
        {
            std::string what = e.what();
            BR_ERROR("std::exception %s", what.c_str());
        }
    }

    void readShop()
    {
        try
        {
            Beryll::DataBase::setSqlQuery(selectShopAll);
            std::vector<std::vector<std::variant<long long int, double, std::string, Beryll::SqliteNULL>>> rows = Beryll::DataBase::executeSelectQuery();
            BR_ASSERT((!rows.empty() && !rows[0].empty()), "%s", "readShop() rows are empty.");

            BR_INFO("readShop() rows: %d columns: %d", rows.size(), rows[0].size());

            BR_ASSERT((std::holds_alternative<long long int>(rows[0][0])), "%s", "ID INTEGER PRIMARY KEY contains wrong data.");

            BR_ASSERT((std::holds_alternative<long long int>(rows[0][1])), "%s", "Item1FirstBuy contains wrong data.");
            if(std::holds_alternative<long long int>(rows[0][1]))
                EnumsAndVars::Shop::item1FirstBuy = std::get<long long int>(rows[0][1]) == 1; // True if column contains 1.
            BR_INFO("Item1FirstBuy after read: %d", int(EnumsAndVars::Shop::item1FirstBuy));

            BR_ASSERT((std::holds_alternative<long long int>(rows[0][2])), "%s", "Item2FirstBuy contains wrong data.");
            if(std::holds_alternative<long long int>(rows[0][2]))
                EnumsAndVars::Shop::item2FirstBuy = std::get<long long int>(rows[0][2]) == 1; // True if column contains 1.
            BR_INFO("Item2FirstBuy after read: %d", int(EnumsAndVars::Shop::item2FirstBuy));

            BR_ASSERT((std::holds_alternative<long long int>(rows[0][3])), "%s", "Item3FirstBuy contains wrong data.");
            if(std::holds_alternative<long long int>(rows[0][3]))
                EnumsAndVars::Shop::item3FirstBuy = std::get<long long int>(rows[0][3]) == 1; // True if column contains 1.
            BR_INFO("Item3FirstBuy after read: %d", int(EnumsAndVars::Shop::item3FirstBuy));

            BR_ASSERT((std::holds_alternative<long long int>(rows[0][4])), "%s", "Item4FirstBuy contains wrong data.");
            if(std::holds_alternative<long long int>(rows[0][4]))
                EnumsAndVars::Shop::item4FirstBuy = std::get<long long int>(rows[0][4]) == 1; // True if column contains 1.
            BR_INFO("Item4FirstBuy after read: %d", int(EnumsAndVars::Shop::item4FirstBuy));

            BR_ASSERT((std::holds_alternative<long long int>(rows[0][5])), "%s", "Item5FirstBuy contains wrong data.");
            if(std::holds_alternative<long long int>(rows[0][5]))
                EnumsAndVars::Shop::item5FirstBuy = std::get<long long int>(rows[0][5]) == 1; // True if column contains 1.
            BR_INFO("Item5FirstBuy after read: %d", int(EnumsAndVars::Shop::item5FirstBuy));

            BR_ASSERT((std::holds_alternative<long long int>(rows[0][6])), "%s", "Item6FirstBuy contains wrong data.");
            if(std::holds_alternative<long long int>(rows[0][6]))
                EnumsAndVars::Shop::item6FirstBuy = std::get<long long int>(rows[0][6]) == 1; // True if column contains 1.
            BR_INFO("Item6FirstBuy after read: %d", int(EnumsAndVars::Shop::item6FirstBuy));
        }
        catch(const Beryll::DataBaseException& e)
        {
            std::string what = e.what();
            BR_ERROR("DataBaseException %s", what.c_str());
        }
        catch(const std::exception& e)
        {
            std::string what = e.what();
            BR_ERROR("std::exception %s", what.c_str());
        }
    }

    void checkDatabaseMigrations()
    {
        try
        {
            Beryll::DataBase::setSqlQuery(selectDatabaseMigrationsAll);
            std::vector<std::vector<std::variant<long long int, double, std::string, Beryll::SqliteNULL>>> rows = Beryll::DataBase::executeSelectQuery();
            BR_ASSERT((!rows.empty() && !rows[0].empty()), "%s", "checkDatabaseMigrations() rows are empty.");

            BR_INFO("checkDatabaseMigrations() rows: %d columns: %d", rows.size(), rows[0].size());

            BR_ASSERT((std::holds_alternative<long long int>(rows[0][0])), "%s", "ID INTEGER PRIMARY KEY contains wrong data.");

            BR_ASSERT((std::holds_alternative<long long int>(rows[0][1])), "%s", "LastScriptApplied contains wrong data.");
            int lastScriptAppliedIndex = 0;
            if (std::holds_alternative<long long int>(rows[0][1]))
                lastScriptAppliedIndex = std::get<long long int>(rows[0][1]);
            BR_INFO("DatabaseMigrations lastScriptAppliedIndex: %d", lastScriptAppliedIndex);

            //        if(lastScriptAppliedIndex < 1)
            //        {
            //            applyDatabaseMigrationsScript1();
            //            storeDatabaseMigrationsLastScriptApplied(1);
            //        }
            //        // No else here. Only new if().
            //        if(lastScriptAppliedIndex < 2)
            //        {
            //            applyDatabaseMigrationsScript2();
            //            storeDatabaseMigrationsLastScriptApplied(2);
            //        }
            //        // No else here. Only new if().
        }
        catch(const Beryll::DataBaseException& e)
        {
            std::string what = e.what();
            BR_ERROR("DataBaseException %s", what.c_str());
        }
        catch(const std::exception& e)
        {
            std::string what = e.what();
            BR_ERROR("std::exception %s", what.c_str());
        }
    }

    void readPlayerTalents()
    {
        try
        {
            Beryll::DataBase::setSqlQuery(selectPlayerTalentsAll);
            std::vector<std::vector<std::variant<long long int, double, std::string, Beryll::SqliteNULL>>> rows = Beryll::DataBase::executeSelectQuery();
            BR_ASSERT((!rows.empty() && !rows[0].empty()), "%s", "readPlayerTalents() rows are empty.");

            BR_INFO("readPlayerTalents() rows: %d columns: %d", rows.size(), rows[0].size());

            for(int i = 0; i <rows.size(); ++i)
            {
                BR_ASSERT((std::holds_alternative<long long int>(rows[i][0])), "%s", "ID INTEGER PRIMARY KEY contains wrong data.");

                BR_ASSERT((std::holds_alternative<std::string>(rows[i][1]) && std::holds_alternative<long long int>(rows[i][2])),
                          "%s", "Name or CurrentLevel contains wrong data.");
                if(std::holds_alternative<std::string>(rows[i][1]) && std::holds_alternative<long long int>(rows[i][2]))
                {
                    for(auto& talent : EnumsAndVars::allPlayerTalents)
                    {
                        if(talent.name == std::get<std::string>(rows[i][1]))
                        {
                            talent.currentLevel = std::get<long long int>(rows[i][2]);
                            BR_INFO("Read talent:%s. Level:%d", talent.name.c_str(),talent.currentLevel);
                            break;
                        }
                    }
                }
            }
        }
        catch(const Beryll::DataBaseException& e)
        {
            std::string what = e.what();
            BR_ERROR("DataBaseException %s", what.c_str());
        }
        catch(const std::exception& e)
        {
            std::string what = e.what();
            BR_ERROR("std::exception %s", what.c_str());
        }
    }

    void readGameDifficulty()
    {
        try
        {
            Beryll::DataBase::setSqlQuery(selectGameDifficultyAll);
            std::vector<std::vector<std::variant<long long int, double, std::string, Beryll::SqliteNULL>>> rows = Beryll::DataBase::executeSelectQuery();
            BR_ASSERT((!rows.empty() && !rows[0].empty()), "%s", "readGameDifficulty() rows are empty.");

            BR_INFO("readGameDifficulty() rows: %d columns: %d", rows.size(), rows[0].size());

            BR_ASSERT((std::holds_alternative<long long int>(rows[0][0])), "%s", "ID INTEGER PRIMARY KEY contains wrong data.");

            BR_ASSERT((std::holds_alternative<long long int>(rows[0][1])), "%s", "Level contains wrong data.");
            if(std::holds_alternative<long long int>(rows[0][1]))
                EnumsAndVars::GameDifficulty::level = std::get<long long int>(rows[0][1]);
            BR_INFO("GameDifficulty::level after read: %d", EnumsAndVars::GameDifficulty::level);
        }
        catch(const Beryll::DataBaseException& e)
        {
            std::string what = e.what();
            BR_ERROR("DataBaseException %s", what.c_str());
        }
        catch(const std::exception& e)
        {
            std::string what = e.what();
            BR_ERROR("std::exception %s", what.c_str());
        }
    }

    void readAds()
    {
        try
        {
            Beryll::DataBase::setSqlQuery(selectAdsAll);
            std::vector<std::vector<std::variant<long long int, double, std::string, Beryll::SqliteNULL>>> rows = Beryll::DataBase::executeSelectQuery();
            BR_ASSERT((!rows.empty() && !rows[0].empty()), "%s", "readAds() rows are empty.");

            BR_INFO("readAds() rows: %d columns: %d", rows.size(), rows[0].size());

            BR_ASSERT((std::holds_alternative<long long int>(rows[0][0])), "%s", "ID INTEGER PRIMARY KEY contains wrong data.");

            BR_ASSERT((std::holds_alternative<long long int>(rows[0][1])), "%s", "RewardedAdTime contains wrong data.");
            if(std::holds_alternative<long long int>(rows[0][1]))
                EnumsAndVars::Ads::rewardedAdTime = std::get<long long int>(rows[0][1]);
            BR_INFO("Ads::rewardedAdTime after read: %d", EnumsAndVars::Ads::rewardedAdTime);
        }
        catch(const Beryll::DataBaseException& e)
        {
            std::string what = e.what();
            BR_ERROR("DataBaseException %s", what.c_str());
        }
        catch(const std::exception& e)
        {
            std::string what = e.what();
            BR_ERROR("std::exception %s", what.c_str());
        }
    }

    void executeSql(const std::string& sql)
    {
        BR_INFO("executeSql() %s", sql.c_str());
        try
        {
            Beryll::DataBase::setSqlQuery(sql);
            Beryll::DataBase::executeNotSelectQuery();
        }
        catch(const Beryll::DataBaseException& e)
        {
            std::string what = e.what();
            BR_ASSERT(false, "DataBaseException %s", what.c_str());
        }
        catch(const std::exception& e)
        {
            std::string what = e.what();
            BR_ASSERT(false, "std::exception %s", what.c_str());
        }
    }
}
