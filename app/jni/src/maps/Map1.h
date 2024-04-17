#pragma once

#include "BaseMap.h"

namespace MagneticBall3D
{
    class Map1 : public BaseMap
    {
    public:
        Map1(std::shared_ptr<PlayStateGUILayer> gui);
        ~Map1() override;

        void toMakeClassAbstract() override {};

    private:
        void loadPlayerAndEnv();
        void loadGarbage();
        void loadEnemies();
    };
}
