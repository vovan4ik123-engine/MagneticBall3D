#include "PlayerAirTrajectory.h"

namespace MagneticBall3D
{
    PlayerAirTrajectory::PlayerAirTrajectory()
    {
        m_trajectoryPoint = std::make_shared<Beryll::SimpleObject>("models3D/player/PlayerTrajectoryPoint.fbx", Beryll::SceneObjectGroups::NONE);
        m_trajectoryHitPoint = std::make_shared<Beryll::SimpleObject>("models3D/player/PlayerTrajectoryHitPoint.fbx", Beryll::SceneObjectGroups::NONE);

        m_shader = Beryll::Renderer::createShader("shaders/GLES/PlayerAirTrajectory.vert",
                                                  "shaders/GLES/PlayerAirTrajectory.frag");
        m_shader->bind();
        m_shader->activateDiffuseTextureMat1();

        m_allTrajectoryPoints.reserve(30);
        m_hitPoint.reserve(1);
    }

    PlayerAirTrajectory::~PlayerAirTrajectory()
    {

    }

    void PlayerAirTrajectory::calculateAndDraw(const std::shared_ptr<Beryll::SimpleCollidingObject>& physicObj,
                                               const glm::vec3& sunLightDir)
    {
        const glm::vec3 linearVelocity = physicObj->getLinearVelocity();
        const glm::vec3 direction = glm::normalize(linearVelocity);
        const float speed = glm::length(linearVelocity);
        const float angleWorldUpAndDir = BeryllUtils::Common::getAngleInRadians(BeryllConstants::worldUp, direction);
        const float throwAngleRadians = glm::half_pi<float>() - angleWorldUpAndDir;
        const glm::vec3 gravity = physicObj->getGravity();
        const glm::vec3 startPosition = physicObj->getOrigin();
        glm::vec3 throwDirXZPlane{0.0f};
        glm::vec3 startPositionXZPlane = startPosition;
        startPositionXZPlane.y = 0.0f;

        float XZDistance = 0.0f;
        const float startHeight = startPosition.y + 2.0f;
        float calculatedY = 0.0f;
        glm::vec3 previousPoint{0.0f};
        glm::vec3 currentPoint{0.0f};
        bool castRayBetweenPoints = false;

        m_allTrajectoryPoints.clear();
        m_hitPoint.clear();

        // Calculate some points on fly trajectory.
        for(int i = 2; i <= 25; ++i)
        {
            throwDirXZPlane = direction * float(i * i) * 1.2f; // Point every float(i * i)... meters on trajectory !!! Not on ground !!!
            if(i == 3)
                throwDirXZPlane *= 1.035f;

            throwDirXZPlane.y = 0.0f; // Do point projection on ground.
            XZDistance = glm::length(throwDirXZPlane);
            // https://www.omnicalculator.com/physics/trajectory-projectile-motion
            calculatedY = startHeight +
                          XZDistance * glm::tan(throwAngleRadians) - glm::length(gravity) * glm::pow(XZDistance, 2) /
                                                                     (2 * glm::pow(speed, 2) * glm::pow(glm::cos(throwAngleRadians), 2));

            if(calculatedY < -30.0f)
                break;

            currentPoint = startPositionXZPlane + throwDirXZPlane;
            currentPoint.y = calculatedY;

            if(castRayBetweenPoints)
            {
                Beryll::RayClosestHit hit = Beryll::Physics::castRayClosestHit(previousPoint, currentPoint,
                                                                               physicObj->getCollisionGroup(),
                                                                               Beryll::CollisionGroups::GROUND | Beryll::CollisionGroups::BUILDING |
                                                                               Beryll::CollisionGroups::JUMPPAD | Beryll::CollisionGroups::BOSS);

                if(hit)
                {
                    m_hitPoint.push_back(hit.hitPoint);
                    break;
                }
            }

            m_allTrajectoryPoints.push_back(currentPoint);

            previousPoint = currentPoint;
            castRayBetweenPoints = true;
        }

        // Draw.
        Beryll::Renderer::setDepthFunctionAlways();

        m_shader->bind();
        m_shader->set3Float("sunLightDir", sunLightDir);
        m_shader->set3Float("cameraPos", Beryll::Camera::getCameraPos());
        m_shader->set1Float("ambientLight", 0.8f);
        m_shader->set1Float("specularLightStrength", 2.0f);

        if(!m_hitPoint.empty())
        {
            m_trajectoryHitPoint->setOrigin(m_hitPoint[0]);

            float scaleFactor = 1.0f + glm::distance(startPosition, m_hitPoint[0]) / 350.0f; // Scale to + 100% size every 350m.
            m_modelMatrix = m_trajectoryHitPoint->getModelMatrix() * glm::scale(glm::mat4{1.0f}, glm::vec3{scaleFactor});
            m_shader->setMatrix4x4Float("modelMatrix", m_modelMatrix);
            m_shader->setMatrix3x3Float("normalMatrix", glm::mat3(m_modelMatrix));
            Beryll::Renderer::drawObject(m_trajectoryHitPoint, m_modelMatrix, m_shader);
        }

        std::reverse(m_allTrajectoryPoints.begin(), m_allTrajectoryPoints.end());
        for(const glm::vec3& point : m_allTrajectoryPoints)
        {
            m_trajectoryPoint->setOrigin(point);

            m_modelMatrix = m_trajectoryPoint->getModelMatrix();
            m_shader->setMatrix4x4Float("modelMatrix", m_modelMatrix);
            m_shader->setMatrix3x3Float("normalMatrix", glm::mat3(m_modelMatrix));
            Beryll::Renderer::drawObject(m_trajectoryPoint, m_modelMatrix, m_shader);
        }

        Beryll::Renderer::setDepthFunctionLess();
    }
}
