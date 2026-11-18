add_library(ForgeEngine STATIC
    src/Core/Log.cpp
    src/Core/Application.cpp
)

target_include_directories(ForgeEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
