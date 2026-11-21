# ForgeEngine

![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)
![CMake](https://img.shields.io/badge/Build-CMake-064F8C.svg)
![OpenGL](https://img.shields.io/badge/Graphics-OpenGL%203.3-5586A4.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)

**ForgeEngine** is an open-source 3D Game Engine & Editor built from scratch in modern **C++20**.

Designed with a clean, modular architecture (`Engine` static library + `Sandbox` editor/game layer), ForgeEngine gives developers direct control over 3D rendering, real-time shadows, physics, native C++ scripting, and scene tooling.

---

## ✨ Key Features

- **3D Rendering (`SceneRenderer`):** OpenGL 3.3 Core pipeline with Phong lighting, **2048×2048 PCF Real-Time Shadow Mapping**, **360° Procedural Skybox**, and an **Infinite 3D Grid**.
- **3D Models & Textures:** Built-in primitives (Cube, Sphere, Pyramid, Plane), custom `.obj` mesh loader, and `.png`/`.jpg` texture loading via `stb_image` and native Windows File Dialog (`Browse...`).
- **Forge Physics:** Built-in AABB collision detection, gravity, static/dynamic bodies, and configurable bounciness.
- **Native C++ Scripting:** Extensible `Forge::Script` & `Forge::ScriptRegistry` system (`OnCreate`, `OnUpdate`, `OnDestroy`) with a sample `PlayerScript` (WASD movement, jumping, and smooth camera follow).
- **Integrated 3D Editor:** Dear ImGui interface with clean `Segoe UI` typography, Framebuffer-based 3D Viewport, mouse raycasting selection, interactive **3D Gizmos (Move, Rotate, Scale)**, Hierarchy, Inspector, Console, and Play/Pause/Stop simulation with automatic scene state backup/restore.
- **Scene Serialization:** Save and load full scenes (entities, transforms, models, textures, physics, scripts, and lighting) to `.forge` files.

---

## 🛠️ Tech Stack

| Technology | Purpose |
| :--- | :--- |
| **C++20** | Core engine & native scripting language |
| **CMake** | Cross-platform build system |
| **OpenGL 3.3 Core** | Graphics API |
| **GLFW** | Window creation & input handling |
| **GLAD** | OpenGL function loader |
| **GLM** | 3D/2D math library |
| **Dear ImGui** | Integrated 3D Editor UI |
| **stb_image** | Image & texture loading |

---

## 🗺️ Roadmap

### Phase 1 — Foundation
- [x] CMake project setup (`Engine` library + `Sandbox` editor)
- [x] Core Application loop & Entry point
- [x] Logging system & Input manager
- [x] Window management (GLFW) & OpenGL context (GLAD)
- [x] Native Windows File Dialog (`FileDialog`)

### Phase 2 — 3D Rendering
- [x] Shader, VertexArray, Framebuffer & Texture abstractions
- [x] Perspective Camera & Infinite 3D Grid
- [x] Modular `SceneRenderer` with `.obj` model & texture caching
- [x] Real-Time Shadow Mapping (2048×2048 PCF)
- [x] 360° Procedural Skybox
- [ ] Multiple Point & Spot Lights
- [ ] PBR Materials & `.glTF` skeletal animation

### Phase 3 — Engine Systems
- [x] Scene & Entity management
- [x] Scene Serialization (`scene.forge`)
- [x] `Forge Physics` v1 (Gravity, AABB collisions, static/dynamic bodies)
- [x] Native C++ Scripting (`Script` & `ScriptRegistry`)
- [ ] `Forge Physics` v2 (Raycasts, Trigger zones, Capsule colliders)
- [ ] Audio system & In-Game HUD layer

### Phase 4 — Editor & Export
- [x] Dear ImGui 3D Editor (Viewport, Hierarchy, Inspector, Console)
- [x] Mouse Raycasting & 3D Gizmos (Move, Rotate, Scale)
- [x] Play / Pause / Stop simulation with scene backup
- [x] Editor Hotkeys (`1/2/3`, `Ctrl+S`, `Ctrl+D`, `Delete`, `F`)
- [ ] Content Browser (Asset Panel) & Undo/Redo
- [ ] Standalone Runtime Export (`Game.exe`)

---

## 📁 Project Structure

```text
ForgeEngine/
├── Engine/                     # Core engine static library
│   ├── include/Forge/          # Public headers (Core, Input, Physics, Renderer, Scene)
│   └── src/                    # Engine implementation files
├── Sandbox/                    # 3D Editor & sample game scripts
│   └── src/
│       ├── Editor/             # EditorUI, ViewportGizmo, SceneSerializer
│       └── Scripts/            # Sample C++ scripts (PlayerScript.h)
├── Assets/                     # Models, textures, and scenes
├── CMakeLists.txt              # Root CMake configuration
├── .gitignore
├── LICENSE
└── README.md

🚀 Building & Running (Windows / MinGW)
cmake -B build -G "MinGW Makefiles" "-DCMAKE_POLICY_VERSION_MINIMUM=3.5"
cmake --build build -j
.\build\bin\Sandbox.exe

🎮 Editor Controls
| Control | Action |
|---|---|
| RMB + WASD / Q / E | Fly camera in Viewport (hold Shift for 2× speed) |
| LMB Click | Select entity in Viewport |
| 1 / 2 / 3 | Switch Gizmo mode (Move / Rotate / Scale) |
| F | Focus camera on selected entity |
| Ctrl + D | Duplicate selected entity |
| Delete | Delete selected entity |
| Ctrl + S | Save scene (scene.forge) |
| ESC | Exit editor |
📄 License
This project is licensed under the MIT License — see the LICENSE file for details.
