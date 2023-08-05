# waveform-generator

## Build

First update vcpkg baseline :

    vcpkg x-update-baseline --add-initial-baseline

Then configure and build :

    # preset = releasewin on Windows
    cmake --preset=release
    cmake --build build/release
