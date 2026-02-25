# Interactive 3D Gothic Environment 🦇

## Overview
An exploration of photorealism concepts in interactive 3D environments using OpenGL. The project renders a complex medieval scene featuring a gothic castle, a moonlit sky, and atmospheric effects, blending technical precision with artistic design to produce a realistic nocturnal atmosphere.

## Key Technical Features
* **Advanced Navigation System:** Implementation of a 3D camera with WASD controls and mouse-based orientation using Euler angles (Yaw/Pitch).
* **Realistic Lighting (Phong Model):** * **Directional Light:** Simulating moonlight across the entire scene.
    * **Point Lights:** Strategically placed lanterns with quadratic attenuation to simulate realistic light decay.
* **Real-time Particle System:** A rain simulation managing a pool of 2000 independent particles with unique velocities and reset mechanisms.
* **Atmospheric Effects:** * **Exponential Fog:** Distance-based fog implementation to enhance depth and mood.
    * **Skybox:** High-resolution cube map for an immersive starry night background.
* **Dynamic Rendering Modes:** Capability to toggle between Solid, Wireframe, and Pointform rendering modes for debugging and visualization.

## Tech Stack
* **Language:** C++
* **Graphics API:** OpenGL
* **Libraries:** GLFW (window management), GLM (mathematics), GLEW
* **IDE:** Visual Studio

## Project Structure
* `src/` - C++ source files and headers (Camera, Mesh, Model3D, Shader, SkyBox)
* `shaders/` - Vertex and Fragment shaders for lighting, fog, and shadows
* `models/` - 3D assets in .obj and .mtl format
* `docs/` - Technical documentation and architectural overview (PDF)

## How to Run
1. Open the `project.sln` file in Visual Studio.
2. Ensure the dependencies (GLFW, GLM) are correctly linked in the project properties.
3. Build and Run the solution in **Release** mode for optimal performance.
4. **Controls:** Use `W`, `A`, `S`, `D` to move, `Mouse` to look around, and specific keys (as defined in the documentation) to toggle rain or rendering modes.
