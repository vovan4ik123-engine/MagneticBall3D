#include "ShopGUILayer.h"
#include "EnumsAndVariables.h"
#include "GameStateHelper.h"

namespace MagneticBall3D
{
    // All there IDs as strings required by ImGUI.
    const std::string ShopGUILayer::m_buttonBackID = std::to_string(BeryllUtils::Common::generateID());
    const std::string ShopGUILayer::m_shopHeaderID = std::to_string(BeryllUtils::Common::generateID());
    const std::string ShopGUILayer::m_buttonItem1ID = std::to_string(BeryllUtils::Common::generateID());
    const std::string ShopGUILayer::m_buttonItem2ID = std::to_string(BeryllUtils::Common::generateID());
    const std::string ShopGUILayer::m_buttonItem3ID = std::to_string(BeryllUtils::Common::generateID());
    const std::string ShopGUILayer::m_buttonItem4ID = std::to_string(BeryllUtils::Common::generateID());
    const std::string ShopGUILayer::m_buttonItem5ID = std::to_string(BeryllUtils::Common::generateID());
    const std::string ShopGUILayer::m_buttonItem6ID = std::to_string(BeryllUtils::Common::generateID());

    std::atomic<bool> ShopGUILayer::m_item1Bought = false;
    std::atomic<bool> ShopGUILayer::m_item2Bought = false;
    std::atomic<bool> ShopGUILayer::m_item3Bought = false;
    std::atomic<bool> ShopGUILayer::m_item4Bought = false;
    std::atomic<bool> ShopGUILayer::m_item5Bought = false;
    std::atomic<bool> ShopGUILayer::m_item6Bought = false;
    std::atomic<bool> ShopGUILayer::m_buyError = false;

    ShopGUILayer::ShopGUILayer()
    {
        m_buttonBackTexture = Beryll::Renderer::createTexture("GUI/menus/LeftArrow.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);

        m_shopHeaderTexture = Beryll::Renderer::createTexture("GUI/menus/shop/ShopHeader.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_buttonItem1Texture = Beryll::Renderer::createTexture("GUI/menus/shop/CrystalsItem1.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_buttonItem2Texture = Beryll::Renderer::createTexture("GUI/menus/shop/CrystalsItem2.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_buttonItem3Texture = Beryll::Renderer::createTexture("GUI/menus/shop/CrystalsItem3.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_buttonItem4Texture = Beryll::Renderer::createTexture("GUI/menus/shop/CrystalsItem4.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_buttonItem5Texture = Beryll::Renderer::createTexture("GUI/menus/shop/CrystalsItem5.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);
        m_buttonItem6Texture = Beryll::Renderer::createTexture("GUI/menus/shop/CrystalsItem6.jpg", Beryll::TextureType::DIFFUSE_TEXTURE_MAT_1);

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
        if (m_buttonBackClicked)
        {
            m_buttonBackClicked = false;
            GameStateHelper::popState();
            return;
        }
        else if(m_buttonItem1Clicked)
        {
            m_buttonItem1Clicked = false;
            BR_INFO("%s", "Item 1 clicked.");
            Beryll::BillingSystem::getInstance()->makeInAppPurchaseGooglePlay("purchase_0_99_dollar",
                                                                              m_buyItem1SuccessCallback,
                                                                              m_commonErrorCallback);
        }
        else if(m_buttonItem2Clicked)
        {
            m_buttonItem2Clicked = false;
            BR_INFO("%s", "Item 2 clicked.");
            Beryll::BillingSystem::getInstance()->makeInAppPurchaseGooglePlay("purchase_2_5_dollar",
                                                                              m_buyItem2SuccessCallback,
                                                                              m_commonErrorCallback);
        }
        else if(m_buttonItem3Clicked)
        {
            m_buttonItem3Clicked = false;
            BR_INFO("%s", "Item 3 clicked.");
            Beryll::BillingSystem::getInstance()->makeInAppPurchaseGooglePlay("purchase_5_99_dollar",
                                                                              m_buyItem3SuccessCallback,
                                                                              m_commonErrorCallback);
        }
        else if(m_buttonItem4Clicked)
        {
            m_buttonItem4Clicked = false;
            BR_INFO("%s", "Item 4 clicked.");
            Beryll::BillingSystem::getInstance()->makeInAppPurchaseGooglePlay("purchase_18_dollar",
                                                                              m_buyItem4SuccessCallback,
                                                                              m_commonErrorCallback);
        }
        else if(m_buttonItem5Clicked)
        {
            m_buttonItem5Clicked = false;
            BR_INFO("%s", "Item 5 clicked.");
            Beryll::BillingSystem::getInstance()->makeInAppPurchaseGooglePlay("purchase_49_dollar",
                                                                              m_buyItem5SuccessCallback,
                                                                              m_commonErrorCallback);
        }
        else if(m_buttonItem6Clicked)
        {
            m_buttonItem6Clicked = false;
            BR_INFO("%s", "Item 6 clicked.");
            Beryll::BillingSystem::getInstance()->makeInAppPurchaseGooglePlay("purchase_99_dollar",
                                                                              m_buyItem6SuccessCallback,
                                                                              m_commonErrorCallback);
        }

        if(ShopGUILayer::m_item1Bought)
        {
            ShopGUILayer::m_item1Bought = false;
            BR_INFO("%s", "Item 1 bought.");

            // Give item 1 to player.
        }
        else if(ShopGUILayer::m_item2Bought)
        {
            ShopGUILayer::m_item2Bought = false;
            BR_INFO("%s", "Item 2 bought.");

            // Give item 2 to player.
        }
        else if(ShopGUILayer::m_item3Bought)
        {
            ShopGUILayer::m_item3Bought = false;
            BR_INFO("%s", "Item 3 bought.");

            // Give item 3 to player.
        }
        else if(ShopGUILayer::m_item4Bought)
        {
            ShopGUILayer::m_item4Bought = false;
            BR_INFO("%s", "Item 4 bought.");

            // Give item 4 to player.
        }
        else if(ShopGUILayer::m_item5Bought)
        {
            ShopGUILayer::m_item5Bought = false;
            BR_INFO("%s", "Item 5 bought.");

            // Give item 5 to player.
        }
        else if(ShopGUILayer::m_item6Bought)
        {
            ShopGUILayer::m_item6Bought = false;
            BR_INFO("%s", "Item 6 bought.");

            // Give item 6 to player.
        }
        else if(ShopGUILayer::m_buyError)
        {
            ShopGUILayer::m_buyError = false;
            BR_INFO("%s", "Buy error.");
        }
    }

    void ShopGUILayer::updateAfterPhysics()
    {

    }

    void ShopGUILayer::draw()
    {
        // Button back.
        ImGui::SetNextWindowPos(ImVec2(-0.01f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.905f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.

        ImGui::Begin(m_buttonBackID.c_str(), nullptr, m_noBackgroundNoFrame);
        m_buttonBackClicked = ImGui::ImageButton(m_buttonBackID.c_str(),reinterpret_cast<ImTextureID>(m_buttonBackTexture->getID()),
                                                 ImVec2(0.34f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.1f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::End();

        // Shop header.
        ImGui::SetNextWindowPos(ImVec2(0.3f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.0f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f)); // Set next window size. Set axis to 0.0f to force an auto-fit on this axis.

        ImGui::Begin(m_shopHeaderID.c_str(), nullptr, m_noBackgroundNoFrame);
        ImGui::Image(reinterpret_cast<ImTextureID>(m_shopHeaderTexture->getID()),
                     ImVec2(0.4f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.06f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::End();

        // Button item1.
        ImGui::SetNextWindowPos(ImVec2(0.01f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.15f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));

        ImGui::Begin(m_buttonItem1ID.c_str(), nullptr, m_noBackgroundNoFrame);
        m_buttonItem1Clicked = ImGui::ImageButton(m_buttonItem1ID.c_str(),reinterpret_cast<ImTextureID>(m_buttonItem1Texture->getID()),
                                                 ImVec2(0.3f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.2f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::End();

        // Button item2.
        ImGui::SetNextWindowPos(ImVec2(0.34f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.15f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));

        ImGui::Begin(m_buttonItem2ID.c_str(), nullptr, m_noBackgroundNoFrame);
        m_buttonItem2Clicked = ImGui::ImageButton(m_buttonItem2ID.c_str(),reinterpret_cast<ImTextureID>(m_buttonItem2Texture->getID()),
                                                  ImVec2(0.3f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.2f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::End();

        // Button item3.
        ImGui::SetNextWindowPos(ImVec2(0.67f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.15f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));

        ImGui::Begin(m_buttonItem3ID.c_str(), nullptr, m_noBackgroundNoFrame);
        m_buttonItem3Clicked = ImGui::ImageButton(m_buttonItem3ID.c_str(),reinterpret_cast<ImTextureID>(m_buttonItem3Texture->getID()),
                                                  ImVec2(0.3f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.2f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::End();

        // Button item4.
        ImGui::SetNextWindowPos(ImVec2(0.01f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.375f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));

        ImGui::Begin(m_buttonItem4ID.c_str(), nullptr, m_noBackgroundNoFrame);
        m_buttonItem4Clicked = ImGui::ImageButton(m_buttonItem4ID.c_str(),reinterpret_cast<ImTextureID>(m_buttonItem4Texture->getID()),
                                                  ImVec2(0.3f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.2f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::End();

        // Button item5.
        ImGui::SetNextWindowPos(ImVec2(0.34f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.375f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));

        ImGui::Begin(m_buttonItem5ID.c_str(), nullptr, m_noBackgroundNoFrame);
        m_buttonItem5Clicked = ImGui::ImageButton(m_buttonItem5ID.c_str(),reinterpret_cast<ImTextureID>(m_buttonItem5Texture->getID()),
                                                  ImVec2(0.3f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.2f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::End();

        // Button item6.
        ImGui::SetNextWindowPos(ImVec2(0.67f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.375f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));

        ImGui::Begin(m_buttonItem6ID.c_str(), nullptr, m_noBackgroundNoFrame);
        m_buttonItem6Clicked = ImGui::ImageButton(m_buttonItem6ID.c_str(),reinterpret_cast<ImTextureID>(m_buttonItem6Texture->getID()),
                                                  ImVec2(0.3f * Beryll::MainImGUI::getInstance()->getGUIWidth(), 0.2f * Beryll::MainImGUI::getInstance()->getGUIHeight()));
        ImGui::End();
    }
}
