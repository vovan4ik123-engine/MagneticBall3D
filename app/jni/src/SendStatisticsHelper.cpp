#include "SendStatisticsHelper.h"
#include "EnumsAndVariables.h"

namespace MagneticBall3D
{
    bool SendStatisticsHelper::m_isCanSendPlayerDieEvent = true;
    bool SendStatisticsHelper::m_isCanSendMapStartEvent = true;
    bool SendStatisticsHelper::m_isCanSendMapWinEvent = true;
    bool SendStatisticsHelper::m_isCanSendMapLoseEvent = true;

    void SendStatisticsHelper::reset()
    {
        m_isCanSendPlayerDieEvent = true;
        m_isCanSendMapStartEvent = true;
        m_isCanSendMapWinEvent = true;
        m_isCanSendMapLoseEvent = true;
    }

    void SendStatisticsHelper::sendPlayerDie()
    {
        if(m_isCanSendPlayerDieEvent)
        {
            m_isCanSendPlayerDieEvent = false;

            std::string eventType = "map_" + std::to_string(EnAndVars::mapCurrentNumber) + "_player_die";

            Beryll::GoogleAnalytics::getInstance()->sendEventFloatParam(eventType, "die_time_sec", EnAndVars::mapPlayTimeSec);
        }
    }

    void SendStatisticsHelper::canSendPlayerDie()
    {
        m_isCanSendPlayerDieEvent = true;
    }

    void SendStatisticsHelper::sendMapStart()
    {
        if(m_isCanSendMapStartEvent)
        {
            m_isCanSendMapStartEvent = false;

            std::string eventType = "map_" + std::to_string(EnAndVars::mapCurrentNumber) + "_start";

            Beryll::GoogleAnalytics::getInstance()->sendEventEmpty(eventType);
        }
    }

    void SendStatisticsHelper::sendMapWin()
    {
        if(m_isCanSendMapWinEvent)
        {
            m_isCanSendMapWinEvent = false;

            std::string eventType = "map_" + std::to_string(EnAndVars::mapCurrentNumber) + "_win";

            Beryll::GoogleAnalytics::getInstance()->sendEventFloatParam(eventType, "win_time_sec", EnAndVars::mapPlayTimeSec);
        }
    }

    void SendStatisticsHelper::canSendMapWin()
    {
        m_isCanSendMapWinEvent = true;
    }

    void SendStatisticsHelper::sendMapLose()
    {
        if(m_isCanSendMapLoseEvent)
        {
            m_isCanSendMapLoseEvent = false;

            std::string eventType = "map_" + std::to_string(EnAndVars::mapCurrentNumber) + "_lose";

            Beryll::GoogleAnalytics::getInstance()->sendEventFloatParam(eventType, "lose_time_sec", EnAndVars::mapPlayTimeSec);
        }
    }

    void SendStatisticsHelper::canSendMapLose()
    {
        m_isCanSendMapLoseEvent = true;
    }
}
