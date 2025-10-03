#pragma once

#include <filesystem>
#include <string>
#include <sys/un.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "../network/messages.hpp"

class ipc_pipe {

#ifdef _WIN32
#else
    std::filesystem::path path;
    sockaddr_un saddr;
    int fd;
#endif

public:
    ipc_pipe(std::string path);
    ~ipc_pipe();

    void shutdown();

    bool is_connected();

    bool connect();

    size_t write(void* buffer, size_t size);

    template <typename T>
    inline void write_message(message_type type, T message) {
        write(&type, sizeof(message_type));
        write(&message, sizeof(T));
    }

};
