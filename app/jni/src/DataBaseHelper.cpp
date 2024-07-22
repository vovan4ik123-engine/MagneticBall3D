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
                Beryll::DataBase::setSqlQuery(createTableSettings);
                Beryll::DataBase::executeNotSelectQuery();
                Beryll::DataBase::setSqlQuery(insertFirstRowSettings);
                Beryll::DataBase::executeNotSelectQuery();
                storeSettingsFPSLimit(EnumsAndVars::SettingsMenu::FPSLimit);
                storeSettingsBackgroundMusic(EnumsAndVars::SettingsMenu::backgroundMusic);
                storeSettingsMeteorParticles(EnumsAndVars::SettingsMenu::meteorParticles);

                // Create table CurrencyBalance and insert 1 row.
                Beryll::DataBase::setSqlQuery(createTableCurrencyBalance);
                Beryll::DataBase::executeNotSelectQuery();
                Beryll::DataBase::setSqlQuery(insertFirstRowCurrencyBalance);
                Beryll::DataBase::executeNotSelectQuery();
                storeCurrencyBalanceCrystals(EnumsAndVars::CurrencyBalance::crystals);

                // MapsProgress.
                Beryll::DataBase::setSqlQuery(createTableMapsProgress);
                Beryll::DataBase::executeNotSelectQuery();
                Beryll::DataBase::setSqlQuery(insertFirstRowMapsProgress);
                Beryll::DataBase::executeNotSelectQuery();
                storeMapsProgressCurrentMapIndex(EnumsAndVars::MapsProgress::currentMapIndex);
                storeMapsProgressLastOpenedMapIndex(EnumsAndVars::MapsProgress::lastOpenedMapIndex);

                // EnergySystem.
                Beryll::DataBase::setSqlQuery(createTableEnergySystem);
                Beryll::DataBase::executeNotSelectQuery();
                Beryll::DataBase::setSqlQuery(insertFirstRowEnergySystem);
                Beryll::DataBase::executeNotSelectQuery();
                storeEnergySystemCurrentAmount(EnumsAndVars::EnergySystem::currentAmount);
                storeEnergySystemLastSecUpdated(EnumsAndVars::EnergySystem::lastSecUpdated);
                storeEnergySystemLastSecRestored(EnumsAndVars::EnergySystem::lastSecOneEnergyRestored);

                // Shop.
                Beryll::DataBase::setSqlQuery(createTableShop);
                Beryll::DataBase::executeNotSelectQuery();
                Beryll::DataBase::setSqlQuery(insertFirstRowShop);
                Beryll::DataBase::executeNotSelectQuery();
                storeShopItem1FirstBuy(EnumsAndVars::Shop::item1FirstBuy ? 1 : 0);
                storeShopItem2FirstBuy(EnumsAndVars::Shop::item2FirstBuy ? 1 : 0);
                storeShopItem3FirstBuy(EnumsAndVars::Shop::item3FirstBuy ? 1 : 0);
                storeShopItem4FirstBuy(EnumsAndVars::Shop::item4FirstBuy ? 1 : 0);
                storeShopItem5FirstBuy(EnumsAndVars::Shop::item5FirstBuy ? 1 : 0);
                storeShopItem6FirstBuy(EnumsAndVars::Shop::item6FirstBuy ? 1 : 0);

                // DatabaseMigrations.
                Beryll::DataBase::setSqlQuery(createTableDatabaseMigrations);
                Beryll::DataBase::executeNotSelectQuery();
                Beryll::DataBase::setSqlQuery(insertFirstRowDatabaseMigrations);
                Beryll::DataBase::executeNotSelectQuery();
                storeDatabaseMigrationsLastScriptApplied(0);

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
//
//        if(lastScriptAppliedIndex < 2)
//        {
//            applyDatabaseMigrationsScript2();
//            storeDatabaseMigrationsLastScriptApplied(2);
//        }
    }

    void executeSqlWithLongLongParam(const std::string& sql, const char* paramName, long long int value)
    {
        try
        {
            Beryll::DataBase::setSqlQuery(sql);
            Beryll::DataBase::bindParameterLongLongInt(paramName, value);
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
