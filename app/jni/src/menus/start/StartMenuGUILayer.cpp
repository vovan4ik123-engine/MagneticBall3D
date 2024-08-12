#include "StartMenuGUILayer.h"
#include "EnumsAndVariables.h"
#include "GameStateHelper.h"
#include "DataBaseHelper.h"
#include "energySystem/EnergySystem.h"

namespace MagneticBall3D
{
    // All there IDs as strings required by ImGUI.
    const std::string StartMenuGUILayer::m_mainMenuID = std::to_string(BeryllUtils::Common::generateID());
    const std::string StartMenuGUILayer::m_playButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string StartMenuGUILayer::m_shopButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string StartMenuGUILayer::m_talentsButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string StartMenuGUILayer::m_settingsButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string StartMenuGUILayer::m_mapSwipeLeftButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string StartMenuGUILayer::m_mapSwipeRightButtonID = std::to_string(BeryllUtils::Common::generateID());

    StartMenuGUILayer::StartMenuGUILayer()
    {
        m_ID = Beryll::LayerID::START_SCREEN_GUI;

        m_allMapsPreviewsTextures.push_back(Beryll::Renderer::createTexture("loadingScreen/Map0Screen.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1));
        m_allMapsPreviewsTextures.push_back(Beryll::Renderer::createTexture("loadingScreen/Map3Screen.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1));
        m_allMapsPreviewsTextures.push_back(Beryll::Renderer::createTexture("loadingScreen/Map2Screen.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1));
        m_allMapsPreviewsTextures.push_back(Beryll::Renderer::createTexture("loadingScreen/Map1Screen.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1));

        m_crystalIconTexture = Beryll::Renderer::createTexture("GUI/CrystalIcon.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
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

        const float GUIWidth = Beryll::MainImGUI::getInstance()->getGUIWidth();
        const float GUIHeight = Beryll::MainImGUI::getInstance()->getGUIHeight();

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{ 0.75f, 0.75f, 0.75f, 1.0f });
        ImGui::SetNextWindowPos(ImVec2(-0.01f * GUIWidth, -0.01f * GUIHeight));
        // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.
        ImGui::SetNextWindowSize(ImVec2(1.02f * GUIWidth, 1.02f * GUIHeight));

        ImGui::Begin(m_mainMenuID.c_str(), nullptr, m_noFrameNoFocus);

        // Crystals.
        ImGui::SetCursorPos(ImVec2(0.79f * GUIWidth, 0.01f * GUIHeight));
        ImGui::Image(reinterpret_cast<ImTextureID>(m_crystalIconTexture->getID()),
                     ImVec2(0.06f * GUIWidth, 0.06f * GUIWidth));

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 0.0f, 0.0f, 0.0f, 1.0f });
        ImGui::PushFont(m_crystalsFont);
        ImGui::SetCursorPos(ImVec2(0.855f * GUIWidth, 0.013f * GUIHeight));
        ImGui::Text("%d", EnumsAndVars::CurrencyBalance::crystals);
        ImGui::PopFont();
        ImGui::PopStyleColor(1);

        // Play.
        ImGui::SetCursorPos(ImVec2(0.31f * GUIWidth, 0.735f * GUIHeight));
        m_playButtonClicked = ImGui::ImageButton(m_playButtonID.c_str(), reinterpret_cast<ImTextureID>(m_playButtonTexture->getID()),
                                                 ImVec2(0.4f * GUIWidth, 0.09f * GUIHeight));

        // Shop.
        ImGui::SetCursorPos(ImVec2(0.0f * GUIWidth, 0.91f * GUIHeight));
        m_shopButtonClicked = ImGui::ImageButton(m_shopButtonID.c_str(), reinterpret_cast<ImTextureID>(m_shopButtonTexture->getID()),
                                                 ImVec2(0.34f * GUIWidth, 0.105f * GUIHeight));

        // Talents.
        ImGui::SetCursorPos(ImVec2(0.34f * GUIWidth, 0.91f * GUIHeight));
        m_talentsButtonClicked = ImGui::ImageButton(m_talentsButtonID.c_str(), reinterpret_cast<ImTextureID>(m_talentsButtonTexture->getID()),
                                                    ImVec2(0.34f * GUIWidth, 0.105f * GUIHeight));

        // Settings.
        ImGui::SetCursorPos(ImVec2(0.68f * GUIWidth, 0.91f * GUIHeight));
        m_settingsButtonClicked = ImGui::ImageButton(m_settingsButtonID.c_str(), reinterpret_cast<ImTextureID>(m_settingsButtonTexture->getID()),
                                                     ImVec2(0.34f * GUIWidth, 0.105f * GUIHeight));

        // Map swipe left.
        if(EnumsAndVars::MapsProgress::currentMapIndex > 0)
        {
            ImGui::SetCursorPos(ImVec2(0.06f * GUIWidth, 0.335f * GUIHeight));
            m_mapSwipeLeftButtonClicked = ImGui::ImageButton(m_mapSwipeLeftButtonID.c_str(), reinterpret_cast<ImTextureID>(m_mapSwipeLeftButtonTexture->getID()),
                                                             ImVec2(0.15f * GUIWidth, 0.1f * GUIHeight));
        }

        // Map preview.
        if(EnumsAndVars::MapsProgress::currentMapIndex < m_allMapsPreviewsTextures.size())
        {
            ImGui::SetCursorPos(ImVec2(0.21f * GUIWidth, 0.11f * GUIHeight));
            ImGui::Image(reinterpret_cast<ImTextureID>(m_allMapsPreviewsTextures[EnumsAndVars::MapsProgress::currentMapIndex]->getID()),
                         ImVec2(0.6f * GUIWidth, 0.55f * GUIHeight));
        }

        // Map swipe right.
        if(EnumsAndVars::MapsProgress::currentMapIndex < EnumsAndVars::MapsProgress::lastOpenedMapIndex &&
           EnumsAndVars::MapsProgress::currentMapIndex < EnumsAndVars::MapsProgress::maxMapIndex)
        {
            ImGui::SetCursorPos(ImVec2(0.81f * GUIWidth, 0.335f * GUIHeight));
            m_mapSwipeRightButtonClicked = ImGui::ImageButton(m_mapSwipeRightButtonID.c_str(), reinterpret_cast<ImTextureID>(m_mapSwipeRightButtonTexture->getID()),
                                                              ImVec2(0.15f * GUIWidth, 0.1f * GUIHeight));
        }

        ImGui::End();
        ImGui::PopStyleColor(1);
    }
}
