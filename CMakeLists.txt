cmake_minimum_required(VERSION 3.22)
project(WordPlay DESCRIPTION "Exercise your language skills with this card game")

include(VelyraBuildUtils/VelyraBuildUtils.cmake)

set(VELYRA_APP_FRAMEWORK_PATH)
vl_include_or_fetch(VelyraAppFramework VELYRA_APP_FRAMEWORK_PATH)
message(STATUS "VelyraAppFramework found at ${VELYRA_APP_FRAMEWORK_PATH}")

add_executable(WordPlay main.cpp
    src/WordPlayLayer.cpp
    src/WordPlayLayer.hpp
    src/WordPlayInclude.hpp

    src/Layouts/CardGameLayout.cpp
    src/Layouts/CardGameLayout.hpp

    src/Model/Session.hpp
    src/Model/Store.cpp
    src/Model/Store.hpp
    src/Model/WordList.cpp
    src/Model/WordList.hpp

    src/Popups/StatsPopup.cpp
    src/Popups/StatsPopup.hpp

    src/Widgets/Widgets.cpp
    src/Widgets/Widgets.hpp
)

vl_configure_target(WordPlay)
target_link_libraries(WordPlay PRIVATE VelyraAppFramework)
target_include_directories(WordPlay PRIVATE src/Shared)
target_precompile_headers(WordPlay PRIVATE src/Shared/Pch.hpp)
install(TARGETS WordPlay
    RUNTIME DESTINATION bin
)
