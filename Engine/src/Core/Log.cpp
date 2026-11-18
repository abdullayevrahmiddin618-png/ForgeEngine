#include "Forge/Core/Log.h"
#include <iostream>

namespace Forge {

    void Log::Info(std::string_view message) {
        std::cout << "[INFO] " << message << '\n';
    }

    void Log::Warn(std::string_view message) {
        std::cout << "[WARN] " << message << '\n';
    }

    void Log::Error(std::string_view message) {
        std::cerr << "[ERROR] " << message << '\n';
    }

}
