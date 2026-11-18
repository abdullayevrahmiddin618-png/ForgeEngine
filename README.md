# ForgeEngine

![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)
![CMake](https://img.shields.io/badge/Build-CMake-064F8C.svg)
![OpenGL](https://img.shields.io/badge/Graphics-OpenGL-5586A4.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)

**ForgeEngine** is an open-source 2D & 3D Game Engine built from scratch in modern C++20.

Designed with clean, modular architecture, ForgeEngine aims to give developers full control over rendering, scenes, physics, and tooling — starting from a lightweight core and scaling up to a full game development environment.

---

## 🛠️ Tech Stack

| Technology | Purpose |
| :--- | :--- |
| **C++20** | Core engine language |
| **CMake** | Cross-platform build system |
| **OpenGL** | Initial graphics API (Vulkan planned) |
| **GLFW** | Window creation & input handling |
| **GLAD** | OpenGL function loader |
| **GLM** | 3D/2D math library |
| **Dear ImGui** | Integrated editor & debug UI |

---

## 🗺️ Roadmap

### Phase 1 — Foundation (Current)
- [ ] CMake project setup (`Engine` library + `Sandbox` app)
- [ ] Core Application loop & Entry point
- [ ] Logging system
- [ ] Window management (GLFW) & Event system
- [ ] OpenGL context initialization (GLAD)

### Phase 2 — Rendering
- [ ] Shader & Buffer abstraction (VBO, VAO, IBO)
- [ ] 2D Renderer (Sprites, Textures, Batch Rendering)
- [ ] Camera system (Orthographic & Perspective)
- [ ] 3D Renderer (Meshes, Lighting, Materials)

### Phase 3 — Engine Systems
- [ ] Entity Component System (ECS)
- [ ] Scene management & Serialization
- [ ] Asset manager
- [ ] Physics & Audio integration

### Phase 4 — Editor & Export
- [ ] Dear ImGui docking editor (Viewport, Hierarchy, Inspector)
- [ ] Project & Build export tools

---

## 📁 Project Structure

```text
ForgeEngine/
├── Engine/             # Core engine static library
│   ├── include/        # Public headers
│   └── src/            # Engine source files
├── Sandbox/            # Test application / Game project
├── Assets/             # Shaders, textures, models
├── CMakeLists.txt      # Root CMake configuration
├── .gitignore
├── LICENSE
└── README.md
