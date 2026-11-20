add_executable(Sandbox
    src/main.cpp
    src/EditorApp.cpp
    src/Editor/EditorUI.cpp
    src/Editor/SceneSerializer.cpp
)

target_include_directories(Sandbox PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(Sandbox PRIVATE ForgeEngine)

set_target_properties(Sandbox PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
