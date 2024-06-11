#include "EnergySystem.h"
#include "EnumsAndVariables.h"
#include "DataBaseHelper.h"

namespace MagneticBall3D
{
    EnergySystem::EnergySystem()
    {
        BR_INFO("%s", "EnergySystem::EnergySystem()");

        m_buttonEnergy = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/start/Energy.jpg", "", 0.4f, 0.0f, 0.2f, 0.05f);
        m_textAmount = std::make_shared<Beryll::Text>("00/00", EnAndVars::FontsPath::roboto, 0.025f, 0.46f, 0.008, 0.14f, 0.03f, false, true);
        m_textRestoreTimer = std::make_shared<Beryll::Text>("00:00", EnAndVars::FontsPath::roboto, 0.015f, 0.48f, 0.033, 0.12f, 0.02f, false, true);
    }

    EnergySystem::~EnergySystem()
    {

    }

    void EnergySystem::update()
    {
        m_currentSec = Beryll::TimeStep::getSecSinceEpoch();

        if(EnAndVars::EnergySystem::lastSecUpdated < m_currentSec)
        {
            BR_INFO("if(EnAndVars::EnergySystem::lastSecUpdated < currentSec) %d", m_currentSec);
            EnAndVars::EnergySystem::lastSecUpdated = m_currentSec;
            DataBaseHelper::storeEnergySystemLastSecUpdated(EnAndVars::EnergySystem::lastSecUpdated);

            if(EnAndVars::EnergySystem::currentAmount >= EnAndVars::EnergySystem::maxLimitToRestore)
            {
                // Reset timer if we have max or more(more than max can be if user will buy energy for real money. There is no limit)
                // If currentAmount will drop below maxLimitToRestore, energy restore process will  start from currentSec time.
                EnAndVars::EnergySystem::lastSecOneEnergyRestored = m_currentSec;
                DataBaseHelper::storeEnergySystemLastSecRestored(EnAndVars::EnergySystem::lastSecOneEnergyRestored);
            }
            else if(EnAndVars::EnergySystem::lastSecOneEnergyRestored + EnAndVars::EnergySystem::secToRestoreOneEnergy <= m_currentSec)
            {
                BR_INFO("%s", "Need and can restore one or more energy.");
                // Time when one energy can be restored. Or more if app launched after long time inactive.
                const int amountCanBeRestored = (m_currentSec - EnAndVars::EnergySystem::lastSecOneEnergyRestored) / EnAndVars::EnergySystem::secToRestoreOneEnergy;
                const int newAmount = EnAndVars::EnergySystem::currentAmount + amountCanBeRestored;
                if(newAmount >= EnAndVars::EnergySystem::maxLimitToRestore)
                {
                    EnAndVars::EnergySystem::currentAmount = EnAndVars::EnergySystem::maxLimitToRestore;
                    EnAndVars::EnergySystem::lastSecOneEnergyRestored = m_currentSec;
                }
                else
                {
                    int amountRestored = newAmount - EnAndVars::EnergySystem::currentAmount;
                    EnAndVars::EnergySystem::currentAmount = newAmount;
                    EnAndVars::EnergySystem::lastSecOneEnergyRestored += EnAndVars::EnergySystem::secToRestoreOneEnergy * amountRestored;
                }

                BR_INFO("amountCanBeRestored %d after restore %d", amountCanBeRestored, EnAndVars::EnergySystem::currentAmount);
                DataBaseHelper::storeEnergySystemCurrentAmount(EnAndVars::EnergySystem::currentAmount);
                DataBaseHelper::storeEnergySystemLastSecRestored(EnAndVars::EnergySystem::lastSecOneEnergyRestored);
            }
        }
    }

    void EnergySystem::draw()
    {
        m_buttonEnergy->draw();

        m_textAmount->text = std::to_string(EnAndVars::EnergySystem::currentAmount);
        m_textAmount->text += "/";
        m_textAmount->text += std::to_string(EnAndVars::EnergySystem::maxLimitToRestore);
        m_textAmount->draw();

        if(EnAndVars::EnergySystem::currentAmount < EnAndVars::EnergySystem::maxLimitToRestore)
        {
            // Draw timer about restore one energy.
            const uint64_t secLeft = (EnAndVars::EnergySystem::lastSecOneEnergyRestored + EnAndVars::EnergySystem::secToRestoreOneEnergy) - m_currentSec;
            if(secLeft > EnAndVars::EnergySystem::secToRestoreOneEnergy)
            {
                BR_ASSERT(false, "secLeft > EnAndVars::EnergySystem::secToRestoreOneEnergy %d %d", secLeft, EnAndVars::EnergySystem::secToRestoreOneEnergy);
                return;
            }

            int min = int(secLeft) / 60;
            int sec = int(secLeft) % 60;

            std::stringstream stream;
            if(min < 10)
                stream << "0";

            stream << min << ":";

            if(sec < 10)
                stream << "0";

            stream << sec;

            m_textRestoreTimer->text = stream.str();
            m_textRestoreTimer->draw();
        }
    }

    bool EnergySystem::isEnoughForPlay()
    {
        return EnAndVars::EnergySystem::currentAmount >= EnAndVars::EnergySystem::playCost;
    }

    void EnergySystem::handlePlay()
    {
        EnAndVars::EnergySystem::currentAmount -= EnAndVars::EnergySystem::playCost;
        DataBaseHelper::storeEnergySystemCurrentAmount(EnAndVars::EnergySystem::currentAmount);
    }
}
