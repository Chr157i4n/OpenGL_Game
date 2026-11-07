cmake_minimum_required(VERSION 3.24)

project(OpenGL_Game
    VERSION 0.1.0
    DESCRIPTION "Cross-platform OpenGL game and dedicated server"
    LANGUAGES C CXX)

# ---------------------------------------------------------------------------
# Global settings
# ---------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# ---------------------------------------------------------------------------
# Options
# ---------------------------------------------------------------------------
option(OG_BUILD_GAME "Build the game client" ON)
option(OG_BUILD_SERVER "Build the dedicated server" ON)
option(OG_BUILD_MODELCONVERTER "Build the Assimp based BMF converter" OFF)
option(OG_ENABLE_AUDIO "Enable SoLoud audio with 3D sound" ON)
option(OG_USE_SYSTEM_DEPS "Prefer system/vcpkg packages over downloaded dependencies" OFF)
option(OG_FETCH_DEPS "Download missing dependencies with FetchContent" ON)
option(OG_BUILD_TESTS "Build Catch2 unit tests for the shared common code" ON)

set(OG_DEPS_DIR "${CMAKE_SOURCE_DIR}/libs/_deps" CACHE PATH
    "Directory used for downloaded third-party dependencies")

# Download dependencies into the tracked libs/ directory.  The folder is
# ignored by git, but keeps all third-party code in one obvious place.
set(FETCHCONTENT_BASE_DIR "${OG_DEPS_DIR}" CACHE PATH "FetchContent base directory" FORCE)
set(FETCHCONTENT_QUIET OFF)
include(FetchContent)

include(cmake/Dependencies.cmake)

# Shared, platform-independent code used by both the client and the server.
file(GLOB OG_COMMON_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/common/*.cpp")


# ---------------------------------------------------------------------------
# Helper: keep executables next to their assets for every configuration
# ---------------------------------------------------------------------------
function(og_set_output_directory target directory)
    set_target_properties(${target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${directory}"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${directory}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${directory}"
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${directory}"
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${directory}")
endfunction()

# ---------------------------------------------------------------------------
# Game client
# ---------------------------------------------------------------------------
if(OG_BUILD_GAME)
    file(GLOB OG_GAME_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/Game/*.cpp")

    add_executable(OpenGL_Game ${OG_GAME_SOURCES} ${OG_COMMON_SOURCES})
    target_include_directories(OpenGL_Game PRIVATE
        "${CMAKE_SOURCE_DIR}/Game"
        "${CMAKE_SOURCE_DIR}/common")
    target_compile_definitions(OpenGL_Game PRIVATE
        SDL_MAIN_HANDLED
        GLM_ENABLE_EXPERIMENTAL)
    target_link_libraries(OpenGL_Game PRIVATE
        ${OG_SDL_TARGET}
        ${OG_GLEW_TARGET}
        OpenGL::GL
        og_enet
        tinyxml2::tinyxml2
        glm::glm
        og_lua
        og_stb)

    if(OG_ENABLE_AUDIO)
        target_compile_definitions(OpenGL_Game PRIVATE OG_AUDIO_ENABLED)
        target_link_libraries(OpenGL_Game PRIVATE og_soloud)
    endif()

    if(WIN32)
        target_link_libraries(OpenGL_Game PRIVATE winmm ws2_32)
    endif()

    og_set_output_directory(OpenGL_Game "${CMAKE_SOURCE_DIR}/Game")

    if(WIN32 AND TARGET SDL2::SDL2 AND NOT TARGET SDL2::SDL2-static)
        add_custom_command(TARGET OpenGL_Game POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "$<TARGET_FILE:SDL2::SDL2>"
                "$<TARGET_FILE_DIR:OpenGL_Game>"
            VERBATIM)
    endif()

    add_custom_target(run_game
        COMMAND "$<TARGET_FILE:OpenGL_Game>"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/Game"
        DEPENDS OpenGL_Game
        USES_TERMINAL)
endif()

# ---------------------------------------------------------------------------
# Dedicated server
# ---------------------------------------------------------------------------
if(OG_BUILD_SERVER)
    file(GLOB OG_SERVER_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/GameServer/*.cpp")

    add_executable(GameServer ${OG_SERVER_SOURCES} ${OG_COMMON_SOURCES})
    target_include_directories(GameServer PRIVATE
        "${CMAKE_SOURCE_DIR}/GameServer"
        "${CMAKE_SOURCE_DIR}/common")
    target_compile_definitions(GameServer PRIVATE GLM_ENABLE_EXPERIMENTAL)
    target_link_libraries(GameServer PRIVATE
        og_enet
        tinyxml2::tinyxml2
        glm::glm
        og_stb)

    if(WIN32)
        target_link_libraries(GameServer PRIVATE winmm ws2_32)
    endif()

    og_set_output_directory(GameServer "${CMAKE_SOURCE_DIR}/GameServer")

    add_custom_target(run_server
        COMMAND "$<TARGET_FILE:GameServer>"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/GameServer"
        DEPENDS GameServer
        USES_TERMINAL)
endif()

# ---------------------------------------------------------------------------
# Unit tests for the shared common code
# ---------------------------------------------------------------------------
if(OG_BUILD_TESTS)
    enable_testing()

    file(GLOB OG_TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/tests/*.cpp")

    add_executable(og_tests ${OG_COMMON_SOURCES} ${OG_TEST_SOURCES})
    target_include_directories(og_tests PRIVATE
        "${CMAKE_SOURCE_DIR}/common")
    target_compile_definitions(og_tests PRIVATE GLM_ENABLE_EXPERIMENTAL)
    target_link_libraries(og_tests PRIVATE
        Catch2::Catch2WithMain
        glm::glm)

    list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")
    include(Catch)
    catch_discover_tests(og_tests)
endif()

# ---------------------------------------------------------------------------
# Optional model converter (needs Assimp)
# ---------------------------------------------------------------------------
if(OG_BUILD_MODELCONVERTER)
    find_package(assimp CONFIG QUIET)

    if(NOT assimp_FOUND)
        message(STATUS "Assimp not found - downloading it for modelconverter")
        set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
        set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
        set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
        set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
        FetchContent_Declare(assimp
            GIT_REPOSITORY https://github.com/assimp/assimp.git
            GIT_TAG v5.4.3
            GIT_SHALLOW TRUE)
        FetchContent_MakeAvailable(assimp)
    endif()

    add_executable(modelconverter "${CMAKE_SOURCE_DIR}/modelconverter/modelconverter.cpp")
    target_link_libraries(modelconverter PRIVATE assimp::assimp)
    og_set_output_directory(modelconverter "${CMAKE_SOURCE_DIR}/modelconverter")
endif()

# ---------------------------------------------------------------------------
# Informational summary
# ---------------------------------------------------------------------------
message(STATUS "")
message(STATUS "OpenGL_Game configuration")
message(STATUS "  Build type:        ${CMAKE_BUILD_TYPE}")
message(STATUS "  Game client:       ${OG_BUILD_GAME}")
message(STATUS "  Dedicated server:  ${OG_BUILD_SERVER}")
message(STATUS "  Model converter:   ${OG_BUILD_MODELCONVERTER}")
if(OG_ENABLE_AUDIO)
    message(STATUS "  Audio:             ON (SoLoud + SDL2, 3D)")
else()
    message(STATUS "  Audio:             OFF (silent)")
endif()
message(STATUS "  Dependencies:      ${OG_DEPS_DIR}")
message(STATUS "")
