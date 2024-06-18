#include "StartMenuGUILayer.h"
#include "EnumsAndVariables.h"
#include "GameStateHelper.h"
#include "DataBaseHelper.h"
#include "energySystem/EnergySystem.h"

namespace MagneticBall3D
{
    // All there IDs as strings required by ImGUI.
    const std::string StartMenuGUILayer::m_buttonPlayID = std::to_string(BeryllUtils::Common::generateID());
    const std::string StartMenuGUILayer::m_buttonSettingsID = std::to_string(BeryllUtils::Common::generateID());
    const std::string StartMenuGUILayer::m_buttonMapSwipeLeftID = std::to_string(BeryllUtils::Common::generateID());
    const std::string StartMenuGUILayer::m_buttonMapSwipeRightID = std::to_string(BeryllUtils::Common::generateID());
    const std::string StartMenuGUILayer::m_mapTextureID = std::to_string(BeryllUtils::Common::generateID());
    const std::string StartMenuGUILayer::m_textCrystalsID = std::to_string(BeryllUtils::Common::generateID());

    StartMenuGUILayer::StartMenuGUILayer()
    {
        m_ID = Beryll::LayerID::START_SCREEN_GUI;

        Beryll::Window::getInstance()->setClearColor(0.0666f * 0.7f, 0.7333f * 0.7f, 0.2f * 0.7f, 1.0f);

        m_allMapsTextures.push_back(Beryll::Renderer::createTexture("loadingScreen/Map0Screen.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1));
        m_allMapsTextures.push_back(Beryll::Renderer::createTexture("loadingScreen/Map1Screen.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1));

        m_buttonMapSwipeLeftTexture = Beryll::Renderer::createTexture("GUI/menus/start/MapSwipeLeft.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_buttonMapSwipeRightTexture = Beryll::Renderer::createTexture("GUI/menus/start/MapSwipeRight.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_buttonPlayTexture = Beryll::Renderer::createTexture("GUI/menus/start/Play.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_buttonSettingsTexture = Beryll::Renderer::createTexture("GUI/menus/start/Settings.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_fontCrystals = Beryll::MainImGUI::getInstance()->createFont(EnAndVars::FontsPath::creamy, 0.023f);
    }

    StartMenuGUILayer::~StartMenuGUILayer()
    {

    }

    void StartMenuGUILayer::updateBeforePhysics()
    {
        //EnergySystem::getInstance().update();

        if(m_buttonPlayClicked)
        {
            //if(EnergySystem::getInstance().isEnoughForPlay())
            {
                //EnergySystem::getInstance().handlePlay();

                GameStateHelper::popState();
                GameStateHelper::pushPlayState();
            }
            //else
            //{
                // Show menu for buy energy.
            //}
        }
//        else if(m_buttonShop)
//        {
//            GameStateHelper::pushShopState();
//        }
//        else if(m_buttonPlayerTalents)
//        {
//            GameStateHelper::pushPlayerTalentsState();
//        }
        else if(m_buttonSettingsClicked)
        {
            GameStateHelper::pushSettingsState();
        }
        else if(m_buttonMapSwipeLeftClicked && EnAndVars::MapsProgress::currentMapIndex > 0)
        {
            --EnAndVars::MapsProgress::currentMapIndex;
            BR_INFO("currentMapIndex: %d", EnAndVars::MapsProgress::currentMapIndex);
            DataBaseHelper::storeMapsProgressCurrentMapIndex(EnAndVars::MapsProgress::currentMapIndex);
        }
        else if(m_buttonMapSwipeRightClicked &&
                EnAndVars::MapsProgress::currentMapIndex < EnAndVars::MapsProgress::lastOpenedMapIndex &&
                EnAndVars::MapsProgress::currentMapIndex < EnAndVars::MapsProgress::maxMapIndex)
        {
            ++EnAndVars::MapsProgress::currentMapIndex;
            BR_INFO("currentMapIndex: %d", EnAndVars::MapsProgress::currentMapIndex);
            DataBaseHelper::storeMapsProgressCurrentMapIndex(EnAndVars::MapsProgress::currentMapIndex);
        }
    }

    void StartMenuGUILayer::updateAfterPhysics()
    {
        // Dont need update.
    }

    void StartMenuGUILayer::draw()
    {
        //EnergySystem::getInstance().draw();

        // Button play.
        ImGui::SetNextWindowPos(ImVec2(0.3f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.7f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.

        ImGui::Begin(m_buttonPlayID.c_str(), nullptr, m_noBackgroundNoFrame);

        m_buttonPlayClicked = ImGui::ImageButton(m_buttonPlayID.c_str(),reinterpret_cast<ImTextureID>(m_buttonPlayTexture->getID()),
                                                 ImVec2(0.4f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.09f * Beryll::MainImGUI::getInstance()->getGUIHeight()));

        ImGui::End();

        // Button settings.
        ImGui::SetNextWindowPos(ImVec2(0.66f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.9f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.

        ImGui::Begin(m_buttonSettingsID.c_str(), nullptr, m_noBackgroundNoFrame);

        m_buttonSettingsClicked = ImGui::ImageButton(m_buttonSettingsID.c_str(),reinterpret_cast<ImTextureID>(m_buttonSettingsTexture->getID()),
                                                     ImVec2(0.34f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.1f * Beryll::MainImGUI::getInstance()->getGUIHeight()));

        ImGui::End();

        // Button map swipe left.
        if(EnAndVars::MapsProgress::currentMapIndex > 0)
        {
            ImGui::SetNextWindowPos(ImVec2(0.05f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.3f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.

            ImGui::Begin(m_buttonMapSwipeLeftID.c_str(), nullptr, m_noBackgroundNoFrame);

            m_buttonMapSwipeLeftClicked = ImGui::ImageButton(m_buttonMapSwipeLeftID.c_str(),reinterpret_cast<ImTextureID>(m_buttonMapSwipeLeftTexture->getID()),
                                                             ImVec2(0.15f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.1f * Beryll::MainImGUI::getInstance()->getGUIHeight()));

            ImGui::End();
        }

        // Button map swipe right.
        if(EnAndVars::MapsProgress::currentMapIndex < EnAndVars::MapsProgress::lastOpenedMapIndex &&
           EnAndVars::MapsProgress::currentMapIndex < EnAndVars::MapsProgress::maxMapIndex)
        {
            ImGui::SetNextWindowPos(ImVec2(0.8f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.3f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.

            ImGui::Begin(m_buttonMapSwipeRightID.c_str(), nullptr, m_noBackgroundNoFrame);

            m_buttonMapSwipeRightClicked = ImGui::ImageButton(m_buttonMapSwipeRightID.c_str(),reinterpret_cast<ImTextureID>(m_buttonMapSwipeRightTexture->getID()),
                                                              ImVec2(0.15f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.1f * Beryll::MainImGUI::getInstance()->getGUIHeight()));

            ImGui::End();
        }

        // Map texture(preview).
        if(EnAndVars::MapsProgress::currentMapIndex < m_allMapsTextures.size())
        {
            ImGui::SetNextWindowPos(ImVec2(0.2f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.1f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.

            ImGui::Begin(m_mapTextureID.c_str(), nullptr, m_noBackgroundNoFrame);

            ImGui::Image(reinterpret_cast<ImTextureID>(m_allMapsTextures[EnAndVars::MapsProgress::currentMapIndex]->getID()),
                         ImVec2(0.6f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.55f * Beryll::MainImGUI::getInstance()->getGUIHeight()));

            ImGui::End();
        }

        // Text crystals.
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 0.0f, 0.0f, 0.0f, 1.0f });
        ImGui::SetNextWindowPos(ImVec2(0.6f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.0f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));

        ImGui::Begin(m_textCrystalsID.c_str(), nullptr, m_noBackgroundNoFrame);

        ImGui::PushFont(m_fontCrystals);
        ImGui::Text("Crystals: %d", EnAndVars::CurrencyBalance::crystals);
        ImGui::PopFont();

        ImGui::End();
        ImGui::PopStyleColor(1);
    }
}
