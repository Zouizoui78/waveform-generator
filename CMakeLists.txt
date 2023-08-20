cmake_minimum_required(VERSION 3.17)

set(PROJECT waveform-generator)
set(VERSION 0.1)
set(PRJ_VER ${PROJECT}-${VERSION})

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

# vcpkg
include(FetchContent)
FetchContent_Declare(
    vcpkg
    GIT_REPOSITORY https://github.com/microsoft/vcpkg
    GIT_TAG 2023.07.21
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/vcpkg
)
FetchContent_MakeAvailable(vcpkg)
set(CMAKE_TOOLCHAIN_FILE "${vcpkg_SOURCE_DIR}/scripts/buildsystems/vcpkg.cmake" CACHE FILEPATH "")

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

if (UNIX)
    # Needed for clangd
    add_custom_command(
        TARGET ${PROJECT}
        COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
    )
endif (UNIX)
