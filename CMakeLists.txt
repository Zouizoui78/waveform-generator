cmake_minimum_required(VERSION 3.17)

set(PROJECT waveform-generator)
set(VERSION 0.1)
set(PRJ_VER ${PROJECT}-${VERSION})

# --- Configuration ---

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Qt variables
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS src)

add_compile_options($<$<CONFIG:Debug>:-DDEBUG>$<$<CONFIG:Release>:-DRELEASE>)

if (WIN32)
    add_compile_options(-DWINDOWS)
endif (WIN32)

if (UNIX)
    add_compile_options(-DLINUX)
endif (UNIX)

# --- Project ---

project(
    ${PROJECT}
    VERSION ${VERSION}
    LANGUAGES CXX
)

set(
    SRC
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.ui
)

add_executable(${PROJECT} ${SRC})

find_package(kissfft CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(tools CONFIG REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Charts Widgets)

target_link_libraries(
    ${PROJECT}
    PRIVATE kissfft::kissfft-float
    PRIVATE Qt::Charts
    PRIVATE Qt::Widgets
    PRIVATE tools-sdl
    PRIVATE spdlog::spdlog
)
