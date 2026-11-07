#pragma once

#include <filesystem>
#include <system_error>

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <limits.h>
#include <unistd.h>
#endif

namespace Platform
{
    // Change into the directory that contains the executable.  The game uses
    // relative asset paths, so this makes launching from IDEs, shells and file
    // managers behave the same way on Windows and Linux.
    inline void setWorkingDirectoryToExecutable()
    {
#ifdef _WIN32
        char buffer[MAX_PATH] = {};
        const DWORD length = GetModuleFileNameA(nullptr, buffer, MAX_PATH);
        if (length > 0 && length < MAX_PATH)
        {
            std::error_code error;
            std::filesystem::current_path(
                std::filesystem::path(buffer).parent_path(), error);
        }
#else
        char buffer[PATH_MAX] = {};
        const ssize_t length = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
        if (length > 0)
        {
            buffer[length] = '\0';
            std::error_code error;
            std::filesystem::current_path(
                std::filesystem::path(buffer).parent_path(), error);
        }
#endif
    }
}
