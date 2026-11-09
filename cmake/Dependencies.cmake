# Third-party dependency discovery.
#
# Every dependency is first looked up as a system package when
# OG_USE_SYSTEM_DEPS is ON.  Missing dependencies are downloaded into
# libs/_deps (configured through FETCHCONTENT_BASE_DIR) when OG_FETCH_DEPS is ON.

# OpenGL, SDL2 and GLEW are only needed by the graphical client.
if(OG_BUILD_GAME)
    find_package(OpenGL REQUIRED)
endif()

if(POLICY CMP0169)
    cmake_policy(SET CMP0169 OLD)
endif()

# Fetch a source archive that does not ship its own CMakeLists.txt.
function(og_fetch_source name repository tag)
    if(NOT OG_FETCH_DEPS)
        message(FATAL_ERROR
            "Dependency '${name}' is missing. Install it system-wide, pass "
            "-DOG_FETCH_DEPS=ON, or place it in libs/.")
    endif()

    set(og_fetch_shallow TRUE)
    if(ARGC GREATER 3)
        set(og_fetch_shallow ${ARGV3})
    endif()

    FetchContent_Declare(${name}
        GIT_REPOSITORY "${repository}"
        GIT_TAG "${tag}"
        GIT_SHALLOW ${og_fetch_shallow})

    FetchContent_GetProperties(${name})
    if(NOT ${name}_POPULATED)
        message(STATUS "Downloading ${name} (${tag})")
        FetchContent_Populate(${name})
    endif()
endfunction()

# ---------------------------------------------------------------------------
# GLM (header only)
# ---------------------------------------------------------------------------
if(OG_USE_SYSTEM_DEPS)
    find_package(glm CONFIG QUIET)
endif()

if(NOT TARGET glm::glm)
    FetchContent_Declare(glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1
        GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(glm)
endif()

# ---------------------------------------------------------------------------
# Catch2 (unit tests for the shared common code)
# ---------------------------------------------------------------------------
if(OG_BUILD_TESTS)
    if(OG_USE_SYSTEM_DEPS)
        find_package(Catch2 3 CONFIG QUIET)
    endif()

    if(NOT TARGET Catch2::Catch2WithMain)
        FetchContent_Declare(catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v3.7.1
            GIT_SHALLOW TRUE)
        FetchContent_MakeAvailable(catch2)
    endif()
endif()


# ---------------------------------------------------------------------------
# stb (header only)
# ---------------------------------------------------------------------------
if(NOT TARGET og_stb)
    og_fetch_source(stb https://github.com/nothings/stb.git
        4e9b368bfc071e97206ea5c6adba68dd0cbf8897)
    FetchContent_GetProperties(stb)

    add_library(og_stb INTERFACE)
    target_include_directories(og_stb INTERFACE "${stb_SOURCE_DIR}")
endif()

# ---------------------------------------------------------------------------
# SDL2
# ---------------------------------------------------------------------------
if(OG_BUILD_GAME)
    if(OG_USE_SYSTEM_DEPS)
        find_package(SDL2 CONFIG QUIET)
    endif()

    if(NOT TARGET SDL2::SDL2 AND NOT TARGET SDL2::SDL2-static)
        set(SDL_SHARED OFF CACHE BOOL "" FORCE)
        set(SDL_STATIC ON CACHE BOOL "" FORCE)
        set(SDL_TEST OFF CACHE BOOL "" FORCE)
        set(SDL_TESTS OFF CACHE BOOL "" FORCE)
        set(SDL2_DISABLE_INSTALL ON CACHE BOOL "" FORCE)

        FetchContent_Declare(SDL2
            GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
            GIT_TAG release-2.32.10
            GIT_SHALLOW TRUE)
        FetchContent_MakeAvailable(SDL2)
    endif()

    if(TARGET SDL2::SDL2-static)
        set(OG_SDL_TARGET SDL2::SDL2-static)
    elseif(TARGET SDL2::SDL2)
        set(OG_SDL_TARGET SDL2::SDL2)
    elseif(TARGET SDL2-static)
        set(OG_SDL_TARGET SDL2-static)
    else()
        message(FATAL_ERROR "SDL2 was found but no usable SDL2 target exists")
    endif()
endif()

# ---------------------------------------------------------------------------
# SoLoud (audio engine, built from source with the SDL2 backend)
# ---------------------------------------------------------------------------
if(OG_BUILD_GAME AND OG_ENABLE_AUDIO)
    find_package(Threads REQUIRED)

    if(OG_USE_SYSTEM_DEPS)
        find_package(SoLoud CONFIG QUIET)
    endif()

    if(TARGET SoLoud::SoLoud)
        add_library(og_soloud INTERFACE)
        target_link_libraries(og_soloud INTERFACE SoLoud::SoLoud)
    elseif(TARGET soloud)
        add_library(og_soloud INTERFACE)
        target_link_libraries(og_soloud INTERFACE soloud)
    else()
        # SoLoud does not ship a CMakeLists.txt.  The pinned commit is cloned
        # in full because Git cannot fetch an arbitrary commit shallowly.
        og_fetch_source(soloud https://github.com/jarikomppa/soloud.git
            e82fd32c1f62183922f08c14c814a02b58db1873 FALSE)
        FetchContent_GetProperties(soloud)

        file(GLOB OG_SOLOUD_CORE CONFIGURE_DEPENDS
            "${soloud_SOURCE_DIR}/src/core/*.cpp")
        file(GLOB OG_SOLOUD_WAV CONFIGURE_DEPENDS
            "${soloud_SOURCE_DIR}/src/audiosource/wav/*.cpp")

        add_library(og_soloud STATIC
            ${OG_SOLOUD_CORE}
            ${OG_SOLOUD_WAV}
            "${soloud_SOURCE_DIR}/src/audiosource/wav/stb_vorbis.c"
            "${soloud_SOURCE_DIR}/src/backend/sdl2_static/soloud_sdl2_static.cpp")

        target_include_directories(og_soloud PUBLIC
            "${soloud_SOURCE_DIR}/include")
        target_compile_definitions(og_soloud PRIVATE WITH_SDL2_STATIC)
        target_link_libraries(og_soloud PUBLIC
            ${OG_SDL_TARGET}
            Threads::Threads)

        if(NOT WIN32)
            target_link_libraries(og_soloud PUBLIC m ${CMAKE_DL_LIBS})
        endif()
    endif()
endif()

# ---------------------------------------------------------------------------
# PicoTTS (SVOX text-to-speech, static library built from pico/lib sources)
# ---------------------------------------------------------------------------
if(OG_BUILD_GAME AND OG_ENABLE_AUDIO)
    if(OG_USE_SYSTEM_DEPS)
        find_package(PicoTTS CONFIG QUIET)
    endif()

    if(TARGET PicoTTS::PicoTTS)
        add_library(og_picotts INTERFACE)
        target_link_libraries(og_picotts INTERFACE PicoTTS::PicoTTS)
    elseif(NOT TARGET og_picotts)
        # The upstream project only ships Android makefiles, so the engine is
        # compiled directly from the portable C sources in pico/lib.
        og_fetch_source(picotts https://github.com/ihuguet/picotts.git
            21089d223e177ba3cb7e385db8613a093dff74b5 FALSE)
        FetchContent_GetProperties(picotts)

        file(GLOB OG_PICOTTS_SOURCES CONFIGURE_DEPENDS
            "${picotts_SOURCE_DIR}/pico/lib/*.c")

        add_library(og_picotts STATIC ${OG_PICOTTS_SOURCES})
        target_include_directories(og_picotts PUBLIC
            "${picotts_SOURCE_DIR}/pico/lib")

        # MSVC has no <endian.h>; provide a tiny compatibility shim.
        if(MSVC)
            target_include_directories(og_picotts PRIVATE
                "${CMAKE_CURRENT_LIST_DIR}/picotts_compat")
        endif()
    endif()
endif()

# ---------------------------------------------------------------------------
# GLEW
# ---------------------------------------------------------------------------
if(OG_BUILD_GAME)
    if(OG_USE_SYSTEM_DEPS)
        find_package(GLEW QUIET)
    endif()

    if(TARGET GLEW::GLEW)
        set(OG_GLEW_TARGET GLEW::GLEW)
    else()
        set(glew-cmake_BUILD_SHARED OFF CACHE BOOL "" FORCE)
        set(glew-cmake_BUILD_STATIC ON CACHE BOOL "" FORCE)

        FetchContent_Declare(glew
            GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
            GIT_TAG glew-cmake-2.3.1
            GIT_SHALLOW TRUE)
        FetchContent_MakeAvailable(glew)

        if(TARGET libglew_static)
            set(OG_GLEW_TARGET libglew_static)
        elseif(TARGET glew_s)
            set(OG_GLEW_TARGET glew_s)
        elseif(TARGET glew)
            set(OG_GLEW_TARGET glew)
        else()
            message(FATAL_ERROR "GLEW was downloaded but no usable target exists")
        endif()
    endif()
endif()

# ---------------------------------------------------------------------------
# ENet (zpl-c fork - dual-stack IPv4/IPv6)
# ---------------------------------------------------------------------------
FetchContent_Declare(enet
    GIT_REPOSITORY https://github.com/zpl-c/enet.git
    GIT_TAG v2.7.0
    GIT_SHALLOW TRUE)

# The fork is a single header (include/enet.h) compiled by test/library.c.
# Its own test executables are not needed here.
set(ENET_TEST           OFF CACHE BOOL "" FORCE)
set(ENET_STATIC         ON  CACHE BOOL "" FORCE)
set(ENET_SHARED         OFF CACHE BOOL "" FORCE)
set(ENET_IPV4_ONLY      OFF CACHE BOOL "" FORCE)
set(ENET_USE_MORE_PEERS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(enet)
FetchContent_GetProperties(enet)

# The fork has no plain "enet" target: it builds enet_static (alias
# enet::enet_static) and/or enet_shared.  Wrap it in a modern interface target.
if(NOT TARGET og_enet)
    add_library(og_enet INTERFACE)

    if(TARGET enet_static)
        target_link_libraries(og_enet INTERFACE enet_static)
    elseif(TARGET enet_shared)
        target_link_libraries(og_enet INTERFACE enet_shared)
    else()
        message(FATAL_ERROR "ENet was downloaded but no usable target exists")
    endif()

    target_include_directories(og_enet INTERFACE "${enet_SOURCE_DIR}/include")

    if(WIN32)
        target_link_libraries(og_enet INTERFACE winmm ws2_32)
    endif()
endif()

# ---------------------------------------------------------------------------
# TinyXML2
# ---------------------------------------------------------------------------
if(OG_USE_SYSTEM_DEPS)
    find_package(tinyxml2 CONFIG QUIET)
endif()

if(NOT TARGET tinyxml2::tinyxml2)
    set(tinyxml2_BUILD_TESTING OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(tinyxml2
        GIT_REPOSITORY https://github.com/leethomason/tinyxml2.git
        GIT_TAG 10.0.0
        GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(tinyxml2)
endif()

# ---------------------------------------------------------------------------
# Lua (static library built from onelua.c, no CMake project upstream)
# ---------------------------------------------------------------------------
if(OG_BUILD_GAME)
    if(OG_USE_SYSTEM_DEPS)
        find_package(Lua 5.4 QUIET)
    endif()

    if(LUA_FOUND AND LUA_INCLUDE_DIR AND LUA_LIBRARIES)
        add_library(og_lua INTERFACE)
        target_include_directories(og_lua INTERFACE "${LUA_INCLUDE_DIR}")
        target_link_libraries(og_lua INTERFACE ${LUA_LIBRARIES})
    else()
        og_fetch_source(lua https://github.com/lua/lua.git v5.4.7)
        FetchContent_GetProperties(lua)

        add_library(og_lua STATIC "${lua_SOURCE_DIR}/onelua.c")
        target_include_directories(og_lua PUBLIC "${lua_SOURCE_DIR}")
        target_compile_definitions(og_lua PRIVATE MAKE_LIB)
        target_link_libraries(og_lua PUBLIC ${CMAKE_DL_LIBS})
        if(NOT WIN32)
            target_link_libraries(og_lua PUBLIC m)
        endif()
    endif()
endif()
