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
                storeShopAdsOnMapsDisabled(0);

                // DatabaseMigrations.
                executeSql(createTableDatabaseMigrations);
                executeSql(insertDatabaseMigrations);
                storeDatabaseMigrationsLastScriptApplied(0);

                // Player talents.
                executeSql(createTablePlayerTalents);
                BR_ASSERT((EnumsAndVars::allPlayerTalents.size() >= 6), "%s", "allPlayerTalents has wrong size.")
                insertPlayerTalentsTalent(EnumsAndVars::allPlayerTalents[0].name, EnumsAndVars::allPlayerTalents[0].currentLevel);
                insertPlayerTalentsTalent(EnumsAndVars::allPlayerTalents[1].name, EnumsAndVars::allPlayerTalents[1].currentLevel);
                insertPlayerTalentsTalent(EnumsAndVars::allPlayerTalents[2].name, EnumsAndVars::allPlayerTalents[2].currentLevel);
                insertPlayerTalentsTalent(EnumsAndVars::allPlayerTalents[3].name, EnumsAndVars::allPlayerTalents[3].currentLevel);
                insertPlayerTalentsTalent(EnumsAndVars::allPlayerTalents[4].name, EnumsAndVars::allPlayerTalents[4].currentLevel);
                insertPlayerTalentsTalent(EnumsAndVars::allPlayerTalents[5].name, EnumsAndVars::allPlayerTalents[5].currentLevel);

                // Game difficulty.
                executeSql(createTableGameDifficulty);
                executeSql(insertGameDifficulty);
                storeGameDifficultyLevel(EnumsAndVars::GameDifficulty::level);

                // Ads.
                executeSql(createTableAds);
                executeSql(insertAds);
                storeAdsRewardedAdTime(EnumsAndVars::Ads::rewardedAdTime);

                // Daily Reward.
                executeSql(createTableDailyReward);
                executeSql(insertDailyReward);
                storeDailyRewardTookTime(EnumsAndVars::DailyReward::tookTime);
                storeDailyRewardTookDay(EnumsAndVars::DailyReward::tookDay);

                applyDatabaseMigrations();
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

            applyDatabaseMigrations();

            readSettings();
            readCurrencyBalance();
            readMapsProgress();
            readEnergySystem();
            readShop();
            readPlayerTalents();
            readGameDifficulty();
            readAds();
            readDailyReward();
        }
    }

    void readSettings()
    {
        std::vector<std::vector<std::variant<long long int, double, std::string, Beryll::SqliteNULL>>> rows = executeSqlSelect(selectSettingsAll);
        BR_ASSERT((!rows.empty() && !rows[0].empty()), "%s", "readSettings() rows are empty.");

        BR_INFO("readSettings() rows: %d columns: %d", rows.size(), rows[0].size());

        BR_ASSERT((std::holds_alternative<long long int>(rows[0][0])), "%s", "ID INTEGER PRIMARY KEY contains wrong data.");

        BR_ASSERT((std::holds_alternative<long long int>(rows[0][1])), "%s", "FPSLimit contains wrong data.");
        if(std::holds_alternative<long long int>(rows[0][1]))
            EnumsAndVars::SettingsMenu::FPSLimit = std::get<long long int>(rows[0][1]);
        BR_INFO("SettingsMenu::FPSLimit after read: %d", EnumsAndVars::SettingsMenu::FPSLimit);

        BR_ASSERT((std::holds_alternative<long long int>(rows[0][2])), "%s", "BackgroundMusic contains wrong data.");
        if(std::holds_alternative<long long int>(rows[0][2]))
            EnumsAndVars::SettingsMenu::backgroundMusic = std::get<long long int>(rows[0][2]) == 1; // True if column contains 1.
        BR_INFO("SettingsMenu::backgroundMusic after read: %d", int(EnumsAndVars::SettingsMenu::backgroundMusic));

        BR_ASSERT((std::holds_alternative<long long int>(rows[0][3])), "%s", "MeteorParticles contains wrong data.");
        if(std::holds_alternative<long long int>(rows[0][3]))
            EnumsAndVars::SettingsMenu::meteorParticles = std::get<long long int>(rows[0][3]) == 1; // True if column contains 1.
        BR_INFO("SettingsMenu::meteorParticles after read: %d", int(EnumsAndVars::SettingsMenu::meteorParticles));

        BR_ASSERT((std::holds_alternative<long long int>(rows[0][4])), "%s", "InterfaceGUI contains wrong data.");
        if(std::holds_alternative<long long int>(rows[0][4]))
            EnumsAndVars::SettingsMenu::interfaceGUI = std::get<long long int>(rows[0][4]) == 1; // True if column contains 1.
        BR_INFO("SettingsMenu::interfaceGUI after read: %d", int(EnumsAndVars::SettingsMenu::interfaceGUI));
    }

    void readCurrencyBalance()
    {
        std::vector<std::vector<std::variant<long long int, double, std::string, Beryll::SqliteNULL>>> rows = executeSqlSelect(selectCurrencyBalanceCrystals);
        BR_ASSERT((!rows.empty() && !rows[0].empty()), "%s", "readCurrencyBalance() rows are empty.");

        BR_INFO("readCurrencyBalance() rows: %d columns: %d", rows.size(), rows[0].size());

        BR_ASSERT((std::holds_alternative<long long int>(rows[0][0])), "%s", "ID INTEGER PRIMARY KEY contains wrong data.");

        BR_ASSERT((std::holds_alternative<long long int>(rows[0][1])), "%s", "Crystals contains wrong data.");
        if(std::holds_alternative<long long int>(rows[0][1]))
            EnumsAndVars::CurrencyBalance::crystals = std::get<long long int>(rows[0][1]);
        BR_INFO("CurrencyBalance::crystals after read: %d", EnumsAndVars::CurrencyBalance::crystals);
    }

    void readMapsProgress()
    {
        std::vector<std::vector<std::variant<long long int, double, std::string, Beryll::SqliteNULL>>> rows = executeSqlSelect(selectMapsProgressAll);
        BR_ASSERT((!rows.empty() && !rows[0].empty()), "%s", "readMapsProgress() rows are empty.");

        BR_INFO("readMapsProgress() rows: %d columns: %d", rows.size(), rows[0].size());

        BR_ASSERT((std::holds_alternative<long long int>(rows[0][0])), "%s", "ID INTEGER PRIMARY KEY contains wrong data.");

        BR_ASSERT((std::holds_alternative<long long int>(rows[0][1])), "%s", "CurrentMapIndex contains wrong data.");
        if(std::holds_alternative<long long int>(rows[0][1]))
            EnumsAndVars::MapsProgress::currentMapIndex = std::get<long long int>(rows[0][1]);
        BR_INFO("MapsProgress::currentMapIndex after read: %d", EnumsAndVars::MapsProgress::currentMapIndex);

        BR_ASSERT((std::holds_alternative<long long int>(rows[0][2])), "%s", "LastOpenedMapIndex contains wrong data.");
        if(std::holds_alternative<long long int>(rows[0][2]))
            EnumsAndVars::MapsProgress::lastOpenedMapIndex = std::get<long long int>(rows[0][2]);
        BR_INFO("MapsProgress::lastOpenedMapIndex after read: %d", EnumsAndVars::MapsProgress::lastOpenedMapIndex);

        // EnumsAndVars::MapsProgress::maxMapIndex is hardcoded and const. Should not be changed during game.
    }

    void readEnergySystem()
    {
        std::vector<std::vector<std::variant<long long int, double, std::string, Beryll::SqliteNULL>>> rows = executeSqlSelect(selectEnergySystemAll);
        BR_ASSERT((!rows.empty() && !rows[0].empty()), "%s", "readEnergySystem() rows are empty.");

        BR_INFO("readEnergySystem() rows: %d columns: %d", rows.size(), rows[0].size());

        BR_ASSERT((std::holds_alternative<long long int>(rows[0][0])), "%s", "ID INTEGER PRIMARY KEY contains wrong data.");

        BR_ASSERT((std::holds_alternative<long long int>(rows[0][1])), "%s", "CurrentAmount contains wrong data.");
        if(std::holds_alternative<long long int>(rows[0][1]))
            EnumsAndVars::EnergySystem::currentAmount = std::get<long long int>(rows[0][1]);
        BR_INFO("EnergySystem::currentAmount after read: %d", EnumsAndVars::EnergySystem::currentAmount);

        BR_ASSERT((std::holds_alternative<long long int>(rows[0][2])), "%s", "LastSecUpdated contains wrong data.");
        if(std::holds_alternative<long long int>(rows[0][2]))
            EnumsAndVars::EnergySystem::lastSecUpdated = std::get<long long int>(rows[0][2]);
        BR_INFO("EnergySystem::lastSecUpdated after read: %d", EnumsAndVars::EnergySystem::lastSecUpdated);

        BR_ASSERT((std::holds_alternative<long long int>(rows[0][3])), "%s", "LastSecOneEnergyRestored contains wrong data.");
        if(std::holds_alternative<long long int>(rows[0][3]))
            EnumsAndVars::EnergySystem::lastSecOneEnergyRestored = std::get<long long int>(rows[0][3]);
        BR_INFO("EnergySystem::lastSecOneEnergyRestored after read: %d", EnumsAndVars::EnergySystem::lastSecOneEnergyRestored);
    }

    void readShop()
    {
        std::vector<std::vector<std::variant<long long int, double, std::string, Beryll::SqliteNULL>>> rows = executeSqlSelect(selectShopAll);
        BR_ASSERT((!rows.empty() && !rows[0].empty()), "%s", "readShop() rows are empty.");

        BR_INFO("readShop() rows: %d columns: %d", rows.size(), rows[0].size());

        BR_ASSERT((std::holds_alternative<long long int>(rows[0][0])), "%s", "ID INTEGER PRIMARY KEY contains wrong data.");

        BR_ASSERT((std::holds_alternative<long long int>(rows[0][1])), "%s", "Item1FirstBuy contains wrong data.");
        if(std::holds_alternative<long long int>(rows[0][1]))
            EnumsAndVars::Shop::item1FirstBuy = std::get<long long int>(rows[0][1]) == 1; // True if column contains 1.
        BR_INFO("Shop::item1FirstBuy after read: %d", int(EnumsAndVars::Shop::item1FirstBuy));

        BR_ASSERT((std::holds_alternative<long long int>(rows[0][2])), "%s", "Item2FirstBuy contains wrong data.");
        if(std::holds_alternative<long long int>(rows[0][2]))
            EnumsAndVars::Shop::item2FirstBuy = std::get<long long int>(rows[0][2]) == 1; // True if column contains 1.
        BR_INFO("Shop::item2FirstBuy after read: %d", int(EnumsAndVars::Shop::item2FirstBuy));

        BR_ASSERT((std::holds_alternative<long long int>(rows[0][3])), "%s", "Item3FirstBuy contains wrong data.");
        if(std::holds_alternative<long long int>(rows[0][3]))
            EnumsAndVars::Shop::item3FirstBuy = std::get<long long int>(rows[0][3]) == 1; // True if column contains 1.
        BR_INFO("Shop::item3FirstBuy after read: %d", int(EnumsAndVars::Shop::item3FirstBuy));

        BR_ASSERT((std::holds_alternative<long long int>(rows[0][4])), "%s", "Item4FirstBuy contains wrong data.");
        if(std::holds_alternative<long long int>(rows[0][4]))
            EnumsAndVars::Shop::item4FirstBuy = std::get<long long int>(rows[0][4]) == 1; // True if column contains 1.
        BR_INFO("Shop::item4FirstBuy after read: %d", int(EnumsAndVars::Shop::item4FirstBuy));

        BR_ASSERT((std::holds_alternative<long long int>(rows[0][5])), "%s", "Item5FirstBuy contains wrong data.");
        if(std::holds_alternative<long long int>(rows[0][5]))
            EnumsAndVars::Shop::item5FirstBuy = std::get<long long int>(rows[0][5]) == 1; // True if column contains 1.
        BR_INFO("Shop::item5FirstBuy after read: %d", int(EnumsAndVars::Shop::item5FirstBuy));

        BR_ASSERT((std::holds_alternative<long long int>(rows[0][6])), "%s", "Item6FirstBuy contains wrong data.");
        if(std::holds_alternative<long long int>(rows[0][6]))
            EnumsAndVars::Shop::item6FirstBuy = std::get<long long int>(rows[0][6]) == 1; // True if column contains 1.
        BR_INFO("Shop::item6FirstBuy after read: %d", int(EnumsAndVars::Shop::item6FirstBuy));

        BR_ASSERT((std::holds_alternative<long long int>(rows[0][7])), "%s", "AdsOnMapsDisabled contains wrong data.");
        if(std::holds_alternative<long long int>(rows[0][7]))
            EnumsAndVars::Shop::adsOnMapsDisabled = std::get<long long int>(rows[0][7]) == 1; // True if column contains 1.
        BR_INFO("Shop::adsOnMapsDisabled after read: %d", int(EnumsAndVars::Shop::adsOnMapsDisabled));
    }

    void applyDatabaseMigrations()
    {
        std::vector<std::vector<std::variant<long long int, double, std::string, Beryll::SqliteNULL>>> rows = executeSqlSelect(selectDatabaseMigrationsAll);
        BR_ASSERT((!rows.empty() && !rows[0].empty()), "%s", "applyDatabaseMigrations() rows are empty.");

        BR_INFO("applyDatabaseMigrations() rows: %d columns: %d", rows.size(), rows[0].size());

        BR_ASSERT((std::holds_alternative<long long int>(rows[0][0])), "%s", "ID INTEGER PRIMARY KEY contains wrong data.");

        BR_ASSERT((std::holds_alternative<long long int>(rows[0][1])), "%s", "LastScriptApplied contains wrong data.");
        int lastScriptAppliedIndex = 0;
        if (std::holds_alternative<long long int>(rows[0][1]))
            lastScriptAppliedIndex = std::get<long long int>(rows[0][1]);
        BR_INFO("DatabaseMigrations lastScriptAppliedIndex: %d", lastScriptAppliedIndex);

        if(lastScriptAppliedIndex < 1)
        {
            executeSql(alterTableSettings1);
            storeSettingsInterfaceGUI(EnumsAndVars::SettingsMenu::interfaceGUI);
            storeDatabaseMigrationsLastScriptApplied(1);
            BR_INFO("%s", "Data base migration script 1 applied.");
        }
        // No else here. Only new if().
        if(lastScriptAppliedIndex < 2)
        {
            BR_ASSERT((EnumsAndVars::allPlayerTalents.size() >= 8), "%s", "allPlayerTalents has wrong size.")
            insertPlayerTalentsTalent(EnumsAndVars::allPlayerTalents[6].name, EnumsAndVars::allPlayerTalents[6].currentLevel);
            insertPlayerTalentsTalent(EnumsAndVars::allPlayerTalents[7].name, EnumsAndVars::allPlayerTalents[7].currentLevel);
            storeDatabaseMigrationsLastScriptApplied(2);
            BR_INFO("%s", "Data base migration script 2 applied.");
        }
        // No else here. Only new if().
//        if(lastScriptAppliedIndex < 3)
//        {
//
//        }
    }

    void readPlayerTalents()
    {
        std::vector<std::vector<std::variant<long long int, double, std::string, Beryll::SqliteNULL>>> rows = executeSqlSelect(selectPlayerTalentsAll);
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

    void readGameDifficulty()
    {
        std::vector<std::vector<std::variant<long long int, double, std::string, Beryll::SqliteNULL>>> rows = executeSqlSelect(selectGameDifficultyAll);
        BR_ASSERT((!rows.empty() && !rows[0].empty()), "%s", "readGameDifficulty() rows are empty.");

        BR_INFO("readGameDifficulty() rows: %d columns: %d", rows.size(), rows[0].size());

        BR_ASSERT((std::holds_alternative<long long int>(rows[0][0])), "%s", "ID INTEGER PRIMARY KEY contains wrong data.");

        BR_ASSERT((std::holds_alternative<long long int>(rows[0][1])), "%s", "Level contains wrong data.");
        if(std::holds_alternative<long long int>(rows[0][1]))
            EnumsAndVars::GameDifficulty::level = std::get<long long int>(rows[0][1]);
        BR_INFO("GameDifficulty::level after read: %d", EnumsAndVars::GameDifficulty::level);
    }

    void readAds()
    {
        std::vector<std::vector<std::variant<long long int, double, std::string, Beryll::SqliteNULL>>> rows = executeSqlSelect(selectAdsAll);
        BR_ASSERT((!rows.empty() && !rows[0].empty()), "%s", "readAds() rows are empty.");

        BR_INFO("readAds() rows: %d columns: %d", rows.size(), rows[0].size());

        BR_ASSERT((std::holds_alternative<long long int>(rows[0][0])), "%s", "ID INTEGER PRIMARY KEY contains wrong data.");

        BR_ASSERT((std::holds_alternative<long long int>(rows[0][1])), "%s", "RewardedAdTime contains wrong data.");
        if(std::holds_alternative<long long int>(rows[0][1]))
            EnumsAndVars::Ads::rewardedAdTime = std::get<long long int>(rows[0][1]);
        BR_INFO("Ads::rewardedAdTime after read: %d", EnumsAndVars::Ads::rewardedAdTime);
    }

    void readDailyReward()
    {
        std::vector<std::vector<std::variant<long long int, double, std::string, Beryll::SqliteNULL>>> rows = executeSqlSelect(selectDailyRewardAll);
        BR_ASSERT((!rows.empty() && !rows[0].empty()), "%s", "readDailyReward() rows are empty.");

        BR_INFO("readDailyReward() rows: %d columns: %d", rows.size(), rows[0].size());

        BR_ASSERT((std::holds_alternative<long long int>(rows[0][0])), "%s", "ID INTEGER PRIMARY KEY contains wrong data.");

        BR_ASSERT((std::holds_alternative<long long int>(rows[0][1])), "%s", "TookTime contains wrong data.");
        if(std::holds_alternative<long long int>(rows[0][1]))
            EnumsAndVars::DailyReward::tookTime = std::get<long long int>(rows[0][1]);
        BR_INFO("DailyReward::tookTime after read: %d", EnumsAndVars::DailyReward::tookTime);

        BR_ASSERT((std::holds_alternative<long long int>(rows[0][2])), "%s", "TookDay contains wrong data.");
        if(std::holds_alternative<long long int>(rows[0][2]))
            EnumsAndVars::DailyReward::tookDay = std::get<long long int>(rows[0][2]);
        BR_INFO("DailyReward::tookDay after read: %d", EnumsAndVars::DailyReward::tookDay);
    }

    void executeSql(const std::string& sql)
    {
        BR_INFO("executeSql() %s", sql.c_str());
        BR_ASSERT((sql.find("SELECT") == std::string::npos), "%s", "That is select query. Use executeSqlSelect().");
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

    std::vector<std::vector<std::variant<long long int, double, std::string, Beryll::SqliteNULL>>> executeSqlSelect(const std::string& sql)
    {
        BR_INFO("executeSqlSelect() %s", sql.c_str());
        BR_ASSERT((sql.find("SELECT") != std::string::npos), "%s", "That is not select query.");
        try
        {
            Beryll::DataBase::setSqlQuery(sql);
            return Beryll::DataBase::executeSelectQuery();
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

        return {};
    }
}
