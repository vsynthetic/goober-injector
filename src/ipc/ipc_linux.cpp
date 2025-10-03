#include "ipc.hpp"
#include <filesystem>
#include <iostream>
#include <string>

#include <sys/socket.h>
#include <sys/unistd.h>
#include <sys/un.h>
#include <unistd.h>

static std::string get_message(int err) {
    char buf[256];
    strerror_r(err, buf, sizeof(buf));
    return std::string(buf, 256);
}

ipc_pipe::ipc_pipe(std::string _path) : fd(-1), saddr({}) {
    path = std::filesystem::path(_path);
    auto str = path.string();

    if (str.size() >= sizeof(saddr.sun_path)) {
        std::cerr << "Socket path too long!" << std::endl;
        exit(1);
    }

    fd = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0);

    if (fd == -1) {
        std::cerr << "Failed to create socket: " << get_message(errno) << std::endl;
        exit(1);
    }

    memset(&saddr, 0, sizeof(saddr));
    saddr.sun_family = AF_UNIX;
    strncpy(saddr.sun_path, path.c_str(), sizeof(saddr.sun_path) - 1);
}

void ipc_pipe::shutdown() {
    if (fd != -1) {
        close(fd);
        fd = -1;
    }
}

bool ipc_pipe::is_connected() {
    return fd != -1;
}

size_t ipc_pipe::write(void* buffer, size_t size) {
    return ::write(fd, buffer, size);
}

bool ipc_pipe::connect() {
    if (::connect(fd, (struct sockaddr*) &saddr, sizeof(saddr)) == -1) {
        //std::cerr << "Failed to connect to socket: " << get_message(errno) << std::endl;
        return false;
    }

    return true;
}

ipc_pipe::~ipc_pipe() {
    if (fd != -1) {
        close(fd);
    }
}
