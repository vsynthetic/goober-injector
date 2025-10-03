#include "imgui.h"
#include "imgui/imgui_wrap.hpp"
#include "imgui_stdlib.h"
#include "injector/injector.hpp"
#include "ipc/ipc.hpp"
#include "network/messages.hpp"
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <thread>

#ifdef _WIN32
#define PIPE_PATH "\\\\.\\pipe\\meow"
#else
#define PIPE_PATH "/tmp/meow.ipc"
#endif

int main(int argc, char* argv[]) {

    static std::string dll_path;
    static char jar_path[512];
    static char entrypoint[256];

#ifdef _WIN32
    static auto home = std::filesystem::path(getenv("HOMEPATH"));
#else
    static auto home = std::filesystem::path(getenv("HOME"));
#endif

    static auto path = std::filesystem::path(home.string() + "/.goober");

    if (std::filesystem::exists(path)) {
        std::ifstream config(path);

        if (config) {
            std::getline(config, dll_path);

            std::string line;

            if (std::getline(config, line)) {
                strncpy(jar_path, line.data(), 511);
                jar_path[511] = 0;
            }

            if (std::getline(config, line)) {
                strncpy(entrypoint, line.data(), 255);
                entrypoint[255] = 0;
            }

            config.close();
        }
    }


    return imgui_wrapper().run([] {
        ImGui::InputText("DLL path", &dll_path, 0, nullptr, nullptr);
        ImGui::InputText("Jar path", jar_path, 512);
        ImGui::InputText("Entrypoint", entrypoint, 256);

        bool should_inject = ImGui::Button("Inject");

        int loop_value = injector::get()->password_popup_loop();
        if (loop_value == 1) {
            return;
        }

        if (should_inject || loop_value == 2) {
            std::ofstream config(path);
            if (config) {
                config << dll_path << "\n";
                config << jar_path << "\n";
                config << entrypoint << "\n";
                config.close();
            }

            injector::get()->load_library(dll_path, "java");

            ipc_pipe pipe(PIPE_PATH);

            if (!pipe.connect()) {
                for (int i = 1; i < 5; i++) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));

                    if (pipe.connect())
                        goto connection_established;
                }

            }

        connection_established:
            auto message = load_jar_message{};
            std::memcpy(message.path, jar_path, 512);
            std::memcpy(message.entrypoint, entrypoint, 256);
            pipe.write_message(message_type::LOAD_JAR, message);
        }
    });

    return 0;
}
