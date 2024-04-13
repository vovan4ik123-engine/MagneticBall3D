cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-fexceptions -frtti)

# for info/warning/assert messages
    add_definitions(-DBR_DEBUG)

add_library(main SHARED main.cpp
                        GameStateHelper.cpp
                        DataBaseHelper.cpp

                        # Menu.
                        menus/start/StartMenuGUILayer.cpp
                        menus/settings/SettingsMenuGUILayer.cpp
                        menus/shop/ShopGUILayer.cpp
                        menus/playerTalents/PlayerTalentsGUILayer.cpp

                        # Maps.
                        maps/BaseMap.cpp
                        maps/Map1.cpp

                        # Play state.
                        playState/PlayStateGUILayer.cpp
                        playState/PlayStateSceneLayer.cpp

                        player/Player.cpp
                        garbage/Garbage.cpp
                        pathfinding/AStar.cpp
                        improvements/Improvements.cpp

                        # Enemies.
                        enemies/BaseEnemy.cpp
                        enemies/CopWithPistol.cpp
                        enemies/CopWithPistolShield.cpp
                        enemies/CopWithGrenadeLauncher.cpp
                        enemies/Sniper.cpp
            )

target_include_directories(main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

#all other desired libs should be included in Beryll-static lib
target_link_libraries(main Beryll-static GLESv3)
