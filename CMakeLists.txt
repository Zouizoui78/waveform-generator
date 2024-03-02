cmake_minimum_required(VERSION 3.17)

set(VERSION 0.1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Qt variables
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS src)

project(
    waveform-generator
    VERSION ${VERSION}
    LANGUAGES CXX
)

set(
    SRC
    src/main.cpp
    src/MainWindow.cpp
    src/sdl_audio_device.cpp
    src/waveform_player.cpp
    src/MainWindow.ui
)

add_executable(${PROJECT_NAME} ${SRC})

include(FetchContent)
FetchContent_Declare(
  kissfft
  GIT_REPOSITORY https://github.com/mborgerding/kissfft
  GIT_TAG 8f47a67f595a6641c566087bf5277034be64f24d
)
FetchContent_Declare(
  tools
  GIT_REPOSITORY https://github.com/Zouizoui78/tools
  GIT_TAG ef4dbe15eb764df80449aa3499a50989d57e1a0d
)

# 'CACHE INTERNAL ""' forces the value
set(KISSFFT_DATATYPE double CACHE INTERNAL "")
set(KISSFFT_TOOLS OFF CACHE INTERNAL "")
set(KISSFFT_TEST OFF CACHE INTERNAL "")
set(KISSFFT_STATIC ON CACHE INTERNAL "")
FetchContent_MakeAvailable(kissfft tools)

find_package(Qt6 COMPONENTS Core Charts Widgets CONFIG REQUIRED)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE fmt
    PRIVATE kissfft
    PRIVATE Qt6::Charts
    PRIVATE Qt6::Widgets
    PRIVATE tools-waveform
    PRIVATE SDL2
    PRIVATE spdlog
)