#pragma once

#include "EngineHeaders.h"

namespace MagneticBall3D
{
    class SendStatisticsHelper
    {
    public:
        static void reset();

        static void sendPlayerDie();
        static void canSendPlayerDie();

        static void sendMapStart();

        static void sendMapWin();
        static void canSendMapWin();

        static void sendMapLose();
        static void canSendMapLose();

    private:
        static bool m_isCanSendPlayerDieEvent;
        static bool m_isCanSendMapStartEvent;
        static bool m_isCanSendMapWinEvent;
        static bool m_isCanSendMapLoseEvent;
    };
}
