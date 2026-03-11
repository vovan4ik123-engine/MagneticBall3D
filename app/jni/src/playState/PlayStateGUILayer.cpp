#include "PlayStateGUILayer.h"
#include "EnumsAndVariables.h"
#include "GameStateHelper.h"
#include "DataBaseHelper.h"
#include "SendStatisticsHelper.h"
#include "Sounds.h"

namespace MagneticBall3D
{
    std::atomic<bool> PlayStateGUILayer::m_resurrectAdSuccess = false;
    std::atomic<bool> PlayStateGUILayer::m_winPrize2AdSuccess = false;
    std::atomic<bool> PlayStateGUILayer::m_commonAdError = false;
    std::atomic<bool> PlayStateGUILayer::m_interruptAdSuccessError = false;

    PlayStateGUILayer::PlayStateGUILayer()
    {
        m_ID = Beryll::LayerID::PLAY_GUI;

        const float screenAR = Beryll::Window::getInstance()->getScreenAspectRatio();

        if(m_statisticsShow)
        {
            m_statistics1 = Beryll::Renderer::createGUIText("", glm::vec3{0.06f, 0.06f, 0.06f}, glm::vec3{6.5f, 96.3f, 0.0f}, 3.0f);
            m_guiObjects.push_back(m_statistics1);
            m_statistics2 = Beryll::Renderer::createGUIText("", glm::vec3{0.06f, 0.06f, 0.06f}, glm::vec3{6.5f, 92.3f, 0.0f}, 3.0f);
            m_guiObjects.push_back(m_statistics2);
            m_statistics3 = Beryll::Renderer::createGUIText("", glm::vec3{0.06f, 0.06f, 0.06f}, glm::vec3{6.5f, 88.3f, 0.0f}, 3.0f);
            m_guiObjects.push_back(m_statistics3);
        }

        playerJoystick = std::make_shared<Beryll::Joystick>("GUI/playState/Joystick.png","",
                                                            glm::vec3{0.0f, 0.0f, 0.1f}, glm::vec2{120.0f / screenAR, 120.0f});
        m_guiObjects.push_back(playerJoystick);
        playerJoystick->disable();

        m_buttonShot = std::make_shared<Beryll::ButtonWithTexture>("GUI/playState/ShotButton.png", "",
                                                                   glm::vec3{76.5f, 48.0f, 0.5f}, glm::vec2{20.0f / screenAR, 20.0f});

        m_buttonPause = std::make_shared<Beryll::ButtonWithTexture>("GUI/playState/Pause.jpg", "",
                                                                    glm::vec3{0.0f, 88.0f, 0.5f}, glm::vec2{6.0f, 12.0f});
        m_guiObjects.push_back(m_buttonPause);
        m_buttonResume = std::make_shared<Beryll::ButtonWithTexture>("GUI/playState/Resume.png", "",
                                                                     glm::vec3{38.0f, 41.0f, 0.5f}, glm::vec2{24.0f, 18.0f});
        m_buttonResume->disable();
        m_guiObjects.push_back(m_buttonResume);

        m_buttonExit = std::make_shared<Beryll::ButtonWithTexture>("GUI/Exit.png", "",
                                                                   glm::vec3{43.0f, 15.0f, 0.5f}, glm::vec2{14.0f, 15.0f});
        m_buttonExit->disable();
        m_guiObjects.push_back(m_buttonExit);

        m_textureLose = std::make_shared<Beryll::GUITexture>("GUI/playState/Lose.jpg",
                                                             glm::vec3{35.0f, 44.0f, 0.5f}, glm::vec2{30.0f, 34.0f});
        m_textureLose->disable();
        m_guiObjects.push_back(m_textureLose);

        m_textureWin = std::make_shared<Beryll::GUITexture>("GUI/playState/Win.jpg",
                                                            glm::vec3{35.0f, 44.0f, 0.5f}, glm::vec2{30.0f, 34.0f});
        m_textureWin->disable();
        m_guiObjects.push_back(m_textureWin);

        m_buttonWinPrize1 = std::make_shared<Beryll::ButtonWithTexture>("GUI/playState/WinPrize1.png", "",
                                                                        glm::vec3{35.0f, 25.0f, 0.5f}, glm::vec2{14.0f, 15.0f});
        m_buttonWinPrize1->disable();
        m_guiObjects.push_back(m_buttonWinPrize1);
        m_buttonWinPrize2 = std::make_shared<Beryll::ButtonWithTexture>("GUI/playState/WinPrize2Ad.png", "",
                                                                        glm::vec3{51.0f, 25.0f, 0.5f}, glm::vec2{14.0f, 15.0f});
        m_buttonWinPrize2->disable();
        m_guiObjects.push_back(m_buttonWinPrize2);


        m_textureTutorialCompleted = std::make_shared<Beryll::GUITexture>("GUI/playState/TutorialCompleted.jpg",
                                                                          glm::vec3{35.0f, 44.0f, 0.5f}, glm::vec2{30.0f, 34.0f});
        m_textureTutorialCompleted->disable();
        m_guiObjects.push_back(m_textureTutorialCompleted);
        m_buttonOkTutorialCompleted = std::make_shared<Beryll::ButtonWithTexture>("GUI/Ok.png", "",
                                                                 glm::vec3{43.0f, 15.0f, 0.5f}, glm::vec2{14.0f, 15.0f});
        m_buttonOkTutorialCompleted->disable();
        m_guiObjects.push_back(m_buttonOkTutorialCompleted);

        m_textureCounters = std::make_shared<Beryll::GUITexture>("GUI/playState/Counters.jpg",
                                                                 glm::vec3{91.0f, 85.5f, 0.5f}, glm::vec2{9.0f, 14.5f});
        m_guiObjects.push_back(m_textureCounters);
        m_textEnemiesKilled = Beryll::Renderer::createGUIText("", glm::vec3{0.06f, 0.06f, 0.06f}, glm::vec3{93.6f, 96.2f, 0.6f}, 3.0f);
        m_guiObjects.push_back(m_textEnemiesKilled);
        m_textSpeed = Beryll::Renderer::createGUIText("", glm::vec3{0.06f, 0.06f, 0.06f}, glm::vec3{93.6f, 91.5f, 0.6f}, 3.0f);
        m_guiObjects.push_back(m_textSpeed);
        m_textPlayTime = Beryll::Renderer::createGUIText("", glm::vec3{0.06f, 0.06f, 0.06f}, glm::vec3{93.6f, 86.9f, 0.6f}, 3.0f);
        m_guiObjects.push_back(m_textPlayTime);

        m_progressHP = std::make_shared<Beryll::GUIProgressBar>(glm::vec3{91.0f, 82.5f, 0.5f}, glm::vec2{9.0f, 3.0f},
                                                                glm::vec3{0.6289f, 1.0f, 0.3086f}, glm::vec3{0.8516f, 0.0859f, 0.1641f});
        m_guiObjects.push_back(m_progressHP);
        m_progressXP = std::make_shared<Beryll::GUIProgressBar>(glm::vec3{91.0f, 79.5f, 0.5f}, glm::vec2{9.0f, 3.0f},
                                                                glm::vec3{0.1367f, 0.6016f, 0.953f}, glm::vec3{0.1953f, 0.1953f, 0.2422f});
        m_guiObjects.push_back(m_progressXP);

        m_textureTutorialMove = std::make_shared<Beryll::GUITexture>("GUI/playState/TutorialMove.png",
                                                                     glm::vec3{5.0f, 5.0f, 0.6f}, glm::vec2{30.0f, 50.0f}, false);
        m_textureTutorialMove->disable();
        m_guiObjects.push_back(m_textureTutorialMove);
        m_textureTutorialCamera = std::make_shared<Beryll::GUITexture>("GUI/playState/TutorialCamera.png",
                                                                       glm::vec3{65.0f, 5.0f, 0.6f}, glm::vec2{30.0f, 50.0f}, false);
        m_textureTutorialCamera->disable();
        m_guiObjects.push_back(m_textureTutorialCamera);
        m_textureTutorialKillEnemies = std::make_shared<Beryll::GUITexture>("GUI/playState/TutorialKillEnemies.png",
                                                                            glm::vec3{28.0f, 90.6f, 0.6f}, glm::vec2{44.0f, 9.4f});
        m_textureTutorialKillEnemies->disable();
        m_guiObjects.push_back(m_textureTutorialKillEnemies);
        m_textureTutorialHealth = std::make_shared<Beryll::GUITexture>("GUI/playState/TutorialHealthExperience.png",
                                                                       glm::vec3{78.0f, 79.0f, 0.6f}, glm::vec2{13.0f, 10.0f});
        m_textureTutorialHealth->disable();
        m_guiObjects.push_back(m_textureTutorialHealth);

        // Next objects handled without m_guiObjects. Should be shown even if interface disabled.
        m_textureResurrect = std::make_shared<Beryll::GUITexture>("GUI/playState/CanResurrect.jpg",
                                                                  glm::vec3{35.0f, 49.0f, 0.6f}, glm::vec2{30.0f, 34.0f});
        m_textureResurrect->disable();
        m_buttonResurrectByCrystals = std::make_shared<Beryll::ButtonWithTexture>("GUI/playState/ResurrectByCrystals.png", "",
                                                                                  glm::vec3{35.0f, 32.0f, 0.6f}, glm::vec2{14.0f, 15.0f});
        m_buttonResurrectByCrystals->disable();
        m_buttonResurrectByAd = std::make_shared<Beryll::ButtonWithTexture>("GUI/playState/ResurrectByAd.png", "",
                                                                            glm::vec3{51.0f, 32.0f, 0.6f}, glm::vec2{14.0f, 15.0f});
        m_buttonResurrectByAd->disable();

        m_textureBackgroundError = std::make_shared<Beryll::GUITexture>("GUI/GrayTransparency10.png",
                                                                        glm::vec3{0.0f, 0.0f, 0.9f}, glm::vec2{100.0f, 100.0f});
        m_textureBackgroundError->disable();
        m_textureAdError = std::make_shared<Beryll::GUITexture>("GUI/AdError.jpg",
                                                                glm::vec3{35.0f, 35.0f, 1.0f}, glm::vec2{30.0f, 50.0f});
        m_textureAdError->disable();
        m_textureCrystalsError = std::make_shared<Beryll::GUITexture>("GUI/NotEnoughCrystals.jpg",
                                                                      glm::vec3{35.0f, 35.0f, 1.0f}, glm::vec2{30.0f, 50.0f});
        m_textureCrystalsError->disable();
        m_buttonCloseError = std::make_shared<Beryll::ButtonWithTexture>("GUI/Ok.png", "",
                                                                         glm::vec3{43.0f, 15.0f, 1.0f}, glm::vec2{14.0f, 15.0f});
        m_buttonCloseError->disable();

        m_textureTankWithCommander = std::make_shared<Beryll::GUITexture>("GUI/playState/BossTankWithCommander.jpg",
                                                                          glm::vec3{35.0f, 35.0f, 0.6f}, glm::vec2{30.0f, 50.0f});
        m_textureTankWithCommander->disable();
        m_buttonCloseBossMessage = std::make_shared<Beryll::ButtonWithTexture>("GUI/Ok.png", "",
                                                                               glm::vec3{43.0f, 15.0f, 0.6f}, glm::vec2{14.0f, 15.0f});
        m_buttonCloseBossMessage->disable();

        // Sort to update nearest objects first. But draw should starts from farest object(in reverse order).
        std::sort(m_guiObjects.begin(), m_guiObjects.end(), [](std::shared_ptr<Beryll::GUIObject> o1, std::shared_ptr<Beryll::GUIObject> o2)
        {
            return (o1->getPositionNormalized().z > o2->getPositionNormalized().z);
        });

        PlayStateGUILayer::m_resurrectAdSuccess = false;
        PlayStateGUILayer::m_winPrize2AdSuccess = false;
        PlayStateGUILayer::m_commonAdError = false;
        PlayStateGUILayer::m_interruptAdSuccessError = false;

        // These callbacks are called from different thread.
        m_resurrectAdSuccessCallback = []() -> void { BR_INFO("%s", "m_resurrectAdSuccessCallback()"); PlayStateGUILayer::m_resurrectAdSuccess = true; };
        m_winPrize2AdSuccessCallback = []() -> void { BR_INFO("%s", "m_winPrize2AdSuccessCallback()"); PlayStateGUILayer::m_winPrize2AdSuccess = true; };
        m_commonAdErrorCallback = []() -> void { BR_INFO("%s", "m_commonAdErrorCallback()"); PlayStateGUILayer::m_commonAdError = true; };
        m_interruptAdSuccessErrorCallback = [](){ BR_INFO("%s", "m_interruptAdSuccessErrorCallback()"); PlayStateGUILayer::m_interruptAdSuccessError = true; };
    }

    PlayStateGUILayer::~PlayStateGUILayer()
    {

    }

    void PlayStateGUILayer::updateBeforePhysics()
    {
        if(EnumsAndVars::improvementSystemOnScreen)
            return;

        // Buttons first.
        if(m_buttonCloseError->getIsEnabled()) { m_buttonCloseError->updateBeforePhysics(); }
        if(m_buttonResurrectByCrystals->getIsEnabled()) { m_buttonResurrectByCrystals->updateBeforePhysics(); }
        if(m_buttonResurrectByAd->getIsEnabled()) { m_buttonResurrectByAd->updateBeforePhysics(); }
        if(m_buttonCloseBossMessage->getIsEnabled()) { m_buttonCloseBossMessage->updateBeforePhysics(); }
        // Texture consume touch events.
        if(m_textureBackgroundError->getIsEnabled()) { m_textureBackgroundError->updateBeforePhysics(); }

        for(const std::shared_ptr<Beryll::GUIObject>& go : m_guiObjects)
        {
            if(go->getIsEnabled())
            {
                go->updateBeforePhysics();
            }
        }

        m_textEnemiesKilled->text = std::to_string(EnumsAndVars::enemiesKilledCount);

        m_textSpeed->text = std::to_string(int(std::roundf(EnumsAndVars::playerCurrentSpeed)));
        m_textSpeed->text += '/';
        m_textSpeed->text += std::to_string(int(std::roundf(EnumsAndVars::playerMaxSpeedXZ)));

        m_textPlayTime->text = "";
        const int min = int(EnumsAndVars::mapPlayTimeSec / 60.0f);
        const int sec = int(std::fmod(EnumsAndVars::mapPlayTimeSec, 60.0f));
        if(min < 10)
            m_textPlayTime->text += "0";

        m_textPlayTime->text += std::to_string(min);
        m_textPlayTime->text += ":";

        if(sec < 10)
            m_textPlayTime->text += "0";

        m_textPlayTime->text += std::to_string(sec);

        m_progressHP->setProgress(progressBarHP);
        m_progressXP->setProgress(progressBarXP);

        if(tutorialTextTipsShow)
        {
            m_textureTutorialKillEnemies->enable();
            m_textureTutorialHealth->enable();

            if(tutorialMoveShow)
            {
                m_textureTutorialMove->enable();
                m_textureTutorialCamera->enable();
            }
            else
            {
                m_textureTutorialMove->disable();
                m_textureTutorialCamera->disable();
            }
        }
        else
        {
            m_textureTutorialKillEnemies->disable();
            m_textureTutorialHealth->disable();
            m_textureTutorialMove->disable();
            m_textureTutorialCamera->disable();
        }

        if(EnumsAndVars::SettingsMenu::interfaceGUI)
        {
            m_buttonShot->updateBeforePhysics();
        }

        if(m_statisticsShow && Beryll::TimeStep::getMilliSecFromStart() > m_statisticsUpdateTime + 200) // Update every 200 ms.
        {
            std::stringstream stream;
            stream << std::fixed << std::setprecision(1);
            stream << "Frame: " << Beryll::GameLoop::getFrameTime() << "  FPS: " << Beryll::GameLoop::getFPS();
            m_statistics1->text = stream.str();

            stream.str(""); // Way to clear std::stringstream.
            stream << std::fixed << std::setprecision(1);
            stream << "Phys: " << Beryll::Physics::getSimulationTime();
            stream << "  Logic: " << (Beryll::GameLoop::getCPUTime() - Beryll::Physics::getSimulationTime());
            stream << "  GPU: " << Beryll::GameLoop::getGPUTime();
            m_statistics2->text = stream.str();

            stream.str("");
            stream << "Time: " << int(EnumsAndVars::mapPlayTimeSec / 60.0f) << ":" << int(std::fmod(EnumsAndVars::mapPlayTimeSec, 60.0f));
            m_statistics3->text = stream.str();

            m_statisticsUpdateTime = Beryll::TimeStep::getMilliSecFromStart();
        }

        if(m_buttonPause->getIsPressed() && !EnumsAndVars::gameOnPause)
        {
            m_buttonResume->enable();
            m_buttonExit->enable();

            Sounds::pauseBackgroundMusic();
            GameStateHelper::pauseGame();
        }
        else if(m_buttonResume->getIsPressed())
        {
            m_buttonResume->disable();
            m_buttonExit->disable();

            Sounds::resumeBackgroundMusic();
            GameStateHelper::resumeGame();
        }
        else if(m_buttonExit->getIsPressed())
        {
            Sounds::stopBackgroundMusic();

            GameStateHelper::popState();
            GameStateHelper::pushStartMenuState();
            return;
        }
        else if(m_buttonWinPrize1->getIsPressed())
        {
            EnumsAndVars::CurrencyBalance::crystals += 25;
            DataBaseHelper::storeCurrencyBalanceCrystals(EnumsAndVars::CurrencyBalance::crystals);

            GameStateHelper::popState();
            GameStateHelper::pushStartMenuState();
            return;
        }
        else if(m_buttonWinPrize2->getIsPressed())
        {
            if(EnumsAndVars::Shop::adsOnMapsDisabled)
            {
                m_winPrize2AdSuccessCallback();
            }
            else
            {
                SendStatisticsHelper::sendCustomMessage("attempt_show_ad");
                Beryll::Ads::getInstance()->showInterstitialAd(m_winPrize2AdSuccessCallback, m_commonAdErrorCallback);
            }
        }
        else if(m_buttonOkTutorialCompleted->getIsPressed())
        {
            Sounds::stopBackgroundMusic();

            GameStateHelper::popState();
            GameStateHelper::pushStartMenuState();
            return;
        }
        else if(m_buttonCloseBossMessage->getIsPressed())
        {
            m_textureTankWithCommander->disable();
            m_buttonCloseBossMessage->disable();

            GameStateHelper::resumeGame();
        }
        else if(m_buttonResurrectByAd->getIsPressed())
        {
            BR_INFO("%s", "m_resurrectByAdButtonClicked");
            Sounds::pauseBackgroundMusic();

            if(EnumsAndVars::Shop::adsOnMapsDisabled)
            {
                m_resurrectAdSuccessCallback();
            }
            else
            {
                SendStatisticsHelper::sendCustomMessage("attempt_show_ad");
                Beryll::Ads::getInstance()->showInterstitialAd(m_resurrectAdSuccessCallback, m_commonAdErrorCallback);
            }
        }
        else if(m_buttonResurrectByCrystals->getIsPressed())
        {
            BR_INFO("%s", "m_resurrectByCrystalsButtonClicked");

            if(EnumsAndVars::CurrencyBalance::crystals >= EnumsAndVars::playerCostOfResurrectionCrystals)
            {
                m_textureResurrect->disable();
                m_buttonResurrectByCrystals->disable();
                m_buttonResurrectByAd->disable();
                m_buttonExit->disable();

                EnumsAndVars::CurrencyBalance::crystals -= EnumsAndVars::playerCostOfResurrectionCrystals;
                DataBaseHelper::storeCurrencyBalanceCrystals(EnumsAndVars::CurrencyBalance::crystals);
                resurrectPlayer = true; // Will handled in BaseMap.cpp
                GameStateHelper::resumeGame();
            }
            else
            {
                m_textureBackgroundError->enable();
                m_textureCrystalsError->enable();
                m_buttonCloseError->enable();
            }
        }

        if(PlayStateGUILayer::m_resurrectAdSuccess)
        {
            PlayStateGUILayer::m_resurrectAdSuccess = false;

            m_textureResurrect->disable();
            m_buttonResurrectByCrystals->disable();
            m_buttonResurrectByAd->disable();
            m_buttonExit->disable();

            resurrectPlayer = true; // Will handled in BaseMap.cpp
            GameStateHelper::resumeGame();
            Sounds::resumeBackgroundMusic();
        }

        if(PlayStateGUILayer::m_interruptAdSuccessError)
        {
            PlayStateGUILayer::m_interruptAdSuccessError = false;

            Sounds::resumeBackgroundMusic();
            GameStateHelper::resumeGame();
        }

        if(PlayStateGUILayer::m_winPrize2AdSuccess)
        {
            PlayStateGUILayer::m_winPrize2AdSuccess = false;

            EnumsAndVars::CurrencyBalance::crystals += 50;
            DataBaseHelper::storeCurrencyBalanceCrystals(EnumsAndVars::CurrencyBalance::crystals);

            GameStateHelper::popState();
            GameStateHelper::pushStartMenuState();
            return;
        }

        if(PlayStateGUILayer::m_commonAdError)
        {
            PlayStateGUILayer::m_commonAdError = false;

            m_textureBackgroundError->enable();
            m_textureAdError->enable();
            m_buttonCloseError->enable();
        }

        if(m_buttonCloseError->getIsPressed())
        {
            BR_INFO("%s", "_buttonCloseError->getIsPressed");
            m_textureBackgroundError->disable();
            m_textureAdError->disable();
            m_textureCrystalsError->disable();
            m_buttonCloseError->disable();
        }
    }

    void PlayStateGUILayer::updateAfterPhysics()
    {
        // Dont need update.
    }

    void PlayStateGUILayer::draw()
    {
        if(EnumsAndVars::SettingsMenu::interfaceGUI)
        {
            for(auto it = m_guiObjects.rbegin(); it != m_guiObjects.rend(); ++it)
            {
                if((*it)->getIsEnabled())
                {
                    (*it)->draw();
                }
            }

            m_buttonShot->draw();
        }

        // Messages about map progress / win / lose ... should be shown even if interface disabled.

        if(m_textureResurrect->getIsEnabled()) { m_textureResurrect->draw(); }
        if(m_buttonResurrectByCrystals->getIsEnabled()) { m_buttonResurrectByCrystals->draw(); }
        if(m_buttonResurrectByAd->getIsEnabled()) { m_buttonResurrectByAd->draw(); }
        if(m_textureBackgroundError->getIsEnabled()) { m_textureBackgroundError->draw(); }
        if(m_textureAdError->getIsEnabled()) { m_textureAdError->draw(); }
        if(m_textureCrystalsError->getIsEnabled()) { m_textureCrystalsError->draw(); }
        if(m_buttonCloseError->getIsEnabled()) { m_buttonCloseError->draw(); }
        if(m_textureTankWithCommander->getIsEnabled()) { m_textureTankWithCommander->draw(); }
        if(m_buttonCloseBossMessage->getIsEnabled()) { m_buttonCloseBossMessage->draw(); }
    }

    void PlayStateGUILayer::showMenuResurrect()
    {
        m_textureResurrect->enable();
        m_buttonResurrectByCrystals->enable();
        m_buttonResurrectByAd->enable();
        m_buttonExit->enable();

        GameStateHelper::pauseGame();
    }

    void PlayStateGUILayer::showMenuLose()
    {
        m_textureLose->enable();
        m_buttonExit->enable();

        Sounds::stopBackgroundMusic();
        GameStateHelper::pauseGame();
    }

    void PlayStateGUILayer::showMenuWin()
    {
        if(EnumsAndVars::MapsProgress::mapIndexWhenMapStart == 0) // Tutorial completed.
        {
            m_textureTutorialCompleted->enable();
            m_buttonOkTutorialCompleted->enable();
        }
        else
        {
            m_textureWin->enable();
            m_buttonWinPrize1->enable();
            m_buttonWinPrize2->enable();
        }

        Sounds::stopBackgroundMusic();
        GameStateHelper::pauseGame();
    }

    void PlayStateGUILayer::showMenuBossTankWithCommander()
    {
        m_textureTankWithCommander->enable();
        m_buttonCloseBossMessage->enable();

        GameStateHelper::pauseGame();
    }
}
