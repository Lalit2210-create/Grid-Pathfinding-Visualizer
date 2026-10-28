cmake_minimum_required(VERSION 3.29)
project(project_pathfinding_grid)
set(CMAKE_CXX_STANDARD 20)
find_package(SFML COMPONENTS Network Graphics Window Audio System CONFIG REQUIRED)
find_package(TGUI CONFIG REQUIRED)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
        main.cpp
        utils/Constants.hpp
        graph/Graph.cpp
        graph/Graph.hpp
        graph/AStarRunner.cpp
        graph/AStarRunner.hpp
        graph/StepResult.hpp
        graph/DijkstraRunner.cpp
        graph/DijkstraRunner.hpp
        grid/Cell.cpp
        grid/Cell.hpp
        ui/UIManager.hpp
        ui/UIManager.cpp
)

add_executable(${PROJECT_NAME} main.cpp
        utils/Constants.hpp
        graph/Graph.cpp
        graph/Graph.hpp
        graph/AStarRunner.cpp
        graph/AStarRunner.hpp
        graph/StepResult.hpp
        graph/DijkstraRunner.cpp
        graph/DijkstraRunner.hpp
        grid/Cell.cpp
        grid/Cell.hpp
        ui/UIManager.hpp
        ui/UIManager.cpp
        utils/RoundedRectangle.hpp
)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        SFML::Network
        SFML::Graphics
        SFML::Window
        SFML::Audio
        SFML::System
        TGUI::TGUI
)

if(WIN32)
    target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup>
    )
endif()
