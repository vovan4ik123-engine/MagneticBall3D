#pragma once

#include "BaseMap.h"

namespace MagneticBall3D
{
    class Map3 : public BaseMap
    {
    public:
        Map3(std::shared_ptr<PlayStateGUILayer> gui);
        ~Map3() override;

        void draw() override;

    private:
        void loadEnv();
        void loadGarbage();
        void loadEnemies();
        void loadBoss();

        void spawnEnemies() override;
        void startBossPhase() override;
        void handlePossPhase() override;

        // Enemies waves.
    };
}
