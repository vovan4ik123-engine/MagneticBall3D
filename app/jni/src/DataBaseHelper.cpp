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
            std::vector<std::vector<std::variant<int, float, std::string, Beryll::SqliteNULL>>> rows = Beryll::DataBase::executeSelectQuery();

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

        BR_INFO("%s", "Settings table empty.");
        return true;
    }

    void prepareDatabase()
    {
        // Table Settings is main table.
        // If it does not exist app is launched first time on device.
        // Create all tables we need for app.
        if(getIsSettingsTableEmpty())
        {
            BR_INFO("%s", "Create all tables + first rows at app first launch.");

            try
            {
                // Create table Settings and insert 1 row.
                Beryll::DataBase::openDataBase(dataBaseName);
                Beryll::DataBase::setSqlQuery(createTableSettings);
                Beryll::DataBase::executeNotSelectQuery();
                Beryll::DataBase::setSqlQuery(insertFirstRowSettings);
                Beryll::DataBase::executeNotSelectQuery();
                storeSettingsFPSLimit(EnAndVars::SettingsMenu::FPSLimit);
                storeSettingsBackgroundMusic(EnAndVars::SettingsMenu::backgroundMusic);

                // Create table CurrencyBalance and insert 1 row.
                Beryll::DataBase::openDataBase(dataBaseName);
                Beryll::DataBase::setSqlQuery(createTableCurrencyBalance);
                Beryll::DataBase::executeNotSelectQuery();
                Beryll::DataBase::setSqlQuery(insertFirstRowCurrencyBalance);
                Beryll::DataBase::executeNotSelectQuery();
                storeCurrencyBalanceCrystals(EnAndVars::CurrencyBalance::crystals);
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
            BR_INFO("%s", "Read all tables at app launch.");

            readSettings();
            readCurrencyBalance();
        }
    }

    void readSettings()
    {
        try
        {
            Beryll::DataBase::openDataBase(dataBaseName);
            Beryll::DataBase::setSqlQuery(selectSettingsAll);
            std::vector<std::vector<std::variant<int, float, std::string, Beryll::SqliteNULL>>> rows = Beryll::DataBase::executeSelectQuery();
            if(!rows.empty())
            {
                BR_INFO("readSettings() rows: %d columns: %d", rows.size(), rows[0].size());

                BR_ASSERT((std::holds_alternative<int>(rows[0][0])), "%s", "ID INTEGER PRIMARY KEY contains wrong data.");

                BR_ASSERT((std::holds_alternative<int>(rows[0][1])), "%s", "FPSLimit contains wrong data.");
                if(std::holds_alternative<int>(rows[0][1]))
                    EnAndVars::SettingsMenu::FPSLimit = std::get<int>(rows[0][1]);
                BR_INFO("FPSLimit after read: %d", EnAndVars::SettingsMenu::FPSLimit);

                BR_ASSERT((std::holds_alternative<int>(rows[0][2])), "%s", "BackgroundMusic contains wrong data.");
                if(std::holds_alternative<int>(rows[0][2]))
                    EnAndVars::SettingsMenu::backgroundMusic = std::get<int>(rows[0][2]) == 1; // True if column contains 1.
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
            Beryll::DataBase::openDataBase(dataBaseName);
            Beryll::DataBase::setSqlQuery(selectCurrencyBalanceCrystals);
            std::vector<std::vector<std::variant<int, float, std::string, Beryll::SqliteNULL>>> rows = Beryll::DataBase::executeSelectQuery();
            if(!rows.empty() && !rows[0].empty())
            {
                BR_INFO("readCurrencyBalance() rows.size() %d", rows.size());

                BR_ASSERT((std::holds_alternative<int>(rows[0][0])), "%s", "Result of selectCurrencyBalanceCrystals contains wrong data.");

                if(std::holds_alternative<int>(rows[0][0]))
                    EnAndVars::CurrencyBalance::crystals = std::get<int>(rows[0][0]);

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

    void storeSettingsFPSLimit(int value)
    {
        try
        {
            Beryll::DataBase::openDataBase(dataBaseName);
            Beryll::DataBase::setSqlQuery(updateSettingsFPSLimit);
            Beryll::DataBase::bindParameterInt(":FPS", value);
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
        int intValue = 0;
        if(playMusic)
            intValue = 1;

        try
        {
            Beryll::DataBase::openDataBase(dataBaseName);
            Beryll::DataBase::setSqlQuery(updateSettingsBackgroundMusic);
            Beryll::DataBase::bindParameterInt(":playMusic", intValue);
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

    void storeCurrencyBalanceCrystals(int value)
    {
        if(value < 0)
            return;

        try
        {
            Beryll::DataBase::openDataBase(dataBaseName);
            Beryll::DataBase::setSqlQuery(updateCurrencyBalanceCrystals);
            Beryll::DataBase::bindParameterInt(":crystals", value);
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
