#include "SendStatisticsHelper.h"
#include "EnumsAndVariables.h"

namespace MagneticBall3D
{
    // Maps progress.
    bool SendStatisticsHelper::m_canSendPlayerDieEvent = true;
    bool SendStatisticsHelper::m_canSendMapStartEvent = true;
    bool SendStatisticsHelper::m_canSendMapWinEvent = true;
    bool SendStatisticsHelper::m_canSendMapLoseEvent = true;

    // Map0Tutorial.
    bool SendStatisticsHelper::m_canSendMap0_100mPassed = true;
    bool SendStatisticsHelper::m_canSendMap0_200mPassed = true;
    bool SendStatisticsHelper::m_canSendMap0_400mPassed = true;
    bool SendStatisticsHelper::m_canSendMap0_600mPassed = true;
    bool SendStatisticsHelper::m_canSendMap0_800mPassed = true;
    bool SendStatisticsHelper::m_canSendMap0_onBuilding = true;
    bool SendStatisticsHelper::m_canSendMap0_1200mPassed = true;

    void SendStatisticsHelper::reset()
    {
        // Maps progress.
        m_canSendPlayerDieEvent = true;
        m_canSendMapStartEvent = true;
        m_canSendMapWinEvent = true;
        m_canSendMapLoseEvent = true;

        // Map0Tutorial.
        m_canSendMap0_100mPassed = true;
        m_canSendMap0_200mPassed = true;
        m_canSendMap0_400mPassed = true;
        m_canSendMap0_600mPassed = true;
        m_canSendMap0_800mPassed = true;
        m_canSendMap0_onBuilding = true;
        m_canSendMap0_1200mPassed = true;
    }

    void SendStatisticsHelper::sendPlayerDie()
    {
        if(m_canSendPlayerDieEvent)
        {
            m_canSendPlayerDieEvent = false;

            std::string eventType = "map_" + std::to_string(EnumsAndVars::MapsProgress::currentMapIndex) + "_player_die";

            Beryll::GoogleAnalytics::getInstance()->sendEventIntParam(eventType, "die_time_sec", int(EnumsAndVars::mapPlayTimeSec));
        }
    }

    void SendStatisticsHelper::canSendPlayerDie()
    {
        m_canSendPlayerDieEvent = true;
    }

    void SendStatisticsHelper::sendMapStart()
    {
        if(m_canSendMapStartEvent)
        {
            m_canSendMapStartEvent = false;

            std::string eventType = "map_" + std::to_string(EnumsAndVars::MapsProgress::currentMapIndex) + "_start";

            Beryll::GoogleAnalytics::getInstance()->sendEventEmpty(eventType);
        }
    }

    void SendStatisticsHelper::sendMapWin()
    {
        if(m_canSendMapWinEvent)
        {
            m_canSendMapWinEvent = false;

            std::string eventType = "map_" + std::to_string(EnumsAndVars::MapsProgress::currentMapIndex) + "_win";

            Beryll::GoogleAnalytics::getInstance()->sendEventIntParam(eventType, "win_time_sec", int(EnumsAndVars::mapPlayTimeSec));
        }
    }

    void SendStatisticsHelper::sendMapLose()
    {
        if(m_canSendMapLoseEvent)
        {
            m_canSendMapLoseEvent = false;

            std::string eventType = "map_" + std::to_string(EnumsAndVars::MapsProgress::currentMapIndex) + "_lose";

            Beryll::GoogleAnalytics::getInstance()->sendEventIntParam(eventType, "lose_time_sec", int(EnumsAndVars::mapPlayTimeSec));
        }
    }

    void SendStatisticsHelper::sendMap0_100mPassed()
    {
        if(m_canSendMap0_100mPassed)
        {
            m_canSendMap0_100mPassed = false;

            Beryll::GoogleAnalytics::getInstance()->sendEventEmpty("map_0_100m_passed");
        }
    }

    void SendStatisticsHelper::sendMap0_200mPassed()
    {
        if(m_canSendMap0_200mPassed)
        {
            m_canSendMap0_200mPassed = false;

            Beryll::GoogleAnalytics::getInstance()->sendEventEmpty("map_0_200m_passed");
        }
    }

    void SendStatisticsHelper::sendMap0_400mPassed()
    {
        if(m_canSendMap0_400mPassed)
        {
            m_canSendMap0_400mPassed = false;

            Beryll::GoogleAnalytics::getInstance()->sendEventEmpty("map_0_400m_passed");
        }
    }

    void SendStatisticsHelper::sendMap0_600mPassed()
    {
        if(m_canSendMap0_600mPassed)
        {
            m_canSendMap0_600mPassed = false;

            Beryll::GoogleAnalytics::getInstance()->sendEventEmpty("map_0_600m_passed");
        }
    }

    void SendStatisticsHelper::sendMap0_800mPassed()
    {
        if(m_canSendMap0_800mPassed)
        {
            m_canSendMap0_800mPassed = false;

            Beryll::GoogleAnalytics::getInstance()->sendEventEmpty("map_0_800m_passed");
        }
    }

    void SendStatisticsHelper::sendMap0_onBuilding()
    {
        if(m_canSendMap0_onBuilding)
        {
            m_canSendMap0_onBuilding = false;

            Beryll::GoogleAnalytics::getInstance()->sendEventEmpty("map_0_on_building");
        }
    }

    void SendStatisticsHelper::sendMap0_1200mPassed()
    {
        if(m_canSendMap0_1200mPassed)
        {
            m_canSendMap0_1200mPassed = false;

            Beryll::GoogleAnalytics::getInstance()->sendEventEmpty("map_0_1200m_passed");
        }
    }

    void SendStatisticsHelper::sendTalentImproved(std::string name, const int level, std::string currencySpent)
    {
        std::string event = "talent_" + name + "_level_" + std::to_string(level) + "_improved_by_" + currencySpent;

        Beryll::GoogleAnalytics::getInstance()->sendEventEmpty(event);
    }
}
