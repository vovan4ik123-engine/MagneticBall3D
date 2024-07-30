#include "StartMenuGUILayer.h"
#include "EnumsAndVariables.h"
#include "GameStateHelper.h"
#include "DataBaseHelper.h"
#include "energySystem/EnergySystem.h"

namespace MagneticBall3D
{
    // All there IDs as strings required by ImGUI.
    const std::string StartMenuGUILayer::m_playButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string StartMenuGUILayer::m_shopButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string StartMenuGUILayer::m_talentsButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string StartMenuGUILayer::m_settingsButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string StartMenuGUILayer::m_mapSwipeLeftButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string StartMenuGUILayer::m_mapSwipeRightButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string StartMenuGUILayer::m_mapPreviewTextureID = std::to_string(BeryllUtils::Common::generateID());
    const std::string StartMenuGUILayer::m_crystalsTextID = std::to_string(BeryllUtils::Common::generateID());

    StartMenuGUILayer::StartMenuGUILayer()
    {
        m_ID = Beryll::LayerID::START_SCREEN_GUI;

        m_allMapsPreviewsTextures.push_back(Beryll::Renderer::createTexture("loadingScreen/Map0Screen.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1));
        m_allMapsPreviewsTextures.push_back(Beryll::Renderer::createTexture("loadingScreen/Map1Screen.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1));
        m_allMapsPreviewsTextures.push_back(Beryll::Renderer::createTexture("loadingScreen/Map2Screen.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1));
        m_allMapsPreviewsTextures.push_back(Beryll::Renderer::createTexture("loadingScreen/Map3Screen.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1));

        m_mapSwipeLeftButtonTexture = Beryll::Renderer::createTexture("GUI/menus/start/MapSwipeLeft.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_mapSwipeRightButtonTexture = Beryll::Renderer::createTexture("GUI/menus/start/MapSwipeRight.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_playButtonTexture = Beryll::Renderer::createTexture("GUI/menus/start/Play.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_shopButtonTexture = Beryll::Renderer::createTexture("GUI/menus/start/Shop.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_talentsButtonTexture = Beryll::Renderer::createTexture("GUI/menus/start/PlayerTalents.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_settingsButtonTexture = Beryll::Renderer::createTexture("GUI/menus/start/Settings.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_crystalsFont = Beryll::MainImGUI::getInstance()->createFont(EnumsAndVars::FontsPath::roboto, 0.025f);
    }

    StartMenuGUILayer::~StartMenuGUILayer()
    {

    }

    void StartMenuGUILayer::updateBeforePhysics()
    {
        //EnergySystem::getInstance().update();

        if(m_playButtonClicked)
        {
            m_playButtonClicked = false;

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
        else if(m_shopButtonClicked)
        {
            m_shopButtonClicked = false;
            GameStateHelper::pushShopState();
        }
        else if(m_talentsButtonClicked)
        {
            m_talentsButtonClicked = false;
            GameStateHelper::pushPlayerTalentsState();
        }
        else if(m_settingsButtonClicked)
        {
            m_settingsButtonClicked = false;
            GameStateHelper::pushSettingsState();
        }
        else if(m_mapSwipeLeftButtonClicked && EnumsAndVars::MapsProgress::currentMapIndex > 0)
        {
            m_mapSwipeLeftButtonClicked = false;
            --EnumsAndVars::MapsProgress::currentMapIndex;
            BR_INFO("currentMapIndex: %d", EnumsAndVars::MapsProgress::currentMapIndex);
            DataBaseHelper::storeMapsProgressCurrentMapIndex(EnumsAndVars::MapsProgress::currentMapIndex);
        }
        else if(m_mapSwipeRightButtonClicked &&
                EnumsAndVars::MapsProgress::currentMapIndex < EnumsAndVars::MapsProgress::lastOpenedMapIndex &&
                EnumsAndVars::MapsProgress::currentMapIndex < EnumsAndVars::MapsProgress::maxMapIndex)
        {
            m_mapSwipeRightButtonClicked = false;
            ++EnumsAndVars::MapsProgress::currentMapIndex;
            BR_INFO("currentMapIndex: %d", EnumsAndVars::MapsProgress::currentMapIndex);
            DataBaseHelper::storeMapsProgressCurrentMapIndex(EnumsAndVars::MapsProgress::currentMapIndex);
        }
    }

    void StartMenuGUILayer::updateAfterPhysics()
    {
        // Dont need update.
    }

    void StartMenuGUILayer::draw()
    {
        //EnergySystem::getInstance().draw();

        // Play.
        ImGui::SetNextWindowPos(ImVec2(0.3f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.7f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.

        ImGui::Begin(m_playButtonID.c_str(), nullptr, m_noBackgroundNoFrame);

        m_playButtonClicked = ImGui::ImageButton(m_playButtonID.c_str(), reinterpret_cast<ImTextureID>(m_playButtonTexture->getID()),
                                                 ImVec2(0.4f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.09f * Beryll::MainImGUI::getInstance()->getGUIHeight()));

        ImGui::End();

        // Shop.
        ImGui::SetNextWindowPos(ImVec2(-0.01f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.9f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.

        ImGui::Begin(m_shopButtonID.c_str(), nullptr, m_noBackgroundNoFrame);

        m_shopButtonClicked = ImGui::ImageButton(m_shopButtonID.c_str(), reinterpret_cast<ImTextureID>(m_shopButtonTexture->getID()),
                                                 ImVec2(0.34f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.105f * Beryll::MainImGUI::getInstance()->getGUIHeight()));

        ImGui::End();

        // Talents.
        ImGui::SetNextWindowPos(ImVec2(0.33f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.9f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.

        ImGui::Begin(m_talentsButtonID.c_str(), nullptr, m_noBackgroundNoFrame);

        m_talentsButtonClicked = ImGui::ImageButton(m_talentsButtonID.c_str(), reinterpret_cast<ImTextureID>(m_talentsButtonTexture->getID()),
                                                    ImVec2(0.33f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.105f * Beryll::MainImGUI::getInstance()->getGUIHeight()));

        ImGui::End();

        // Settings.
        ImGui::SetNextWindowPos(ImVec2(0.66f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.9f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.

        ImGui::Begin(m_settingsButtonID.c_str(), nullptr, m_noBackgroundNoFrame);

        m_settingsButtonClicked = ImGui::ImageButton(m_settingsButtonID.c_str(), reinterpret_cast<ImTextureID>(m_settingsButtonTexture->getID()),
                                                     ImVec2(0.34f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.105f * Beryll::MainImGUI::getInstance()->getGUIHeight()));

        ImGui::End();

        // Map swipe left.
        if(EnumsAndVars::MapsProgress::currentMapIndex > 0)
        {
            ImGui::SetNextWindowPos(ImVec2(0.05f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.3f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.

            ImGui::Begin(m_mapSwipeLeftButtonID.c_str(), nullptr, m_noBackgroundNoFrame);

            m_mapSwipeLeftButtonClicked = ImGui::ImageButton(m_mapSwipeLeftButtonID.c_str(), reinterpret_cast<ImTextureID>(m_mapSwipeLeftButtonTexture->getID()),
                                                             ImVec2(0.15f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.1f * Beryll::MainImGUI::getInstance()->getGUIHeight()));

            ImGui::End();
        }

        // Map swipe right.
        if(EnumsAndVars::MapsProgress::currentMapIndex < EnumsAndVars::MapsProgress::lastOpenedMapIndex &&
           EnumsAndVars::MapsProgress::currentMapIndex < EnumsAndVars::MapsProgress::maxMapIndex)
        {
            ImGui::SetNextWindowPos(ImVec2(0.8f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.3f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.

            ImGui::Begin(m_mapSwipeRightButtonID.c_str(), nullptr, m_noBackgroundNoFrame);

            m_mapSwipeRightButtonClicked = ImGui::ImageButton(m_mapSwipeRightButtonID.c_str(), reinterpret_cast<ImTextureID>(m_mapSwipeRightButtonTexture->getID()),
                                                              ImVec2(0.15f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.1f * Beryll::MainImGUI::getInstance()->getGUIHeight()));

            ImGui::End();
        }

        // Map preview.
        if(EnumsAndVars::MapsProgress::currentMapIndex < m_allMapsPreviewsTextures.size())
        {
            ImGui::SetNextWindowPos(ImVec2(0.2f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.1f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.

            ImGui::Begin(m_mapPreviewTextureID.c_str(), nullptr, m_noBackgroundNoFrame);

            ImGui::Image(reinterpret_cast<ImTextureID>(m_allMapsPreviewsTextures[EnumsAndVars::MapsProgress::currentMapIndex]->getID()),
                         ImVec2(0.6f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.55f * Beryll::MainImGUI::getInstance()->getGUIHeight()));

            ImGui::End();
        }

        // Crystals.
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 0.0f, 0.0f, 0.0f, 1.0f });
        ImGui::SetNextWindowPos(ImVec2(0.7f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.0f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));

        ImGui::Begin(m_crystalsTextID.c_str(), nullptr, m_noBackgroundNoFrame);

        ImGui::PushFont(m_crystalsFont);
        ImGui::Text("Crystals: %d", EnumsAndVars::CurrencyBalance::crystals);
        ImGui::PopFont();

        ImGui::End();
        ImGui::PopStyleColor(1);
    }
}
