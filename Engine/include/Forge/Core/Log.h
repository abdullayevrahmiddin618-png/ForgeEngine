#pragma once

#include <string_view>

namespace Forge {

    class Log {
    public:
        static void Info(std::string_view message);
        static void Warn(std::string_view message);
        static void Error(std::string_view message);
    };

}

#define FE_INFO(msg)  ::Forge::Log::Info(msg)
#define FE_WARN(msg)  ::Forge::Log::Warn(msg)
#define FE_ERROR(msg) ::Forge::Log::Error(msg)
