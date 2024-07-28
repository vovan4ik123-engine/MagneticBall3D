#include "ShopGUILayer.h"
#include "EnumsAndVariables.h"
#include "GameStateHelper.h"
#include "DataBaseHelper.h"

namespace MagneticBall3D
{
    // All there IDs as strings required by ImGUI.
    const std::string ShopGUILayer::m_backButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string ShopGUILayer::m_shopHeaderID = std::to_string(BeryllUtils::Common::generateID());
    const std::string ShopGUILayer::m_allItemsMenuID = std::to_string(BeryllUtils::Common::generateID());
    const std::string ShopGUILayer::m_item1ButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string ShopGUILayer::m_item2ButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string ShopGUILayer::m_item3ButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string ShopGUILayer::m_item4ButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string ShopGUILayer::m_item5ButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string ShopGUILayer::m_item6ButtonID = std::to_string(BeryllUtils::Common::generateID());
    const std::string ShopGUILayer::m_errorMenuID = std::to_string(BeryllUtils::Common::generateID());
    const std::string ShopGUILayer::m_errorButtonOkID = std::to_string(BeryllUtils::Common::generateID());

    std::atomic<bool> ShopGUILayer::m_item1Bought = false;
    std::atomic<bool> ShopGUILayer::m_item2Bought = false;
    std::atomic<bool> ShopGUILayer::m_item3Bought = false;
    std::atomic<bool> ShopGUILayer::m_item4Bought = false;
    std::atomic<bool> ShopGUILayer::m_item5Bought = false;
    std::atomic<bool> ShopGUILayer::m_item6Bought = false;
    std::atomic<bool> ShopGUILayer::m_buyError = false;

    ShopGUILayer::ShopGUILayer()
    {
        m_backButtonTexture = Beryll::Renderer::createTexture("GUI/menus/LeftArrow.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);

        m_shopHeaderTexture = Beryll::Renderer::createTexture("GUI/menus/shop/ShopHeader.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_item1ButtonTexture = Beryll::Renderer::createTexture("GUI/menus/shop/CrystalsItem1.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_item2ButtonTexture = Beryll::Renderer::createTexture("GUI/menus/shop/CrystalsItem2.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_item3ButtonTexture = Beryll::Renderer::createTexture("GUI/menus/shop/CrystalsItem3.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_item4ButtonTexture = Beryll::Renderer::createTexture("GUI/menus/shop/CrystalsItem4.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_item5ButtonTexture = Beryll::Renderer::createTexture("GUI/menus/shop/CrystalsItem5.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_item6ButtonTexture = Beryll::Renderer::createTexture("GUI/menus/shop/CrystalsItem6.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_errorTexture = Beryll::Renderer::createTexture("GUI/menus/shop/PurchaseError.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_errorButtonOkTexture = Beryll::Renderer::createTexture("GUI/Ok.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);

        // These callbacks can be called from different thread.
        m_buyItem1SuccessCallback = []() -> void
        {
            BR_INFO("%s", "testBillingLogs m_buyItem1SuccessCallback called");
            ShopGUILayer::m_item1Bought = true;
        };
        m_buyItem2SuccessCallback = []() -> void
        {
            BR_INFO("%s", "testBillingLogs m_buyItem2SuccessCallback called");
            ShopGUILayer::m_item2Bought = true;
        };
        m_buyItem3SuccessCallback = []() -> void
        {
            BR_INFO("%s", "testBillingLogs m_buyItem3SuccessCallback called");
            ShopGUILayer::m_item3Bought = true;
        };
        m_buyItem4SuccessCallback = []() -> void
        {
            BR_INFO("%s", "testBillingLogs m_buyItem4SuccessCallback called");
            ShopGUILayer::m_item4Bought = true;
        };
        m_buyItem5SuccessCallback = []() -> void
        {
            BR_INFO("%s", "testBillingLogs m_buyItem5SuccessCallback called");
            ShopGUILayer::m_item5Bought = true;
        };
        m_buyItem6SuccessCallback = []() -> void
        {
            BR_INFO("%s", "testBillingLogs m_buyItem6SuccessCallback called");
            ShopGUILayer::m_item6Bought = true;
        };

        m_commonErrorCallback = []() -> void
        {
            BR_INFO("%s", "testBillingLogs m_commonErrorCallback called");
            ShopGUILayer::m_buyError = true;
        };
    }

    ShopGUILayer::~ShopGUILayer()
    {

    }

    void ShopGUILayer::updateBeforePhysics()
    {
        if (m_backButtonClicked)
        {
            m_backButtonClicked = false;
            GameStateHelper::popState();
            return;
        }
        else if(m_item1ButtonClicked)
        {
            m_item1ButtonClicked = false;
            BR_INFO("%s", "Item 1 clicked.");
            Beryll::BillingSystem::getInstance()->makeInAppPurchaseGooglePlay("purchase_0_99_dollar",
                                                                              m_buyItem1SuccessCallback,
                                                                              m_commonErrorCallback);
        }
        else if(m_item2ButtonClicked)
        {
            m_item2ButtonClicked = false;
            BR_INFO("%s", "Item 2 clicked.");
            Beryll::BillingSystem::getInstance()->makeInAppPurchaseGooglePlay("purchase_2_5_dollar",
                                                                              m_buyItem2SuccessCallback,
                                                                              m_commonErrorCallback);
        }
        else if(m_item3ButtonClicked)
        {
            m_item3ButtonClicked = false;
            BR_INFO("%s", "Item 3 clicked.");
            Beryll::BillingSystem::getInstance()->makeInAppPurchaseGooglePlay("purchase_5_99_dollar",
                                                                              m_buyItem3SuccessCallback,
                                                                              m_commonErrorCallback);
        }
        else if(m_item4ButtonClicked)
        {
            m_item4ButtonClicked = false;
            BR_INFO("%s", "Item 4 clicked.");
            Beryll::BillingSystem::getInstance()->makeInAppPurchaseGooglePlay("purchase_18_dollar",
                                                                              m_buyItem4SuccessCallback,
                                                                              m_commonErrorCallback);
        }
        else if(m_item5ButtonClicked)
        {
            m_item5ButtonClicked = false;
            BR_INFO("%s", "Item 5 clicked.");
            Beryll::BillingSystem::getInstance()->makeInAppPurchaseGooglePlay("purchase_49_dollar",
                                                                              m_buyItem5SuccessCallback,
                                                                              m_commonErrorCallback);
        }
        else if(m_item6ButtonClicked)
        {
            m_item6ButtonClicked = false;
            BR_INFO("%s", "Item 6 clicked.");
            Beryll::BillingSystem::getInstance()->makeInAppPurchaseGooglePlay("purchase_99_dollar",
                                                                              m_buyItem6SuccessCallback,
                                                                              m_commonErrorCallback);
        }

        if(ShopGUILayer::m_item1Bought)
        {
            ShopGUILayer::m_item1Bought = false;
            BR_INFO("%s", "Item 1 bought.");

            int addCrystals = 60;
            if(EnumsAndVars::Shop::item1FirstBuy)
            {
                EnumsAndVars::Shop::item1FirstBuy = false;
                BR_INFO("%s", "Item 1 first buy.");
                DataBaseHelper::storeShopItem1FirstBuy(0);

                addCrystals *= 2;
            }

            EnumsAndVars::CurrencyBalance::crystals += addCrystals;
            DataBaseHelper::storeCurrencyBalanceCrystals(EnumsAndVars::CurrencyBalance::crystals);
        }
        else if(ShopGUILayer::m_item2Bought)
        {
            ShopGUILayer::m_item2Bought = false;
            BR_INFO("%s", "Item 2 bought.");

            int addCrystals = 150;
            if(EnumsAndVars::Shop::item2FirstBuy)
            {
                EnumsAndVars::Shop::item2FirstBuy = false;
                BR_INFO("%s", "Item 2 first buy.");
                DataBaseHelper::storeShopItem2FirstBuy(0);

                addCrystals *= 2;
            }

            EnumsAndVars::CurrencyBalance::crystals += addCrystals;
            DataBaseHelper::storeCurrencyBalanceCrystals(EnumsAndVars::CurrencyBalance::crystals);
        }
        else if(ShopGUILayer::m_item3Bought)
        {
            ShopGUILayer::m_item3Bought = false;
            BR_INFO("%s", "Item 3 bought.");

            int addCrystals = 400;
            if(EnumsAndVars::Shop::item3FirstBuy)
            {
                EnumsAndVars::Shop::item3FirstBuy = false;
                BR_INFO("%s", "Item 3 first buy.");
                DataBaseHelper::storeShopItem3FirstBuy(0);

                addCrystals *= 2;
            }

            EnumsAndVars::CurrencyBalance::crystals += addCrystals;
            DataBaseHelper::storeCurrencyBalanceCrystals(EnumsAndVars::CurrencyBalance::crystals);
        }
        else if(ShopGUILayer::m_item4Bought)
        {
            ShopGUILayer::m_item4Bought = false;
            BR_INFO("%s", "Item 4 bought.");

            int addCrystals = 1200;
            if(EnumsAndVars::Shop::item4FirstBuy)
            {
                EnumsAndVars::Shop::item4FirstBuy = false;
                BR_INFO("%s", "Item 4 first buy.");
                DataBaseHelper::storeShopItem4FirstBuy(0);

                addCrystals *= 2;
            }

            EnumsAndVars::CurrencyBalance::crystals += addCrystals;
            DataBaseHelper::storeCurrencyBalanceCrystals(EnumsAndVars::CurrencyBalance::crystals);
        }
        else if(ShopGUILayer::m_item5Bought)
        {
            ShopGUILayer::m_item5Bought = false;
            BR_INFO("%s", "Item 5 bought.");

            int addCrystals = 3500;
            if(EnumsAndVars::Shop::item5FirstBuy)
            {
                EnumsAndVars::Shop::item5FirstBuy = false;
                BR_INFO("%s", "Item 5 first buy.");
                DataBaseHelper::storeShopItem5FirstBuy(0);

                addCrystals *= 2;
            }

            EnumsAndVars::CurrencyBalance::crystals += addCrystals;
            DataBaseHelper::storeCurrencyBalanceCrystals(EnumsAndVars::CurrencyBalance::crystals);
        }
        else if(ShopGUILayer::m_item6Bought)
        {
            ShopGUILayer::m_item6Bought = false;
            BR_INFO("%s", "Item 6 bought.");

            int addCrystals = 7500;
            if(EnumsAndVars::Shop::item6FirstBuy)
            {
                EnumsAndVars::Shop::item6FirstBuy = false;
                BR_INFO("%s", "Item 6 first buy.");
                DataBaseHelper::storeShopItem6FirstBuy(0);

                addCrystals *= 2;
            }

            EnumsAndVars::CurrencyBalance::crystals += addCrystals;
            DataBaseHelper::storeCurrencyBalanceCrystals(EnumsAndVars::CurrencyBalance::crystals);
        }
        else if(ShopGUILayer::m_buyError)
        {
            ShopGUILayer::m_buyError = false;
            BR_INFO("%s", "Buy error.");
            m_showErrorMenu = true;
        }

        if(m_errorButtonOkClicked)
        {
            m_errorButtonOkClicked = false;
            m_showErrorMenu = false;
        }
    }

    void ShopGUILayer::updateAfterPhysics()
    {

    }

    void ShopGUILayer::draw()
    {
        // Back.
        ImGui::SetNextWindowPos(ImVec2(-0.01f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.9f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.

        ImGui::Begin(m_backButtonID.c_str(), nullptr, m_noBackgroundNoFrameNoFocus);
        m_backButtonClicked = ImGui::ImageButton(m_backButtonID.c_str(), reinterpret_cast<ImTextureID>(m_backButtonTexture->getID()),
                                                 ImVec2(0.34f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.105f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::End();

        // Shop header.
        ImGui::SetNextWindowPos(ImVec2(0.3f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.0f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.

        ImGui::Begin(m_shopHeaderID.c_str(), nullptr, m_noBackgroundNoFrameNoFocus);
        ImGui::Image(reinterpret_cast<ImTextureID>(m_shopHeaderTexture->getID()),
                     ImVec2(0.4f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.06f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::End();

        // Shop all items.
        ImGui::SetNextWindowPos(ImVec2(0.0f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.1f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(1.0f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.8f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::Begin(m_allItemsMenuID.c_str(), nullptr, m_noBackgroundNoFrameNoFocus);

        // Crystals item1.
        ImGui::SetCursorPos(ImVec2(0.02f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.05f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        m_item1ButtonClicked = ImGui::ImageButton(m_item1ButtonID.c_str(), reinterpret_cast<ImTextureID>(m_item1ButtonTexture->getID()),
                                                  ImVec2(0.3f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.2f * Beryll::MainImGUI::getInstance()->getGUIHeight()));

        // Crystals item2.
        ImGui::SetCursorPos(ImVec2(0.35f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.05f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        m_item2ButtonClicked = ImGui::ImageButton(m_item2ButtonID.c_str(), reinterpret_cast<ImTextureID>(m_item2ButtonTexture->getID()),
                                                  ImVec2(0.3f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.2f * Beryll::MainImGUI::getInstance()->getGUIHeight()));

        // Crystals item3.
        ImGui::SetCursorPos(ImVec2(0.68f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.05f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        m_item3ButtonClicked = ImGui::ImageButton(m_item3ButtonID.c_str(), reinterpret_cast<ImTextureID>(m_item3ButtonTexture->getID()),
                                                  ImVec2(0.3f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.2f * Beryll::MainImGUI::getInstance()->getGUIHeight()));

        // Crystals item4.
        ImGui::SetCursorPos(ImVec2(0.02f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.275f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        m_item4ButtonClicked = ImGui::ImageButton(m_item4ButtonID.c_str(), reinterpret_cast<ImTextureID>(m_item4ButtonTexture->getID()),
                                                  ImVec2(0.3f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.2f * Beryll::MainImGUI::getInstance()->getGUIHeight()));

        // Crystals item5.
        ImGui::SetCursorPos(ImVec2(0.35f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.275f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        m_item5ButtonClicked = ImGui::ImageButton(m_item5ButtonID.c_str(), reinterpret_cast<ImTextureID>(m_item5ButtonTexture->getID()),
                                                  ImVec2(0.3f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.2f * Beryll::MainImGUI::getInstance()->getGUIHeight()));

        // Crystals item6.
        ImGui::SetCursorPos(ImVec2(0.68f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.275f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        m_item6ButtonClicked = ImGui::ImageButton(m_item6ButtonID.c_str(), reinterpret_cast<ImTextureID>(m_item6ButtonTexture->getID()),
                                                  ImVec2(0.3f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.2f * Beryll::MainImGUI::getInstance()->getGUIHeight()));

        ImGui::End();

        if(m_showErrorMenu)
        {
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{ 0.0f, 0.0f, 0.0f, 0.92f });
            ImGui::SetNextWindowFocus();
            ImGui::SetNextWindowPos(ImVec2(0.0f * Beryll::MainImGUI::getInstance()->getGUIWidth(), -0.01f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::SetNextWindowSize(ImVec2(1.0f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 1.02f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::Begin(m_errorMenuID.c_str(), nullptr, m_noFrame);

            ImGui::SetCursorPos(ImVec2(0.2f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.26f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::Image(reinterpret_cast<ImTextureID>(m_errorTexture->getID()),
                         ImVec2(0.6f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.25f * Beryll::MainImGUI::getInstance()->getGUIHeight()));

            ImGui::SetCursorPos(ImVec2(0.35f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.52f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            m_errorButtonOkClicked = ImGui::ImageButton(m_errorButtonOkID.c_str(),reinterpret_cast<ImTextureID>(m_errorButtonOkTexture->getID()),
                                                          ImVec2(0.3f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.07f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
            ImGui::End();
            ImGui::PopStyleColor(1);
        }
    }
}
