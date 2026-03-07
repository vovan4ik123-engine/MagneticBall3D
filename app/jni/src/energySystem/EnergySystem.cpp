#include "EnergySystem.h"
#include "EnumsAndVariables.h"
#include "DataBaseHelper.h"

namespace MagneticBall3D
{
    EnergySystem::EnergySystem()
    {
        BR_INFO("%s", "EnergySystem::EnergySystem()");

    }

    EnergySystem::~EnergySystem()
    {

    }

    void EnergySystem::update()
    {
        m_currentSec = Beryll::TimeStep::getSecSinceEpoch();

        if(EnumsAndVars::EnergySystem::lastSecUpdated < m_currentSec)
        {
            //BR_INFO("if(EnumsAndVars::EnergySystem::lastSecUpdated < currentSec) %d", m_currentSec);
            EnumsAndVars::EnergySystem::lastSecUpdated = m_currentSec;
            DataBaseHelper::storeEnergySystemLastSecUpdated(EnumsAndVars::EnergySystem::lastSecUpdated);

            if(EnumsAndVars::EnergySystem::currentAmount >= EnumsAndVars::EnergySystem::maxLimitToRestore)
            {
                // Reset timer if we have max or more(more than max can be if user will buy energy for real money. There is no limit)
                // If currentAmount will drop below maxLimitToRestore, energy restore process will  start from currentSec time.
                EnumsAndVars::EnergySystem::lastSecOneEnergyRestored = m_currentSec;
                DataBaseHelper::storeEnergySystemLastSecRestored(EnumsAndVars::EnergySystem::lastSecOneEnergyRestored);
            }
            else if(EnumsAndVars::EnergySystem::lastSecOneEnergyRestored + EnumsAndVars::EnergySystem::secToRestoreOneEnergy <= m_currentSec)
            {
                //BR_INFO("%s", "Need and can restore one or more energy.");
                // Time when one energy can be restored. Or more if app launched after long time inactive.
                const int amountCanBeRestored = (m_currentSec - EnumsAndVars::EnergySystem::lastSecOneEnergyRestored) / EnumsAndVars::EnergySystem::secToRestoreOneEnergy;
                const int newAmount = EnumsAndVars::EnergySystem::currentAmount + amountCanBeRestored;
                if(newAmount >= EnumsAndVars::EnergySystem::maxLimitToRestore)
                {
                    EnumsAndVars::EnergySystem::currentAmount = EnumsAndVars::EnergySystem::maxLimitToRestore;
                    EnumsAndVars::EnergySystem::lastSecOneEnergyRestored = m_currentSec;
                }
                else
                {
                    int amountRestored = newAmount - EnumsAndVars::EnergySystem::currentAmount;
                    EnumsAndVars::EnergySystem::currentAmount = newAmount;
                    EnumsAndVars::EnergySystem::lastSecOneEnergyRestored += EnumsAndVars::EnergySystem::secToRestoreOneEnergy * amountRestored;
                }

                //BR_INFO("amountCanBeRestored %d after restore %d", amountCanBeRestored, EnumsAndVars::EnergySystem::currentAmount);
                DataBaseHelper::storeEnergySystemCurrentAmount(EnumsAndVars::EnergySystem::currentAmount);
                DataBaseHelper::storeEnergySystemLastSecRestored(EnumsAndVars::EnergySystem::lastSecOneEnergyRestored);
            }
        }
    }

    void EnergySystem::draw()
    {

        // Text restore timer.
        m_restoreTimerText = "";
        if(EnumsAndVars::EnergySystem::currentAmount < EnumsAndVars::EnergySystem::maxLimitToRestore &&
           EnumsAndVars::EnergySystem::lastSecOneEnergyRestored + EnumsAndVars::EnergySystem::secToRestoreOneEnergy >= m_currentSec)
        {
            // Set timer about restore one energy.
            uint64_t secLeft = (EnumsAndVars::EnergySystem::lastSecOneEnergyRestored + EnumsAndVars::EnergySystem::secToRestoreOneEnergy) - m_currentSec;
            if(secLeft > EnumsAndVars::EnergySystem::secToRestoreOneEnergy)
                secLeft = EnumsAndVars::EnergySystem::secToRestoreOneEnergy;

            int min = secLeft / 60;
            int sec = secLeft % 60;

            if(min < 10)
                m_restoreTimerText += "0";

            m_restoreTimerText += std::to_string(min);
            m_restoreTimerText += ":";

            if(sec < 10)
                m_restoreTimerText += "0";

            m_restoreTimerText += std::to_string(sec);
        }

    }

    bool EnergySystem::isEnoughForPlay()
    {
        return EnumsAndVars::EnergySystem::currentAmount >= EnumsAndVars::EnergySystem::playCost;
    }

    void EnergySystem::handlePlay()
    {
        EnumsAndVars::EnergySystem::currentAmount -= EnumsAndVars::EnergySystem::playCost;
        DataBaseHelper::storeEnergySystemCurrentAmount(EnumsAndVars::EnergySystem::currentAmount);
    }
}
