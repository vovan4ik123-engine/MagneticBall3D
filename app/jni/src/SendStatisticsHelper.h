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

        // Map0Tutorial.
        static void sendMap0_100mPassed();
        static void sendMap0_200mPassed();
        static void sendMap0_400mPassed();
        static void sendMap0_600mPassed();
        static void sendMap0_800mPassed();
        static void sendMap0_onBuilding();
        static void sendMap0_1200mPassed();

        // Talents.
        static void sendTalentImproved(const std::string name, const std::string currencySpent); // currencySpent = ad or crystals.

        // Custom message.
        static void sendCustomMessage(const std::string text);

    private:
        static bool m_canSendPlayerDieEvent;
        static bool m_canSendMapStartEvent;
        static bool m_canSendMapWinEvent;
        static bool m_canSendMapLoseEvent;

        // Map0Tutorial.
        static bool m_canSendMap0_100mPassed;
        static bool m_canSendMap0_200mPassed;
        static bool m_canSendMap0_400mPassed;
        static bool m_canSendMap0_600mPassed;
        static bool m_canSendMap0_800mPassed;
        static bool m_canSendMap0_onBuilding;
        static bool m_canSendMap0_1200mPassed;
    };
}
