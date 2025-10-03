#include "imgui.h"
#include "injector.hpp"
#include <cstdio>
#include <cstring>
#include <memory>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

std::shared_ptr<injector>& injector::get() {
    static auto instance = std::make_shared<injector>();
    return instance;
}

static bool password_submitted = false;
static char password[256];

int injector::password_popup_loop() {
    if (ImGui::IsPopupOpen("Enter password")) {
        if (ImGui::BeginPopupModal("Enter password")) {
            ImGui::InputText("Password", password, 256, ImGuiInputTextFlags_Password);

            if (ImGui::Button("Submit")) {
                password_submitted = true;
                ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
                return 2;
            }

            ImGui::EndPopup();
        }
        return 1;
    }

    return 0;
}

void injector::load_library(std::filesystem::path path, std::string processName) {
    if (!password_submitted) {
        memset(password, 0, 256);
        ImGui::OpenPopup("Enter password", ImGuiPopupFlags_AnyPopupLevel);
        return;
    }

    int sv[2];

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == -1) {
        perror("socketpair");
        return;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        close(sv[0]);
        close(sv[1]);
        return;
    }

    if (pid == 0) {
        close(sv[0]);

        if (dup2(sv[1], STDIN_FILENO) == -1) {
            perror("dup2 stdin");
            exit(1);
        }
        if (dup2(sv[1], STDOUT_FILENO) == -1) {
            perror("dup2 stdout");
            exit(1);
        }
        if (dup2(sv[1], STDERR_FILENO) == -1) {
            perror("dup2 stderr");
            exit(1);
        }

        close(sv[1]);

        std::stringstream command;
        command << "sudo -S gdb -batch-silent -p ";
        command << "$(echo \"$(pidof " << processName << ")\" | cut -d \" \" -f 1) ";
        command << "-ex \"call (void*) dlopen(\\\"" << path.string() << "\\\", 2)\"";

        std::string built = command.str();

        execlp("sh", "sh", "-c", built.c_str(), nullptr);

        perror("execlp");
        exit(1);
    }

    close(sv[1]);

    FILE *fp = fdopen(sv[0], "r+");
    if (!fp) {
        perror("fdopen");
        close(sv[0]);
        return;
    }

    setvbuf(fp, NULL, _IONBF, 0);

    std::stringstream stream;
    stream << password << "\n";
    std::string out = stream.str();

    fprintf(fp, "%s", out.c_str());
    fclose(fp);

    int status = 0;
    pid_t w = waitpid(pid, &status, 0);
    if (w == -1) {
        perror("waitpid");
        return;
    }

    if (WIFEXITED(status)) {
        printf("child exited with status %d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("child killed by signal %d\n", WTERMSIG(status));
    } else {
        printf("child ended with unknown status\n");
    }
}
