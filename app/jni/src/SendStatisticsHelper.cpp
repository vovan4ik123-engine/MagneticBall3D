#include "SendStatisticsHelper.h"
#include "EnumsAndVariables.h"

namespace MagneticBall3D
{
    // Maps progress.
    bool SendStatisticsHelper::m_canSendPlayerDieEvent = true;
    bool SendStatisticsHelper::m_canSendMapStartEvent = true;
    bool SendStatisticsHelper::m_canSendMapWinEvent = true;
    bool SendStatisticsHelper::m_canSendMapLoseEvent = true;

    void SendStatisticsHelper::reset()
    {
        // Maps progress.
        m_canSendPlayerDieEvent = true;
        m_canSendMapStartEvent = true;
        m_canSendMapWinEvent = true;
        m_canSendMapLoseEvent = true;
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

    void SendStatisticsHelper::sendTalentImproved(const std::string name, const std::string currencySpent)
    {
        std::string event = "talent_" + name + "_" + currencySpent;

        Beryll::GoogleAnalytics::getInstance()->sendEventEmpty(event);
    }

    void SendStatisticsHelper::sendCustomMessage(const std::string text)
    {
        Beryll::GoogleAnalytics::getInstance()->sendEventEmpty(text);
    }
}
