#include "PlayerTalentsGUILayer.h"
#include "EnumsAndVariables.h"
#include "GameStateHelper.h"
#include "DataBaseHelper.h"
#include "SendStatisticsHelper.h"

namespace MagneticBall3D
{
    int PlayerTalentsGUILayer::m_selectedIndex = 0;
    std::atomic<bool> PlayerTalentsGUILayer::m_adSuccess = false;
    std::atomic<bool> PlayerTalentsGUILayer::m_adError = false;

    PlayerTalentsGUILayer::PlayerTalentsGUILayer()
    {
        const float screenAR = Beryll::Window::getInstance()->getScreenAspectRatio();

        auto background = std::make_shared<Beryll::GUITexture>("GUI/menus/playerTalents/TalentsBackground.jpg",
                                                                                     glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec2{100.0f, 100.0f});
        m_guiObjects.push_back(background);
        m_buttonBack = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/LeftArrow.png", "",
                                                                   glm::vec3{3.0f, 5.0f, 0.1f}, glm::vec2{15.0f, 15.0f});
        m_guiObjects.push_back(m_buttonBack);
        m_buttonMaxSpeed = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/playerTalents/MaxSpeed.jpg", "",
                                                                       glm::vec3{6.0f, 60.0f, 0.1f}, glm::vec2{14.0f, 30.0f});
        m_guiObjects.push_back(m_buttonMaxSpeed);
        m_buttonMagneticRadius = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/playerTalents/MagneticRadius.jpg", "",
                                                                             glm::vec3{21.0f, 45.0f, 0.1f}, glm::vec2{14.0f, 30.0f});
        m_guiObjects.push_back(m_buttonMagneticRadius);
        m_buttonAmountOfMagnetizedItems = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/playerTalents/AmountOfMagnetizedItems.jpg", "",
                                                                                      glm::vec3{36.0f, 45.0f, 0.1f}, glm::vec2{14.0f, 30.0f});
        m_guiObjects.push_back(m_buttonAmountOfMagnetizedItems);
        m_buttonAccelerateFaster = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/playerTalents/AccelerateFaster.jpg", "",
                                                                               glm::vec3{51.0f, 45.0f, 0.1f}, glm::vec2{14.0f, 30.0f});
        m_guiObjects.push_back(m_buttonAccelerateFaster);
        m_buttonBallAndItemsProtection = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/playerTalents/BallAndItemsProtection.jpg", "",
                                                                                     glm::vec3{66.0f, 45.0f, 0.1f}, glm::vec2{14.0f, 30.0f});
        m_guiObjects.push_back(m_buttonBallAndItemsProtection);
        m_buttonResurrectionAttempts = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/playerTalents/ResurrectionAttempts.jpg", "",
                                                                                   glm::vec3{81.0f, 60.0f, 0.1f}, glm::vec2{14.0f, 30.0f});
        m_guiObjects.push_back(m_buttonResurrectionAttempts);
        m_buttonSmashDamage = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/playerTalents/SmashDamage.jpg", "",
                                                                          glm::vec3{6.0f, 27.0f, 0.1f}, glm::vec2{14.0f, 30.0f});
        m_guiObjects.push_back(m_buttonSmashDamage);
        m_buttonShotDamage = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/playerTalents/ShotDamage.jpg", "",
                                                                         glm::vec3{81.0f, 27.0f, 0.1f}, glm::vec2{14.0f, 30.0f});
        m_guiObjects.push_back(m_buttonShotDamage);

        auto selectedTalentBackground = std::make_shared<Beryll::GUITexture>("GUI/menus/playerTalents/SelectedTalentBackground.jpg",
                                                                                                   glm::vec3{24.0f, 15.0f, 0.1f}, glm::vec2{52.0f, 20.0f});
        m_guiObjects.push_back(selectedTalentBackground);

        m_textureImproveByAd = std::make_shared<Beryll::GUITexture>("GUI/menus/playerTalents/AdImage.jpg",
                                                                    glm::vec3{53.0f, 15.0f, 0.2f}, glm::vec2{9.2f, 20.0f});
        m_guiObjects.push_back(m_textureImproveByAd);
        m_textureImproveByAdTimer = std::make_shared<Beryll::GUITexture>("GUI/menus/playerTalents/AdImageTimer.jpg",
                                                                         glm::vec3{53.0f, 15.0f, 0.2f}, glm::vec2{9.2f, 20.0f});
        m_guiObjects.push_back(m_textureImproveByAdTimer);
        m_textureImproveByCrystals = std::make_shared<Beryll::GUITexture>("GUI/menus/playerTalents/CrystalsImage.jpg",
                                                                          glm::vec3{67.0f, 15.0f, 0.2f}, glm::vec2{9.2f, 20.0f});
        m_guiObjects.push_back(m_textureImproveByCrystals);
        m_textureMaxLevelReached = std::make_shared<Beryll::GUITexture>("GUI/menus/playerTalents/MaxLevelReached.jpg",
                                                                        glm::vec3{53.0f, 15.0f, 0.2f}, glm::vec2{23.0f, 20.0f});
        m_guiObjects.push_back(m_textureMaxLevelReached);

        m_textSelectedDescription1 = Beryll::Renderer::createGUIText("", glm::vec3{0.95f, 0.95f, 0.95f}, glm::vec3{24.5f, 29.0f, 0.3f}, 0.4f);
        m_guiObjects.push_back(m_textSelectedDescription1);
        m_textSelectedDescription2 = Beryll::Renderer::createGUIText("", glm::vec3{0.95f, 0.95f, 0.95f}, glm::vec3{24.5f, 24.0f, 0.3f}, 0.4f);
        m_guiObjects.push_back(m_textSelectedDescription2);
        m_textSelectedValue = Beryll::Renderer::createGUIText("", glm::vec3{0.95f, 0.95f, 0.95f}, glm::vec3{24.5f, 16.0f, 0.2f}, 0.3f);
        m_guiObjects.push_back(m_textSelectedValue);
        m_textSelectedValueToAdd = Beryll::Renderer::createGUIText("", glm::vec3{0.95f, 0.95f, 0.95f}, glm::vec3{47.5f, 23.0f, 0.3f}, 0.5f);
        m_guiObjects.push_back(m_textSelectedValueToAdd);
        m_textOr = Beryll::Renderer::createGUIText("OR", glm::vec3{0.95f, 0.95f, 0.95f}, glm::vec3{62.75f, 23.0f, 0.3f}, 0.5f);
        m_guiObjects.push_back(m_textOr);
        m_textAdTimer = Beryll::Renderer::createGUIText("", glm::vec3{0.95f, 0.95f, 0.95f}, glm::vec3{54.2f, 23.0f, 0.3f}, 0.5f);
        m_guiObjects.push_back(m_textAdTimer);
        m_textSelectedPriceCrystals = Beryll::Renderer::createGUIText("", glm::vec3{0.1f, 0.1f, 0.1f}, glm::vec3{69.0f, 17.0f, 0.3f}, 0.5f);
        m_guiObjects.push_back(m_textSelectedPriceCrystals);

        m_buttonImproveTalentByAd = std::make_shared<Beryll::ButtonWithTexture>("GUI/FullTransparent.png", "GUI/GrayTransparency20.png",
                                                                                glm::vec3{53.0f, 15.0f, 0.4f}, glm::vec2{9.2f, 20.0f});
        m_guiObjects.push_back(m_buttonImproveTalentByAd);
        m_buttonImproveTalentByCrystal = std::make_shared<Beryll::ButtonWithTexture>("GUI/FullTransparent.png", "GUI/GrayTransparency20.png",
                                                                                     glm::vec3{67.0f, 15.0f, 0.4f}, glm::vec2{9.2f, 20.0f});
        m_guiObjects.push_back(m_buttonImproveTalentByCrystal);

        // Error menus.
        m_textureBackgroundError = std::make_shared<Beryll::GUITexture>("GUI/GrayTransparency10.png",
                                                                        glm::vec3{0.0f, 0.0f, 0.9f}, glm::vec2{100.0f, 100.0f});
        m_textureBackgroundError->disable();
        m_guiObjects.push_back(m_textureBackgroundError);
        m_textureAdError = std::make_shared<Beryll::GUITexture>("GUI/AdError.jpg",
                                                                glm::vec3{35.0f, 35.0f, 1.0f}, glm::vec2{30.0f, 50.0f});
        m_textureAdError->disable();
        m_guiObjects.push_back(m_textureAdError);
        m_textureCrystalsError = std::make_shared<Beryll::GUITexture>("GUI/NotEnoughCrystals.jpg",
                                                                      glm::vec3{35.0f, 35.0f, 1.0f}, glm::vec2{30.0f, 50.0f});
        m_textureCrystalsError->disable();
        m_guiObjects.push_back(m_textureCrystalsError);
        m_buttonCloseError = std::make_shared<Beryll::ButtonWithTexture>("GUI/Ok.png", "",
                                                                         glm::vec3{43.0f, 15.0f, 1.0f}, glm::vec2{14.0f, 15.0f});
        m_buttonCloseError->disable();
        m_guiObjects.push_back(m_buttonCloseError);

        // Sort to update nearest objects first. But draw should starts from farest object(in reverse order).
        std::sort(m_guiObjects.begin(), m_guiObjects.end(), [](std::shared_ptr<Beryll::GUIObject> o1, std::shared_ptr<Beryll::GUIObject> o2)
        {
            return (o1->getPositionNormalized().z > o2->getPositionNormalized().z);
        });

        selectTalent(m_selectedIndex);

        // These callbacks are called from different thread.
        m_adSuccessCallback = []() -> void { BR_INFO("%s", "m_resurrectAdSuccessCallback()"); PlayerTalentsGUILayer::m_adSuccess = true; };
        m_adErrorCallback = []() -> void { BR_INFO("%s", "m_adErrorCallback()"); PlayerTalentsGUILayer::m_adError = true; };
    }

    PlayerTalentsGUILayer::~PlayerTalentsGUILayer()
    {

    }

    void PlayerTalentsGUILayer::updateBeforePhysics()
    {
        for(const std::shared_ptr<Beryll::GUIObject>& go : m_guiObjects)
        {
            if(go->getIsEnabled())
            {
                go->updateBeforePhysics();
            }
        }

        if(m_buttonBack->getIsPressed())
        {
            GameStateHelper::popState();
            return;
        }
        else if(m_buttonMaxSpeed->getIsPressed())
        {
            BR_INFO("%s", "Max speed clicked.");
            selectTalent(0);
        }
        else if(m_buttonMagneticRadius->getIsPressed())
        {
            BR_INFO("%s", "Magnetic radius clicked.");
            selectTalent(1);
        }
        else if(m_buttonAmountOfMagnetizedItems->getIsPressed())
        {
            BR_INFO("%s", "Amount of magnetized clicked.");
            selectTalent(2);
        }
        else if(m_buttonAccelerateFaster->getIsPressed())
        {
            BR_INFO("%s", "Accelerate faster clicked.");
            selectTalent(3);
        }
        else if(m_buttonBallAndItemsProtection->getIsPressed())
        {
            BR_INFO("%s", "Ball and items protection clicked.");
            selectTalent(4);
        }
        else if(m_buttonResurrectionAttempts->getIsPressed())
        {
            BR_INFO("%s", "Resurrection attempts clicked.");
            selectTalent(5);
        }
        else if(m_buttonSmashDamage->getIsPressed())
        {
            BR_INFO("%s", "Smash damage clicked.");
            selectTalent(6);
        }
        else if(m_buttonShotDamage->getIsPressed())
        {
            BR_INFO("%s", "Shot damage clicked.");
            selectTalent(7);
        }
        else if(m_buttonImproveTalentByAd->getIsPressed())
        {
            BR_INFO("%s", "improveByAdClicked.");
            SendStatisticsHelper::sendCustomMessage("attempt_show_ad");
            Beryll::Ads::getInstance()->showInterstitialAd(m_adSuccessCallback, m_adErrorCallback);
        }
        else if(m_buttonImproveTalentByCrystal->getIsPressed())
        {
            BR_INFO("%s", "improveByCrystalsClicked.");

            if(EnumsAndVars::allPlayerTalents[m_selectedIndex].getCurrentLevelPriceCrystals() <= EnumsAndVars::CurrencyBalance::crystals)
            {
                EnumsAndVars::allPlayerTalents[m_selectedIndex].improveLevel(EnumsAndVars::PlayerTalentCurrency::CRYSTALS);
                selectTalent(m_selectedIndex); // Recalculate values.
                SendStatisticsHelper::sendTalentImproved(EnumsAndVars::allPlayerTalents[m_selectedIndex].name, "crystal");
            }
            else
            {
                m_textureBackgroundError->enable();
                m_textureCrystalsError->enable();
                m_buttonCloseError->enable();
            }
        }

        if(PlayerTalentsGUILayer::m_adSuccess)
        {
            PlayerTalentsGUILayer::m_adSuccess = false;
            EnumsAndVars::allPlayerTalents[m_selectedIndex].improveLevel(EnumsAndVars::PlayerTalentCurrency::AD);
            selectTalent(m_selectedIndex); // Recalculate values.
            SendStatisticsHelper::sendTalentImproved(EnumsAndVars::allPlayerTalents[m_selectedIndex].name, "ad");

            EnumsAndVars::Ads::rewardedAdTime = Beryll::TimeStep::getSecSinceEpoch();
            DataBaseHelper::storeAdsRewardedAdTime(EnumsAndVars::Ads::rewardedAdTime);
        }

        if(PlayerTalentsGUILayer::m_adError)
        {
            PlayerTalentsGUILayer::m_adError = false;
            m_textureBackgroundError->enable();
            m_textureAdError->enable();
            m_buttonCloseError->enable();
        }

        if(m_buttonCloseError->getIsPressed())
        {
            m_textureBackgroundError->disable();
            m_textureAdError->disable();
            m_textureCrystalsError->disable();
            m_buttonCloseError->disable();
        }

        m_textureImproveByAd->disable();
        m_textureImproveByAdTimer->disable();
        m_textureImproveByCrystals->disable();
        m_textureMaxLevelReached->disable();
        m_textOr->disable();
        m_textAdTimer->disable();
        m_textSelectedPriceCrystals->disable();
        if(m_selectedCurrentLevel < m_selectedMaxLevel) // Not max level.
        {
            if(m_selectedCanBeImprovedByAds)
            {
                m_textOr->enable();

                if(EnumsAndVars::Ads::rewardedAdTime + EnumsAndVars::Ads::rewardedAdTimeDelay <= Beryll::TimeStep::getSecSinceEpoch())
                {
                    m_textureImproveByAd->enable();
                    m_buttonImproveTalentByAd->enable();
                }
                else
                {
                    m_textureImproveByAdTimer->enable();
                    m_buttonImproveTalentByAd->disable();

                    // Show timer.
                    m_adTimerValue = "";
                    uint64_t secLeft = (EnumsAndVars::Ads::rewardedAdTime + EnumsAndVars::Ads::rewardedAdTimeDelay) - Beryll::TimeStep::getSecSinceEpoch();
                    if(secLeft > EnumsAndVars::Ads::rewardedAdTimeDelay)
                        secLeft = EnumsAndVars::Ads::rewardedAdTimeDelay;

                    int min = secLeft / 60;
                    int sec = secLeft % 60;

                    if(min < 10)
                        m_adTimerValue += "0";

                    m_adTimerValue += std::to_string(min);
                    m_adTimerValue += ":";

                    if(sec < 10)
                        m_adTimerValue += "0";

                    m_adTimerValue += std::to_string(sec);

                    m_textAdTimer->enable();
                    m_textAdTimer->text = m_adTimerValue;
                }
            }
            else
            {
                m_buttonImproveTalentByAd->disable();
            }

            m_textureImproveByCrystals->enable();
            m_buttonImproveTalentByCrystal->enable();

            if(m_selectedPriceCrystals < 10)
                m_textSelectedPriceCrystals->updatePositionInPercents(glm::vec3{70.8f, 17.0f, 0.3f});
            else if(m_selectedPriceCrystals < 100)
                m_textSelectedPriceCrystals->updatePositionInPercents(glm::vec3{70.0f, 17.0f, 0.3f});
            else
                m_textSelectedPriceCrystals->updatePositionInPercents(glm::vec3{69.3f, 17.0f, 0.3f});

            m_textSelectedPriceCrystals->enable();
            m_textSelectedPriceCrystals->text = std::to_string(m_selectedPriceCrystals);
        }
        else
        {
            m_textureMaxLevelReached->enable();
            m_buttonImproveTalentByAd->disable();
            m_buttonImproveTalentByCrystal->disable();
        }
    }

    void PlayerTalentsGUILayer::updateAfterPhysics()
    {

    }

    void PlayerTalentsGUILayer::draw()
    {
        for(auto it = m_guiObjects.rbegin(); it != m_guiObjects.rend(); ++it)
        {
            if((*it)->getIsEnabled())
            {
                (*it)->draw();
            }
        }
    }

    void PlayerTalentsGUILayer::selectTalent(int index)
    {
        BR_ASSERT((index < EnumsAndVars::allPlayerTalents.size()), "%s", "selectTalent(int index) index wrong.");
        m_selectedIndex = index;

        float currentValue = 0.0f;
        if(m_selectedIndex == 0) // Max speed. = 0
        {
            currentValue = EnumsAndVars::playerMaxSpeedXZDefault * (1.0f + EnumsAndVars::allPlayerTalents[m_selectedIndex].getPercentsToImprove() / 100.0f);
        }
        else if(m_selectedIndex == 1) // Magnetic radius. = 1
        {
            currentValue = EnumsAndVars::playerMagneticRadiusDefault * (1.0f + EnumsAndVars::allPlayerTalents[m_selectedIndex].getPercentsToImprove() / 100.0f);
        }
        else if(m_selectedIndex == 2) // Amount of magnetized items. = 2
        {
            currentValue = EnumsAndVars::garbageMaxCountMagnetizedDefault * (1.0f + EnumsAndVars::allPlayerTalents[m_selectedIndex].getPercentsToImprove() / 100.0f);
        }
        else if(m_selectedIndex == 3) // Accelerate faster. = 3
        {
            currentValue = EnumsAndVars::playerImpulseFactorOnGroundDefault * (1.0f + EnumsAndVars::allPlayerTalents[m_selectedIndex].getPercentsToImprove() / 100.0f);
        }
        else if(m_selectedIndex == 4) // Ball and items protection. = 4
        {
            currentValue = EnumsAndVars::garbageStartHPDefault * (1.0f + EnumsAndVars::allPlayerTalents[m_selectedIndex].getPercentsToImprove() / 100.0f);
        }
        else if(m_selectedIndex == 5) // Resurrection attempts. = 5
        {
            currentValue = EnumsAndVars::playerResurrectionAttemptsDefault * int(1.0f + std::roundf(EnumsAndVars::allPlayerTalents[m_selectedIndex].getPercentsToImprove() / 100.0f));
        }
        else if(m_selectedIndex == 6) // Smash damage. = 6
        {
            currentValue = EnumsAndVars::damageSmashDefault * (1.0f + EnumsAndVars::allPlayerTalents[m_selectedIndex].getPercentsToImprove() / 100.0f);
        }
        else if(m_selectedIndex == 7) // Shot Damage. = 7
        {
            currentValue = EnumsAndVars::damageShotDefault * (1.0f + EnumsAndVars::allPlayerTalents[m_selectedIndex].getPercentsToImprove() / 100.0f);
        }
        else
        {
            BR_ASSERT(false, "%s", "Unknown m_selectedIndex.");
        }

        std::stringstream stream;
        stream << std::fixed << std::setprecision(2) << currentValue;

        m_textSelectedDescription1->text = EnumsAndVars::allPlayerTalents[m_selectedIndex].descriptionLine1;
        m_textSelectedDescription2->text = EnumsAndVars::allPlayerTalents[m_selectedIndex].descriptionLine2;
        m_textSelectedValueToAdd->text = EnumsAndVars::allPlayerTalents[m_selectedIndex].increasePerLevelText;
        m_selectedPriceCrystals = EnumsAndVars::allPlayerTalents[m_selectedIndex].getCurrentLevelPriceCrystals();
        m_selectedCanBeImprovedByAds = EnumsAndVars::allPlayerTalents[m_selectedIndex].canBeImprovedByAd;
        m_selectedCurrentLevel = EnumsAndVars::allPlayerTalents[m_selectedIndex].currentLevel;
        m_selectedMaxLevel = EnumsAndVars::allPlayerTalents[m_selectedIndex].maxLevel;

        m_textSelectedValue->text = "Value:";
        m_textSelectedValue->text += stream.str();
        m_textSelectedValue->text += "  Level:";
        m_textSelectedValue->text += std::to_string(m_selectedCurrentLevel);
        m_textSelectedValue->text += "/";
        m_textSelectedValue->text += std::to_string(m_selectedMaxLevel);
    }
}
