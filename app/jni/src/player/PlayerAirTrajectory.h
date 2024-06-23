#pragma once

#include "EngineHeaders.h"

namespace MagneticBall3D
{
    class PlayerAirTrajectory
    {
    public:
        static PlayerAirTrajectory& getInstance()
        {
            static PlayerAirTrajectory inst;
            return inst;
        }


        PlayerAirTrajectory(const PlayerAirTrajectory& es) = delete;
        PlayerAirTrajectory& operator=(const PlayerAirTrajectory& es) = delete;
        PlayerAirTrajectory(PlayerAirTrajectory&& es) = delete;
        PlayerAirTrajectory& operator=(PlayerAirTrajectory&& es) = delete;

        void calculateAndDraw(const std::shared_ptr<Beryll::SimpleCollidingObject>& physicObj,
                              const glm::vec3& sunLightDir);

    private:
        PlayerAirTrajectory();
        ~PlayerAirTrajectory();

        // One m_trajectoryPoint should be used for all m_calculatedPositions
        // Set new origin for next position and draw it again.
        std::shared_ptr<Beryll::SimpleObject> m_trajectoryPoint;
        std::shared_ptr<Beryll::SimpleObject> m_trajectoryHitPoint;
        std::vector<glm::vec3> m_allTrajectoryPoints;
        std::vector<glm::vec3> m_hitPoint;

        glm::mat4 m_modelMatrix{1.0f};
        std::shared_ptr<Beryll::Shader> m_shader;
    };
}
