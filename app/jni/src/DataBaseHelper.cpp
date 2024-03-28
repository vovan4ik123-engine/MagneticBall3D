#include "DataBaseHelper.h"
#include "EnumsAndVariables.h"

namespace DataBaseHelper
{
    void readSettings()
    {
        try
        {
            Beryll::DataBase::openDataBase(dataBaseName);
            Beryll::DataBase::setSqlQuery(selectSettings);
            std::vector<std::vector<std::variant<int, float, std::string, Beryll::SqliteNULL>>> rows = Beryll::DataBase::executeSelectQuery();
            if(!rows.empty() && rows[0].size() == 1)
            {
                BR_INFO("readSettings() rows.size() %d", rows.size());

                BR_ASSERT((std::holds_alternative<int>(rows[0][0])), "%s", "Result of Select FPSLimit contains wrong data.");

                if(std::holds_alternative<int>(rows[0][0]))
                    EnumsAndVariables::SettingsMenu::FPSLimit = std::get<int>(rows[0][0]);
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

    bool getIsSettingsTableEmpty()
    {
        try
        {
            Beryll::DataBase::openDataBase(dataBaseName);
            Beryll::DataBase::setSqlQuery(selectSettings);
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

    void storeFPSLimit(int value)
    {
        if(getIsSettingsTableEmpty())
        {
            try
            {
                Beryll::DataBase::openDataBase(dataBaseName);
                Beryll::DataBase::setSqlQuery(createTableSettings);
                Beryll::DataBase::executeNotSelectQuery();
                Beryll::DataBase::setSqlQuery(insertFirstRowToSettings);
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

        // Here first row must exist. Update it.
        try
        {
            Beryll::DataBase::openDataBase(dataBaseName);
            Beryll::DataBase::setSqlQuery(updateFPSLimit);
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
}
