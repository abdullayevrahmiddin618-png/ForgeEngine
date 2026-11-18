
add_library(ForgeCore STATIC
    Core/Engine.cpp
    Core/Logger.cpp
)

target_include_directories(ForgeCore
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Core
)

target_compile_features(ForgeCore PUBLIC cxx_std_20)
