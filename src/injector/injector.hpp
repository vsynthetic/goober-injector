#pragma once

#include <filesystem>
#include <memory>
#include <string>

class injector {

public:
    static std::shared_ptr<injector>& get();

    void load_library(std::filesystem::path path, std::string processName);

    int password_popup_loop();

};
