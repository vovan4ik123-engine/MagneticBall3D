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
        const float screenAR = Beryll::Window::getInstance()->getScreenAspectRatio();

        auto background = std::make_shared<Beryll::GUITexture>("GUI/menus/shop/ShopBackground.jpg",
                                                               glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec2{100.0f, 100.0f});
        m_guiObjects.push_back(background);
        m_buttonBack = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/LeftArrow.png", "",
                                                                   glm::vec3{3.0f, 5.0f, 0.1f}, glm::vec2{15.0f, 15.0f});
        m_guiObjects.push_back(m_buttonBack);
        // 1.
        m_buttonItem1FirstBuy = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/shop/CrystalsItem1FirstBuy.jpg", "",
                                                                            glm::vec3{3.0f, 36.0f, 0.1f}, glm::vec2{14.0f, 40.0f});
        m_guiObjects.push_back(m_buttonItem1FirstBuy);
        m_buttonItem1 = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/shop/CrystalsItem1.jpg", "",
                                                                    glm::vec3{3.0f, 36.0f, 0.1f}, glm::vec2{14.0f, 40.0f});
        m_guiObjects.push_back(m_buttonItem1);
        // 2.
        m_buttonItem2FirstBuy = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/shop/CrystalsItem2FirstBuy.jpg", "",
                                                                            glm::vec3{19.0f, 36.0f, 0.1f}, glm::vec2{14.0f, 40.0f});
        m_guiObjects.push_back(m_buttonItem2FirstBuy);
        m_buttonItem2 = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/shop/CrystalsItem2.jpg", "",
                                                                    glm::vec3{19.0f, 36.0f, 0.1f}, glm::vec2{14.0f, 40.0f});
        m_guiObjects.push_back(m_buttonItem2);
        // 3.
        m_buttonItem3FirstBuy = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/shop/CrystalsItem3FirstBuy.jpg", "",
                                                                            glm::vec3{35.0f, 36.0f, 0.1f}, glm::vec2{14.0f, 40.0f});
        m_guiObjects.push_back(m_buttonItem3FirstBuy);
        m_buttonItem3 = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/shop/CrystalsItem3.jpg", "",
                                                                    glm::vec3{35.0f, 36.0f, 0.1f}, glm::vec2{14.0f, 40.0f});
        m_guiObjects.push_back(m_buttonItem3);
        // 4.
        m_buttonItem4FirstBuy = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/shop/CrystalsItem4FirstBuy.jpg", "",
                                                                            glm::vec3{51.0f, 36.0f, 0.1f}, glm::vec2{14.0f, 40.0f});
        m_guiObjects.push_back(m_buttonItem4FirstBuy);
        m_buttonItem4 = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/shop/CrystalsItem4.jpg", "",
                                                                    glm::vec3{51.0f, 36.0f, 0.1f}, glm::vec2{14.0f, 40.0f});
        m_guiObjects.push_back(m_buttonItem4);
        // 5.
        m_buttonItem5FirstBuy = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/shop/CrystalsItem5FirstBuy.jpg", "",
                                                                            glm::vec3{67.0f, 36.0f, 0.1f}, glm::vec2{14.0f, 40.0f});
        m_guiObjects.push_back(m_buttonItem5FirstBuy);
        m_buttonItem5 = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/shop/CrystalsItem5.jpg", "",
                                                                    glm::vec3{67.0f, 36.0f, 0.1f}, glm::vec2{14.0f, 40.0f});
        m_guiObjects.push_back(m_buttonItem5);
        // 6.
        m_buttonItem6FirstBuy = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/shop/CrystalsItem6FirstBuy.jpg", "",
                                                                            glm::vec3{83.0f, 36.0f, 0.1f}, glm::vec2{14.0f, 40.0f});
        m_guiObjects.push_back(m_buttonItem6FirstBuy);
        m_buttonItem6 = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/shop/CrystalsItem6.jpg", "",
                                                                    glm::vec3{83.0f, 36.0f, 0.1f}, glm::vec2{14.0f, 40.0f});
        m_guiObjects.push_back(m_buttonItem6);
        // Ads on maps.
        m_buttonDisableAdsOnMaps = std::make_shared<Beryll::ButtonWithTexture>("GUI/menus/shop/DisableAdsOnMaps.jpg", "",
                                                                            glm::vec3{35.0f, 3.0f, 0.1f}, glm::vec2{30.0f, 30.0f});
        m_guiObjects.push_back(m_buttonDisableAdsOnMaps);

        enableDisableButtons();

        // Error menu.
        m_textureBackgroundError = std::make_shared<Beryll::GUITexture>("GUI/GrayTransparency10.png",
                                                                        glm::vec3{0.0f, 0.0f, 0.9f}, glm::vec2{100.0f, 100.0f});
        m_textureBackgroundError->disable();
        m_guiObjects.push_back(m_textureBackgroundError);
        m_textureError = std::make_shared<Beryll::GUITexture>("GUI/menus/shop/PurchaseError.jpg",
                                                              glm::vec3{35.0f, 35.0f, 1.0f}, glm::vec2{30.0f, 50.0f});
        m_textureError->disable();
        m_guiObjects.push_back(m_textureError);
        m_buttonCloseError = std::make_shared<Beryll::ButtonWithTexture>("GUI/Ok.png", "",
                                                                         glm::vec3{43.0f, 15.0f, 1.0f}, glm::vec2{14.0f, 15.0f});
        m_buttonCloseError->disable();
        m_guiObjects.push_back(m_buttonCloseError);

        // Sort to update nearest objects first. But draw should starts from farest object(in reverse order).
        std::sort(m_guiObjects.begin(), m_guiObjects.end(), [](std::shared_ptr<Beryll::GUIObject> o1, std::shared_ptr<Beryll::GUIObject> o2)
        {
            return (o1->getPositionNormalized().z > o2->getPositionNormalized().z);
        });

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
        else if(m_buttonItem1->getIsPressed() || m_buttonItem1FirstBuy->getIsPressed())
        {
            BR_INFO("%s", "Item 1 clicked.");
            Beryll::BillingSystem::getInstance()->makeInAppPurchaseGooglePlay("purchase_0_99_dollar",
                                                                              m_buyItem1SuccessCallback,
                                                                              m_commonErrorCallback);
        }
        else if(m_buttonItem2->getIsPressed() || m_buttonItem2FirstBuy->getIsPressed())
        {
            BR_INFO("%s", "Item 2 clicked.");
            Beryll::BillingSystem::getInstance()->makeInAppPurchaseGooglePlay("purchase_2_5_dollar",
                                                                              m_buyItem2SuccessCallback,
                                                                              m_commonErrorCallback);
        }
        else if(m_buttonItem3->getIsPressed() || m_buttonItem3FirstBuy->getIsPressed())
        {
            BR_INFO("%s", "Item 3 clicked.");
            Beryll::BillingSystem::getInstance()->makeInAppPurchaseGooglePlay("purchase_5_99_dollar",
                                                                              m_buyItem3SuccessCallback,
                                                                              m_commonErrorCallback);
        }
        else if(m_buttonItem4->getIsPressed() || m_buttonItem4FirstBuy->getIsPressed())
        {
            BR_INFO("%s", "Item 4 clicked.");
            Beryll::BillingSystem::getInstance()->makeInAppPurchaseGooglePlay("purchase_18_dollar",
                                                                              m_buyItem4SuccessCallback,
                                                                              m_commonErrorCallback);
        }
        else if(m_buttonItem5->getIsPressed() || m_buttonItem5FirstBuy->getIsPressed())
        {
            BR_INFO("%s", "Item 5 clicked.");
            Beryll::BillingSystem::getInstance()->makeInAppPurchaseGooglePlay("purchase_49_dollar",
                                                                              m_buyItem5SuccessCallback,
                                                                              m_commonErrorCallback);
        }
        else if(m_buttonItem6->getIsPressed() || m_buttonItem6FirstBuy->getIsPressed())
        {
            BR_INFO("%s", "Item 6 clicked.");
            Beryll::BillingSystem::getInstance()->makeInAppPurchaseGooglePlay("purchase_99_dollar",
                                                                              m_buyItem6SuccessCallback,
                                                                              m_commonErrorCallback);
        }
        else if(m_buttonDisableAdsOnMaps->getIsPressed())
        {
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

            enableDisableButtons();
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

            enableDisableButtons();
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

            enableDisableButtons();
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

            enableDisableButtons();
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

            enableDisableButtons();
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

            enableDisableButtons();
            EnumsAndVars::CurrencyBalance::crystals += addCrystals;
            DataBaseHelper::storeCurrencyBalanceCrystals(EnumsAndVars::CurrencyBalance::crystals);
        }
        else if(ShopGUILayer::m_disableAdsOnMapsBought)
        {
            ShopGUILayer::m_disableAdsOnMapsBought = false;
            BR_INFO("%s", "Disable Ads On Maps bought.");

            EnumsAndVars::Shop::adsOnMapsDisabled = true;
            DataBaseHelper::storeShopAdsOnMapsDisabled(1);

            enableDisableButtons();
            EnumsAndVars::CurrencyBalance::crystals += 1000;
            DataBaseHelper::storeCurrencyBalanceCrystals(EnumsAndVars::CurrencyBalance::crystals);
        }
        else if(ShopGUILayer::m_buyError)
        {
            ShopGUILayer::m_buyError = false;
            BR_INFO("%s", "Buy error.");
            m_textureBackgroundError->enable();
            m_textureError->enable();
            m_buttonCloseError->enable();
        }

        if(m_buttonCloseError->getIsPressed())
        {
            m_textureBackgroundError->disable();
            m_textureError->disable();
            m_buttonCloseError->disable();
        }
    }

    void ShopGUILayer::updateAfterPhysics()
    {

    }

    void ShopGUILayer::draw()
    {
        for(auto it = m_guiObjects.rbegin(); it != m_guiObjects.rend(); ++it)
        {
            if((*it)->getIsEnabled())
            {
                (*it)->draw();
            }
        }
    }

    void ShopGUILayer::enableDisableButtons()
    {
        if(EnumsAndVars::Shop::item1FirstBuy)
        {
            m_buttonItem1FirstBuy->enable();
            m_buttonItem1->disable();
        }
        else
        {
            m_buttonItem1FirstBuy->disable();
            m_buttonItem1->enable();
        }

        if(EnumsAndVars::Shop::item2FirstBuy)
        {
            m_buttonItem2FirstBuy->enable();
            m_buttonItem2->disable();
        }
        else
        {
            m_buttonItem2FirstBuy->disable();
            m_buttonItem2->enable();
        }

        if(EnumsAndVars::Shop::item3FirstBuy)
        {
            m_buttonItem3FirstBuy->enable();
            m_buttonItem3->disable();
        }
        else
        {
            m_buttonItem3FirstBuy->disable();
            m_buttonItem3->enable();
        }

        if(EnumsAndVars::Shop::item4FirstBuy)
        {
            m_buttonItem4FirstBuy->enable();
            m_buttonItem4->disable();
        }
        else
        {
            m_buttonItem4FirstBuy->disable();
            m_buttonItem4->enable();
        }

        if(EnumsAndVars::Shop::item5FirstBuy)
        {
            m_buttonItem5FirstBuy->enable();
            m_buttonItem5->disable();
        }
        else
        {
            m_buttonItem5FirstBuy->disable();
            m_buttonItem5->enable();
        }

        if(EnumsAndVars::Shop::item6FirstBuy)
        {
            m_buttonItem6FirstBuy->enable();
            m_buttonItem6->disable();
        }
        else
        {
            m_buttonItem6FirstBuy->disable();
            m_buttonItem6->enable();
        }

        if(EnumsAndVars::Shop::adsOnMapsDisabled)
        {
            m_buttonDisableAdsOnMaps->disable();
        }
    }
}
