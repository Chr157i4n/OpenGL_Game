# Creates a runnable distribution of the built executables.
#
# Called by the CMake target "package":
#   cmake --build build --target package
#
# Arguments (passed by CMakeLists.txt as -DOG_PACK_...):
#   OG_PACK_SOURCE_DIR    project source directory
#   OG_PACK_DIR           output directory (build/dist)
#   OG_PACK_GAME_BINARY   path of the built client executable (optional)
#   OG_PACK_SERVER_BINARY path of the built server executable (optional)
#
# The result is dist/game and dist/server, each holding the executable, its
# assets (models, textures, shaders, levels, audio, ...) and the shared
# libraries copied next to it by the build.  Both executables change into their
# own directory at startup, so the folder is enough to run them.
# Source files and leftover debug binaries are not part of the distribution.

if(NOT OG_PACK_SOURCE_DIR OR NOT OG_PACK_DIR)
    message(FATAL_ERROR "OG_PACK_SOURCE_DIR and OG_PACK_DIR are required")
endif()

set(OG_EXCLUDE_REGEX "\\.(cpp|h|hpp|map|bak|log|tmp|o|obj|pdb)$")
# Local debug binaries (e.g. libassimp-5d.dll) are not part of a release.
set(OG_DEBUG_REGEX "d\\.(dll|so|exe)$")

function(og_copy_distribution source_directory destination_directory)
    if(NOT IS_DIRECTORY "${source_directory}")
        message(FATAL_ERROR "Distribution source '${source_directory}' does not exist")
    endif()

    # RELATIVE keeps the subdirectory structure (models/, textures, shaders...).
    file(GLOB_RECURSE distribution_files RELATIVE "${source_directory}"
        "${source_directory}/*")

    foreach(distribution_file ${distribution_files})
        if(distribution_file MATCHES "${OG_EXCLUDE_REGEX}" OR
           distribution_file MATCHES "${OG_DEBUG_REGEX}")
            continue()
        endif()

        get_filename_component(subdirectory "${distribution_file}" DIRECTORY)
        file(COPY "${source_directory}/${distribution_file}"
             DESTINATION "${destination_directory}/${subdirectory}")
    endforeach()
endfunction()

if(OG_PACK_GAME_BINARY)
    file(COPY "${OG_PACK_GAME_BINARY}" DESTINATION "${OG_PACK_DIR}/game")
    og_copy_distribution("${OG_PACK_SOURCE_DIR}/Game" "${OG_PACK_DIR}/game")
    message(STATUS "Packaged game client into ${OG_PACK_DIR}/game")
endif()

if(OG_PACK_SERVER_BINARY)
    file(COPY "${OG_PACK_SERVER_BINARY}" DESTINATION "${OG_PACK_DIR}/server")
    og_copy_distribution("${OG_PACK_SOURCE_DIR}/GameServer" "${OG_PACK_DIR}/server")
    message(STATUS "Packaged dedicated server into ${OG_PACK_DIR}/server")
endif()
