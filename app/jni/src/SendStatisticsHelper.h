#pragma once

#include "EngineHeaders.h"

namespace MagneticBall3D
{
    class SendStatisticsHelper
    {
    public:
        static void reset();

        // Maps progress.
        static void sendPlayerDie();
        static void canSendPlayerDie(); // Can be reset during map play.
        static void sendMapStart();
        static void sendMapWin();
        static void sendMapLose();

        // Talents.
        static void sendTalentImproved(const std::string name, const std::string currencySpent); // currencySpent = "ad" or "crystal".

        // Custom message.
        static void sendCustomMessage(const std::string text);

    private:
        static bool m_canSendPlayerDieEvent;
        static bool m_canSendMapStartEvent;
        static bool m_canSendMapWinEvent;
        static bool m_canSendMapLoseEvent;
    };
}
