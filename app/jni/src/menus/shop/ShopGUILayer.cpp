#include "ShopGUILayer.h"
#include "EnumsAndVariables.h"
#include "GameStateHelper.h"
#include "DataBaseHelper.h"

namespace MagneticBall3D
{
    std::atomic<bool> ShopGUILayer::m_item1Bought = false;
    std::atomic<bool> ShopGUILayer::m_item2Bought = false;
    std::atomic<bool> ShopGUILayer::m_item3Bought = false;
    std::atomic<bool> ShopGUILayer::m_item4Bought = false;
    std::atomic<bool> ShopGUILayer::m_item5Bought = false;
    std::atomic<bool> ShopGUILayer::m_item6Bought = false;
    std::atomic<bool> ShopGUILayer::m_disableAdsOnMapsBought = false;
    std::atomic<bool> ShopGUILayer::m_buyError = false;

    ShopGUILayer::ShopGUILayer()
    {
        m_backButtonTexture = Beryll::Renderer::createTexture("GUI/menus/LeftArrow.png", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);

        m_backgroundTexture = Beryll::Renderer::createTexture("GUI/menus/shop/ShopBackground.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_item1ButtonTexture = Beryll::Renderer::createTexture("GUI/menus/shop/CrystalsItem1.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_item1FirstBuyButtonTexture = Beryll::Renderer::createTexture("GUI/menus/shop/CrystalsItem1FirstBuy.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_item2ButtonTexture = Beryll::Renderer::createTexture("GUI/menus/shop/CrystalsItem2.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_item2FirstBuyButtonTexture = Beryll::Renderer::createTexture("GUI/menus/shop/CrystalsItem2FirstBuy.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_item3ButtonTexture = Beryll::Renderer::createTexture("GUI/menus/shop/CrystalsItem3.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_item3FirstBuyButtonTexture = Beryll::Renderer::createTexture("GUI/menus/shop/CrystalsItem3FirstBuy.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_item4ButtonTexture = Beryll::Renderer::createTexture("GUI/menus/shop/CrystalsItem4.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_item4FirstBuyButtonTexture = Beryll::Renderer::createTexture("GUI/menus/shop/CrystalsItem4FirstBuy.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_item5ButtonTexture = Beryll::Renderer::createTexture("GUI/menus/shop/CrystalsItem5.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_item5FirstBuyButtonTexture = Beryll::Renderer::createTexture("GUI/menus/shop/CrystalsItem5FirstBuy.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_item6ButtonTexture = Beryll::Renderer::createTexture("GUI/menus/shop/CrystalsItem6.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_item6FirstBuyButtonTexture = Beryll::Renderer::createTexture("GUI/menus/shop/CrystalsItem6FirstBuy.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_disableAdsOnMapsButtonTexture = Beryll::Renderer::createTexture("GUI/menus/shop/DisableAdsOnMaps.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_errorTexture = Beryll::Renderer::createTexture("GUI/menus/shop/PurchaseError.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_errorButtonOkTexture = Beryll::Renderer::createTexture("GUI/Ok.png", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);

        // These callbacks are called from different thread.
        m_buyItem1SuccessCallback = []() -> void { BR_INFO("%s", "m_buyItem1SuccessCallback()"); ShopGUILayer::m_item1Bought = true; };
        m_buyItem2SuccessCallback = []() -> void { BR_INFO("%s", "m_buyItem2SuccessCallback()"); ShopGUILayer::m_item2Bought = true; };
        m_buyItem3SuccessCallback = []() -> void { BR_INFO("%s", "m_buyItem3SuccessCallback()"); ShopGUILayer::m_item3Bought = true; };
        m_buyItem4SuccessCallback = []() -> void { BR_INFO("%s", "m_buyItem4SuccessCallback()"); ShopGUILayer::m_item4Bought = true; };
        m_buyItem5SuccessCallback = []() -> void { BR_INFO("%s", "m_buyItem5SuccessCallback()"); ShopGUILayer::m_item5Bought = true; };
        m_buyItem6SuccessCallback = []() -> void { BR_INFO("%s", "m_buyItem6SuccessCallback()"); ShopGUILayer::m_item6Bought = true; };
        m_buyDisableAdsOnMapsSuccessCallback = []() -> void { BR_INFO("%s", "m_buyDisableAdsOnMapsSuccessCallback()"); ShopGUILayer::m_disableAdsOnMapsBought = true; };
        m_commonErrorCallback = []() -> void { BR_INFO("%s", "m_commonErrorCallback()"); ShopGUILayer::m_buyError = true; };
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
        else if(m_disableAdsOnMapsButtonClicked)
        {
            m_disableAdsOnMapsButtonClicked = false;
            BR_INFO("%s", "Disable Ads On Maps clicked.");
            Beryll::BillingSystem::getInstance()->makeInAppPurchaseGooglePlay("purchase_8_99_dollar",
                                                                              m_buyDisableAdsOnMapsSuccessCallback,
                                                                              m_commonErrorCallback);
        }

        if(ShopGUILayer::m_item1Bought)
        {
            ShopGUILayer::m_item1Bought = false;
            BR_INFO("%s", "Item 1 bought.");

            int addCrystals = 80;
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

            int addCrystals = 200;
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

            int addCrystals = 500;
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
        else if(ShopGUILayer::m_disableAdsOnMapsBought)
        {
            ShopGUILayer::m_disableAdsOnMapsBought = false;
            BR_INFO("%s", "Disable Ads On Maps bought.");

            EnumsAndVars::Shop::adsOnMapsDisabled = true;
            DataBaseHelper::storeShopAdsOnMapsDisabled(1);

            EnumsAndVars::CurrencyBalance::crystals += 1000;
            DataBaseHelper::storeCurrencyBalanceCrystals(EnumsAndVars::CurrencyBalance::crystals);
        }
        else if(ShopGUILayer::m_buyError)
        {
            ShopGUILayer::m_buyError = false;
            BR_INFO("%s", "Buy error.");
            m_errorMenuShow = true;
        }

        if(m_errorButtonOkClicked)
        {
            m_errorButtonOkClicked = false;
            m_errorMenuShow = false;
        }
    }

    void ShopGUILayer::updateAfterPhysics()
    {

    }

    void ShopGUILayer::draw()
    {
        const float GUIWidth = Beryll::MainImGUI::getInstance()->getGUIWidth();
        const float GUIHeight = Beryll::MainImGUI::getInstance()->getGUIHeight();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}); // Lost focus.
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}); // On focus.
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}); // Clicked.
        ImGui::SetNextWindowPos(ImVec2(-0.005f * GUIWidth, -0.005f * GUIHeight));
        ImGui::SetNextWindowSize(ImVec2(1.01f * GUIWidth, 1.01f * GUIHeight));
        ImGui::Begin("shopMenu", nullptr, m_noBackgroundNoFrameNoFocus);

        // Background.
        ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
        ImGui::Image(static_cast<ImTextureID>(m_backgroundTexture->getID()),
                     ImVec2(1.01f * GUIWidth, 1.01f * GUIHeight));

        // Back.
        ImGui::SetCursorPos(ImVec2(0.005f * GUIWidth, 0.855f * GUIHeight));
        m_backButtonClicked = ImGui::ImageButton("backButton", static_cast<ImTextureID>(m_backButtonTexture->getID()),
                                                 ImVec2(0.15f * GUIWidth, 0.15f * GUIHeight));

        // Crystals item1.
        ImGui::SetCursorPos(ImVec2(0.028f * GUIWidth, 0.245f * GUIHeight));
        if(EnumsAndVars::Shop::item1FirstBuy)
            m_item1ButtonClicked = ImGui::ImageButton("item1Button", static_cast<ImTextureID>(m_item1FirstBuyButtonTexture->getID()),
                                                      ImVec2(0.14f * GUIWidth, 0.4f * GUIHeight));
        else
            m_item1ButtonClicked = ImGui::ImageButton("item1Button", static_cast<ImTextureID>(m_item1ButtonTexture->getID()),
                                                      ImVec2(0.14f * GUIWidth, 0.4f * GUIHeight));

        // Crystals item2.
        ImGui::SetCursorPos(ImVec2(0.191f * GUIWidth, 0.245f * GUIHeight));
        if(EnumsAndVars::Shop::item2FirstBuy)
            m_item2ButtonClicked = ImGui::ImageButton("item2Button", static_cast<ImTextureID>(m_item2FirstBuyButtonTexture->getID()),
                                                      ImVec2(0.14f * GUIWidth, 0.4f * GUIHeight));
        else
            m_item2ButtonClicked = ImGui::ImageButton("item2Button", static_cast<ImTextureID>(m_item2ButtonTexture->getID()),
                                                      ImVec2(0.14f * GUIWidth, 0.4f * GUIHeight));

        // Crystals item3.
        ImGui::SetCursorPos(ImVec2(0.354f * GUIWidth, 0.245f * GUIHeight));
        if(EnumsAndVars::Shop::item3FirstBuy)
            m_item3ButtonClicked = ImGui::ImageButton("item3Button", static_cast<ImTextureID>(m_item3FirstBuyButtonTexture->getID()),
                                                      ImVec2(0.14f * GUIWidth, 0.4f * GUIHeight));
        else
            m_item3ButtonClicked = ImGui::ImageButton("item3Button", static_cast<ImTextureID>(m_item3ButtonTexture->getID()),
                                                      ImVec2(0.14f * GUIWidth, 0.4f * GUIHeight));

        // Crystals item4.
        ImGui::SetCursorPos(ImVec2(0.517f * GUIWidth, 0.245f * GUIHeight));
        if(EnumsAndVars::Shop::item4FirstBuy)
            m_item4ButtonClicked = ImGui::ImageButton("item4Button", static_cast<ImTextureID>(m_item4FirstBuyButtonTexture->getID()),
                                                      ImVec2(0.14f * GUIWidth, 0.4f * GUIHeight));
        else
            m_item4ButtonClicked = ImGui::ImageButton("item4Button", static_cast<ImTextureID>(m_item4ButtonTexture->getID()),
                                                      ImVec2(0.14f * GUIWidth, 0.4f * GUIHeight));

        // Crystals item5.
        ImGui::SetCursorPos(ImVec2(0.68f * GUIWidth, 0.245f * GUIHeight));
        if(EnumsAndVars::Shop::item5FirstBuy)
            m_item5ButtonClicked = ImGui::ImageButton("item5Button", static_cast<ImTextureID>(m_item5FirstBuyButtonTexture->getID()),
                                                      ImVec2(0.14f * GUIWidth, 0.4f * GUIHeight));
        else
            m_item5ButtonClicked = ImGui::ImageButton("item5Button", static_cast<ImTextureID>(m_item5ButtonTexture->getID()),
                                                      ImVec2(0.14f * GUIWidth, 0.4f * GUIHeight));

        // Crystals item6.
        ImGui::SetCursorPos(ImVec2(0.843f * GUIWidth, 0.245f * GUIHeight));
        if(EnumsAndVars::Shop::item6FirstBuy)
            m_item6ButtonClicked = ImGui::ImageButton("item6Button", static_cast<ImTextureID>(m_item6FirstBuyButtonTexture->getID()),
                                                      ImVec2(0.14f * GUIWidth, 0.4f * GUIHeight));
        else
            m_item6ButtonClicked = ImGui::ImageButton("item6Button", static_cast<ImTextureID>(m_item6ButtonTexture->getID()),
                                                      ImVec2(0.14f * GUIWidth, 0.4f * GUIHeight));

        // Disable ads on maps.
        if(!EnumsAndVars::Shop::adsOnMapsDisabled)
        {
            ImGui::SetCursorPos(ImVec2(0.356f * GUIWidth, 0.675f * GUIHeight));
            m_disableAdsOnMapsButtonClicked = ImGui::ImageButton("disableAdsOnMapsButton", static_cast<ImTextureID>(m_disableAdsOnMapsButtonTexture->getID()),
                                                                 ImVec2(0.3f * GUIWidth, 0.3f * GUIHeight));
        }
        ImGui::End();
        ImGui::PopStyleColor(3);

        if(m_errorMenuShow)
        {
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{0.0f, 0.0f, 0.0f, 0.92f});
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}); // Lost focus.
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}); // On focus.
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}); // Clicked.
            ImGui::SetNextWindowPos(ImVec2(-0.005f * GUIWidth, -0.005f * GUIHeight));
            ImGui::SetNextWindowSize(ImVec2(1.01f * GUIWidth, 1.01f * GUIHeight));
            ImGui::Begin("purchaseErrorMenu", nullptr, m_noFrame);

            ImGui::SetCursorPos(ImVec2(0.355f * GUIWidth, 0.155f * GUIHeight));
            ImGui::Image(static_cast<ImTextureID>(m_errorTexture->getID()),
                         ImVec2(0.3f * GUIWidth, 0.5f * GUIHeight));

            ImGui::SetCursorPos(ImVec2(0.435f * GUIWidth, 0.705f * GUIHeight));
            m_errorButtonOkClicked = ImGui::ImageButton("purchaseErrorButtonOk",static_cast<ImTextureID>(m_errorButtonOkTexture->getID()),
                                                        ImVec2(0.14f * GUIWidth, 0.1528f * GUIHeight));
            ImGui::End();
            ImGui::PopStyleColor(4);
        }
    }
}
