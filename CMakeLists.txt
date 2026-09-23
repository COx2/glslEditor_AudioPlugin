cmake_minimum_required(VERSION 3.22)

project(GLSLPlugIn VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 9.0.2
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

# JUCE has no native CLAP support, so we pull in the community
# clap-juce-extensions project to add a CLAP target alongside the
# Standalone/VST3 ones. Pinned to a specific commit (rather than a
# branch) for reproducible builds; its CI tests against JUCE 9.0.0.
FetchContent_Declare(
    clap-juce-extensions
    GIT_REPOSITORY https://github.com/free-audio/clap-juce-extensions.git
    GIT_TAG 55525c9858d4b25687be7759a5e0f70eccef218e
    GIT_SUBMODULES_RECURSE TRUE
)
FetchContent_MakeAvailable(clap-juce-extensions)

add_subdirectory(GLSLPlugIn)
