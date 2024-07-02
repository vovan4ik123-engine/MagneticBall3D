#pragma once

#include "BaseMap.h"

namespace MagneticBall3D
{
    class Map2 : public BaseMap
    {
    public:
        Map2(std::shared_ptr<PlayStateGUILayer> gui);
        ~Map2() override;

        void draw() override;

        void spawnCommonGarbage() override;

    private:
        void loadEnv();
        void loadGarbage();
        void loadEnemies();
        void loadBoss();

        std::vector<std::shared_ptr<Beryll::BaseSimpleObject>> m_objWithNormalMap;

        void spawnEnemies() override;
        void startBossPhase() override;
        void handlePossPhase() override;
    };
}
