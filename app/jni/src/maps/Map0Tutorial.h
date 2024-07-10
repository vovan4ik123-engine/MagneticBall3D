#pragma once

#include "BaseMap.h"

namespace MagneticBall3D
{
    class Map0Tutorial : public BaseMap
    {
    public:
        Map0Tutorial(std::shared_ptr<PlayStateGUILayer> gui);
        ~Map0Tutorial() override;

        void updateBeforePhysics() override;
        void updateAfterPhysics() override;
        void draw() override;

        void spawnEnemies() override {};
        void startBossPhase() override {};
        void handlePossPhase() override {};

    private:
        void loadEnv();
        void loadGarbage();
        void loadEnemies();
    };
}
