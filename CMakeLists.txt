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

# Recent CMake versions do not always pass the standard to MSVC, and the code
# needs C++17 (std::filesystem, std::clamp).
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif()

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
option(OG_ENABLE_ASSIMP "Enable direct Assimp model import in the game client (OBJ, FBX, ...)" ON)
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

# ---------------------------------------------------------------------------
# Assimp (direct model import for the game client and the converter)
# ---------------------------------------------------------------------------
set(OG_ASSIMP_FOUND OFF)
if(OG_ENABLE_ASSIMP OR OG_BUILD_MODELCONVERTER)
    find_package(assimp CONFIG QUIET)
    if(assimp_FOUND)
        set(OG_ASSIMP_FOUND ON)
    else()
        if(OG_FETCH_DEPS)
            message(STATUS "Assimp not found - downloading it for model import")
            set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
            set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
            set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
            set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
            set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
            FetchContent_Declare(assimp
                GIT_REPOSITORY https://github.com/assimp/assimp.git
                GIT_TAG v5.4.3
                GIT_SHALLOW TRUE)
            FetchContent_MakeAvailable(assimp)
            set(OG_ASSIMP_FOUND ON)
        else()
            message(WARNING "Assimp is required but was not found and OG_FETCH_DEPS is OFF. "
                            "Disable OG_ENABLE_ASSIMP or install Assimp.")
        endif()
    endif()
endif()

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

# Executables are written next to their assets and change into that directory
# at startup, so every shared dependency has to sit next to the executable as
# well: on Windows otherwise the program fails with STATUS_DLL_NOT_FOUND, on
# Linux it is found through the "$ORIGIN" rpath set for the client.
function(og_copy_runtime_dependency target dependency)
    if(NOT TARGET ${dependency})
        return()
    endif()

    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:${dependency}>"
            "$<TARGET_FILE_DIR:${target}>"
        VERBATIM)

    # On Linux a versioned shared library is built as lib<name>.so.<full version>
    # (for example libassimp.so.5.4.3), but the ELF loader resolves it through its
    # SONAME lib<name>.so.<major> (libassimp.so.5).  Copy the library a second time
    # under that name so the "$ORIGIN" rpath can find it next to the executable.
    if(NOT WIN32)
        get_target_property(dependency_output_name ${dependency} OUTPUT_NAME)
        get_target_property(dependency_soversion ${dependency} SOVERSION)
        if(dependency_output_name AND dependency_soversion)
            add_custom_command(TARGET ${target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "$<TARGET_FILE:${dependency}>"
                    "$<TARGET_FILE_DIR:${target}>/lib${dependency_output_name}$<IF:$<CONFIG:Debug>,${CMAKE_DEBUG_POSTFIX},>.so.${dependency_soversion}"
                VERBATIM)
        endif()
    endif()
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
        GLM_ENABLE_EXPERIMENTAL
        # SDL.h pulls in <windows.h>, whose min/max macros break std::min/max.
        NOMINMAX)
    target_link_libraries(OpenGL_Game PRIVATE
        ${OG_SDL_TARGET}
        ${OG_GLEW_TARGET}
        OpenGL::GL
        og_enet
        nlohmann_json::nlohmann_json
        glm::glm
        og_lua
        og_stb)

    if(OG_ASSIMP_FOUND)
        target_compile_definitions(OpenGL_Game PRIVATE OG_ASSIMP_ENABLED)
        target_link_libraries(OpenGL_Game PRIVATE assimp::assimp)

        # Assimp is shared: it has to sit next to the executable, otherwise the
        # game starts with STATUS_DLL_NOT_FOUND on Windows or with a missing
        # libassimp on Linux.  On Linux the "$ORIGIN" rpath makes the loader
        # search the executable directory.
        og_copy_runtime_dependency(OpenGL_Game assimp::assimp)
        if(NOT WIN32)
            target_link_libraries(OpenGL_Game PRIVATE "-Wl,-rpath,$ORIGIN")
        endif()
    endif()

    if(OG_ENABLE_AUDIO)
        target_compile_definitions(OpenGL_Game PRIVATE OG_AUDIO_ENABLED)
        target_link_libraries(OpenGL_Game PRIVATE og_soloud og_picotts)
    endif()

    if(WIN32)
        target_link_libraries(OpenGL_Game PRIVATE winmm ws2_32)
    endif()

    if(MSVC)
        # MSBuild does not always pull in the UCRT import library for the
        # statically linked C dependencies (Lua, Pico TTS), which leads to
        # unresolved stdio symbols at link time. The static UCRT library that
        # is pulled in by the other prebuilt dependencies is dropped so the
        # import library does not clash with it.
        target_link_libraries(OpenGL_Game PRIVATE ucrt.lib)
        target_link_options(OpenGL_Game PRIVATE /NODEFAULTLIB:libucrt.lib)
        target_link_options(OpenGL_Game PRIVATE "/MAP:${CMAKE_SOURCE_DIR}/Game/OpenGL_Game.map")
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
        nlohmann_json::nlohmann_json
        glm::glm
        og_stb)

    if(OG_ASSIMP_FOUND)
        target_compile_definitions(GameServer PRIVATE OG_ASSIMP_ENABLED)
        target_link_libraries(GameServer PRIVATE assimp::assimp)
        og_copy_runtime_dependency(GameServer assimp::assimp)
        if(NOT WIN32)
            target_link_libraries(GameServer PRIVATE "-Wl,-rpath,$ORIGIN")
        endif()
    endif()

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
        nlohmann_json::nlohmann_json
        glm::glm)

    list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")
    include(Catch)
    catch_discover_tests(og_tests)
endif()

# ---------------------------------------------------------------------------
# Optional model converter (needs Assimp)
# ---------------------------------------------------------------------------
if(OG_BUILD_MODELCONVERTER)
    if(NOT OG_ASSIMP_FOUND)
        message(FATAL_ERROR "The model converter requires Assimp. "
            "Enable OG_FETCH_DEPS or install Assimp.")
    endif()

    add_executable(modelconverter "${CMAKE_SOURCE_DIR}/modelconverter/modelconverter.cpp")
    target_link_libraries(modelconverter PRIVATE assimp::assimp)
    og_set_output_directory(modelconverter "${CMAKE_SOURCE_DIR}/modelconverter")
endif()

# ---------------------------------------------------------------------------
# Packaging: a runnable distribution
# ---------------------------------------------------------------------------
# `cmake --build build --target package` writes
#   build/dist/game     client executable, its assets and its runtime libraries
#   build/dist/server   server executable, its assets and its runtime libraries
# Both executables change into their own directory at startup, so the folder is
# enough to run it.  cmake/package.cmake copies everything except the sources.
set(OG_PACKAGE_TARGETS "")
set(OG_PACKAGE_ARGS -DOG_PACK_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
                    -DOG_PACK_DIR="${CMAKE_BINARY_DIR}/dist")

if(OG_BUILD_GAME)
    list(APPEND OG_PACKAGE_TARGETS OpenGL_Game)
    list(APPEND OG_PACKAGE_ARGS -DOG_PACK_GAME_BINARY="$<TARGET_FILE:OpenGL_Game>")
endif()

if(OG_BUILD_SERVER)
    list(APPEND OG_PACKAGE_TARGETS GameServer)
    list(APPEND OG_PACKAGE_ARGS -DOG_PACK_SERVER_BINARY="$<TARGET_FILE:GameServer>")
endif()

if(OG_PACKAGE_TARGETS)
    add_custom_target(package
        COMMAND ${CMAKE_COMMAND} ${OG_PACKAGE_ARGS}
            -P "${CMAKE_SOURCE_DIR}/cmake/package.cmake"
        DEPENDS ${OG_PACKAGE_TARGETS})
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
message(STATUS "  Assimp import:     ${OG_ENABLE_ASSIMP} (found: ${OG_ASSIMP_FOUND})")
if(OG_ENABLE_AUDIO)
    message(STATUS "  Audio:             ON (SoLoud + SDL2, 3D, Pico TTS voices)")
else()
    message(STATUS "  Audio:             OFF (silent)")
endif()
message(STATUS "  Dependencies:      ${OG_DEPS_DIR}")
message(STATUS "")
