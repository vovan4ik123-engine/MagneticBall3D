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
                storeSettingsFPSLimit(EnAndVars::SettingsMenu::FPSLimit);
                storeSettingsBackgroundMusic(EnAndVars::SettingsMenu::backgroundMusic);

                // Create table CurrencyBalance and insert 1 row.
                Beryll::DataBase::setSqlQuery(createTableCurrencyBalance);
                Beryll::DataBase::executeNotSelectQuery();
                Beryll::DataBase::setSqlQuery(insertFirstRowCurrencyBalance);
                Beryll::DataBase::executeNotSelectQuery();
                storeCurrencyBalanceCrystals(EnAndVars::CurrencyBalance::crystals);

                // MapsProgress.
                Beryll::DataBase::setSqlQuery(createTableMapsProgress);
                Beryll::DataBase::executeNotSelectQuery();
                Beryll::DataBase::setSqlQuery(insertFirstRowMapsProgress);
                Beryll::DataBase::executeNotSelectQuery();
                storeMapsProgressCurrentMapIndex(EnAndVars::MapsProgress::currentMapIndex);
                storeMapsProgressLastOpenedMapIndex(EnAndVars::MapsProgress::lastOpenedMapIndex);

                // EnergySystem.
                Beryll::DataBase::setSqlQuery(createTableEnergySystem);
                Beryll::DataBase::executeNotSelectQuery();
                Beryll::DataBase::setSqlQuery(insertFirstRowEnergySystem);
                Beryll::DataBase::executeNotSelectQuery();
                storeEnergySystemCurrentAmount(EnAndVars::EnergySystem::currentAmount);
                storeEnergySystemLastSecUpdate(EnAndVars::EnergySystem::lastSecUpdate);
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

            readSettings();
            readCurrencyBalance();
            readMapsProgress();
            readEnergySystem();
        }
    }

    void readSettings()
    {
        try
        {
            Beryll::DataBase::setSqlQuery(selectSettingsAll);
            std::vector<std::vector<std::variant<long long int, double, std::string, Beryll::SqliteNULL>>> rows = Beryll::DataBase::executeSelectQuery();
            if(!rows.empty())
            {
                BR_INFO("readSettings() rows: %d columns: %d", rows.size(), rows[0].size());

                BR_ASSERT((std::holds_alternative<long long int>(rows[0][0])), "%s", "ID INTEGER PRIMARY KEY contains wrong data.");

                BR_ASSERT((std::holds_alternative<long long int>(rows[0][1])), "%s", "FPSLimit contains wrong data.");
                if(std::holds_alternative<long long int>(rows[0][1]))
                    EnAndVars::SettingsMenu::FPSLimit = std::get<long long int>(rows[0][1]);
                BR_INFO("FPSLimit after read: %d", EnAndVars::SettingsMenu::FPSLimit);

                BR_ASSERT((std::holds_alternative<long long int>(rows[0][2])), "%s", "BackgroundMusic contains wrong data.");
                if(std::holds_alternative<long long int>(rows[0][2]))
                    EnAndVars::SettingsMenu::backgroundMusic = std::get<long long int>(rows[0][2]) == 1; // True if column contains 1.
                BR_INFO("backgroundMusic after read: %d", int(EnAndVars::SettingsMenu::backgroundMusic));
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

    void readCurrencyBalance()
    {
        try
        {
            Beryll::DataBase::setSqlQuery(selectCurrencyBalanceCrystals);
            std::vector<std::vector<std::variant<long long int, double, std::string, Beryll::SqliteNULL>>> rows = Beryll::DataBase::executeSelectQuery();
            if(!rows.empty() && !rows[0].empty())
            {
                BR_INFO("readCurrencyBalance() rows: %d columns: %d", rows.size(), rows[0].size());

                BR_ASSERT((std::holds_alternative<long long int>(rows[0][0])), "%s", "ID INTEGER PRIMARY KEY contains wrong data.");

                BR_ASSERT((std::holds_alternative<long long int>(rows[0][1])), "%s", "Result of selectCurrencyBalanceCrystals contains wrong data.");
                if(std::holds_alternative<long long int>(rows[0][1]))
                    EnAndVars::CurrencyBalance::crystals = std::get<long long int>(rows[0][1]);

                BR_INFO("crystals after read: %d", EnAndVars::CurrencyBalance::crystals);
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

    void storeSettingsFPSLimit(long long int value)
    {
        try
        {
            Beryll::DataBase::setSqlQuery(updateSettingsFPSLimit);
            Beryll::DataBase::bindParameterLongLongInt(":FPS", value);
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

    void storeSettingsBackgroundMusic(bool playMusic)
    {
        long long int intValue = 0;
        if(playMusic)
            intValue = 1;

        try
        {
            Beryll::DataBase::setSqlQuery(updateSettingsBackgroundMusic);
            Beryll::DataBase::bindParameterLongLongInt(":playMusic", intValue);
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

    void storeCurrencyBalanceCrystals(long long int value)
    {
        if(value < 0)
            value = 0;

        try
        {
            Beryll::DataBase::setSqlQuery(updateCurrencyBalanceCrystals);
            Beryll::DataBase::bindParameterLongLongInt(":crystals", value);
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

    void readMapsProgress()
    {
        try
        {
            Beryll::DataBase::setSqlQuery(selectMapsProgressAll);
            std::vector<std::vector<std::variant<long long int, double, std::string, Beryll::SqliteNULL>>> rows = Beryll::DataBase::executeSelectQuery();
            if(!rows.empty())
            {
                BR_INFO("readMapsProgress() rows: %d columns: %d", rows.size(), rows[0].size());

                BR_ASSERT((std::holds_alternative<long long int>(rows[0][0])), "%s", "ID INTEGER PRIMARY KEY contains wrong data.");

                BR_ASSERT((std::holds_alternative<long long int>(rows[0][1])), "%s", "CurrentMapIndex contains wrong data.");
                if(std::holds_alternative<long long int>(rows[0][1]))
                    EnAndVars::MapsProgress::currentMapIndex = std::get<long long int>(rows[0][1]);
                BR_INFO("currentMapIndex after read: %d", EnAndVars::MapsProgress::currentMapIndex);

                BR_ASSERT((std::holds_alternative<long long int>(rows[0][2])), "%s", "LastOpenedMapIndex contains wrong data.");
                if(std::holds_alternative<long long int>(rows[0][2]))
                    EnAndVars::MapsProgress::lastOpenedMapIndex = std::get<long long int>(rows[0][2]);
                BR_INFO("lastOpenedMapIndex after read: %d", EnAndVars::MapsProgress::lastOpenedMapIndex);

                // EnAndVars::MapsProgress::maxMapIndex is hardcoded and const. Should not be changed during game.
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

    void storeMapsProgressCurrentMapIndex(long long int value)
    {
        try
        {
            Beryll::DataBase::setSqlQuery(updateMapsProgressCurrentMapIndex);
            Beryll::DataBase::bindParameterLongLongInt(":currentMapIndex", value);
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

    void storeMapsProgressLastOpenedMapIndex(long long int value)
    {
        try
        {
            Beryll::DataBase::setSqlQuery(updateMapsProgressLastOpenedMapIndex);
            Beryll::DataBase::bindParameterLongLongInt(":lastOpenedIndex", value);
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

    void readEnergySystem()
    {
        try
        {
            Beryll::DataBase::setSqlQuery(selectEnergySystemAll);
            std::vector<std::vector<std::variant<long long int, double, std::string, Beryll::SqliteNULL>>> rows = Beryll::DataBase::executeSelectQuery();
            if(!rows.empty())
            {
                BR_INFO("readEnergySystem() rows: %d columns: %d", rows.size(), rows[0].size());

                BR_ASSERT((std::holds_alternative<long long int>(rows[0][0])), "%s", "ID INTEGER PRIMARY KEY contains wrong data.");

                BR_ASSERT((std::holds_alternative<long long int>(rows[0][1])), "%s", "CurrentAmount contains wrong data.");
                if(std::holds_alternative<long long int>(rows[0][1]))
                    EnAndVars::EnergySystem::currentAmount = std::get<long long int>(rows[0][1]);
                BR_INFO("currentAmount after read: %d", EnAndVars::EnergySystem::currentAmount);

                BR_ASSERT((std::holds_alternative<long long int>(rows[0][2])), "%s", "LastSecUpdate contains wrong data.");
                if(std::holds_alternative<long long int>(rows[0][2]))
                    EnAndVars::EnergySystem::lastSecUpdate = std::get<long long int>(rows[0][2]);
                BR_INFO("lastSecUpdate after read: %d", EnAndVars::EnergySystem::lastSecUpdate);
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

    void storeEnergySystemCurrentAmount(long long int value)
    {
        try
        {
            Beryll::DataBase::setSqlQuery(updateEnergySystemCurrentAmount);
            Beryll::DataBase::bindParameterLongLongInt(":amount", value);
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

    void storeEnergySystemLastSecUpdate(long long int value)
    {
        try
        {
            Beryll::DataBase::setSqlQuery(updateEnergySystemLastSecUpdate);
            Beryll::DataBase::bindParameterLongLongInt(":lastSecUpdate", value);
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
