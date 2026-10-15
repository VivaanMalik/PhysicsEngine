# Physics Engine

A 2D rigid-body physics engine written in **C++20**, featuring polygon collision detection, impulse-based collision response, rotational dynamics, and real-time OpenGL visualization.

The project implements the core physics and collision pipeline directly, while using OpenGL and supporting libraries for rendering, window management, mathematics, and debug overlays.

## Features

- **Rigid-body dynamics:** Linear and angular position, velocity, acceleration, force, and torque.
- **Polygon geometry:** Convex polygon bodies represented by local and world-space vertices and edge normals.
- **Collision detection:** Separating Axis Theorem (SAT), testing axes derived from both polygons.
- **Contact generation:** Reference/incident edge selection and edge clipping to generate contact points and penetration depths.
- **Collision response:** Impulse-based resolution that accounts for linear and angular motion, mass, and moment of inertia.
- **Static bodies:** Zero-mass rigid bodies for walls and other immovable geometry.
- **Particle simulation:** Particle position and velocity updates under acceleration and applied forces.
- **OpenGL rendering:** GPU-backed rendering of particles and polygon outlines, with configurable world-to-screen projection.
- **Debug visualization:** Contact points, vertex indices, and polygon normals.
- **Simulation controls:** Pause/resume, single-step simulation, and keyboard-driven exit.
- **Fixed-step physics:** Physics updates use a fixed timestep of 1/120 second.

## Physics and collision pipeline

### 1. Motion integration

Particles and rigid bodies store their physical state and update it over a timestep. Rigid bodies integrate linear and angular motion using acceleration from their stored acceleration, applied force, and torque.

For a body with mass \(m\) and moment of inertia \(I\):

- Force-derived linear acceleration: \(a_F = F/m\)
- Torque-derived angular acceleration: \(\alpha_\tau = \tau/I\)

The implementation uses inverse mass and inverse moment of inertia, with zero values for static bodies.

### 2. Polygon transformations

Each rigid body stores its geometry in local coordinates. Its world-space vertices and normals are updated using the body's rotation and translation, allowing collision detection to operate on the transformed geometry.

### 3. Separating Axis Theorem

For each pair of bodies, the collision handler tests the edge normals of both polygons. The polygons are projected onto each candidate axis.

If any axis has disjoint projection intervals, the polygons do not intersect. Otherwise, the smallest overlap provides a candidate penetration depth and collision normal.

### 4. Contact generation

The collision handler selects reference and incident edges and clips the incident edge against the reference edge's side planes. The resulting points are used as contact points for collision response and can be visualized by the renderer.

### 5. Impulse-based response

At each contact, the solver computes relative velocity, including the contribution from angular velocity. It calculates a normal impulse using the bodies' inverse masses and inverse moments of inertia, then applies the resulting linear and angular velocity changes.

The current collision response uses a restitution coefficient of 0.8.

## Tech stack

| Technology | Role |
|---|---|
| C++20 | Physics engine and simulation code |
| OpenGL 3.3 | Real-time rendering |
| GLFW | Window and input management |
| GLAD | OpenGL function loading |
| GLM | Projection and transformation matrices |
| Dear ImGui | Debug overlay and visualization |
| CMake | Build configuration |

The physics types, motion integration, polygon transformations, SAT collision detection, contact generation, and impulse response are implemented in the project. The graphics and windowing stack is provided by the libraries above.

## Project structure

```text
PhysicsEngine/
├── include/
│   └── PhysicsEngine/
│       ├── CollisionHandler.h  # Collision detection and resolution interface
│       ├── Constants.h         # Physics and rendering constants
│       ├── Particle.h          # Particle state and update interface
│       ├── Renderer.h          # OpenGL renderer interface
│       ├── Rigidbody2D.h       # Rigid-body state and polygon geometry
│       ├── Shapes.h            # Polygon and wall geometry
│       └── structs.h           # Vector2, contacts, collision data, boundaries
├── src/
│   ├── CollisionHandler.cpp    # SAT, contact generation, collision response
│   ├── Particle.cpp            # Particle integration and boundaries
│   ├── Renderer.cpp            # OpenGL rendering and debug overlay
│   └── Rigidbody2D.cpp         # Linear/angular integration and transforms
├── shaders/
│   ├── basic.vert              # Vertex shader
│   └── basic.frag              # Fragment shader
├── sandbox/
│   ├── CMakeLists.txt          # Sandbox executable target
│   └── main.cpp                # Simulation entry point and demo scene
├── vendor/                     # Third-party graphics dependencies
├── CMakeLists.txt              # Engine library and project build
├── buildrun.sh                 # Configure, build, and launch helper
└── imgui.ini                   # Saved ImGui window layout
```

## Requirements

- A C++ compiler with C++20 support
- CMake 3.15 or newer
- Git
- A desktop graphics environment that supports OpenGL 3.3

GLFW, GLAD, GLM, and Dear ImGui are included under `vendor/`. On Linux, the build may also require system development packages needed by GLFW for your windowing system, such as X11 or Wayland development headers.

## Build and run

Clone the repository:

```bash
git clone https://github.com/VivaanMalik/PhysicsEngine.git
cd PhysicsEngine
```

The repository includes a helper script intended to configure the CMake build, compile the sandbox executable, and launch it:

```bash
chmod +x buildrun.sh
./buildrun.sh
```

Alternatively, build manually:

```bash
cmake -S . -B build
cmake --build build --target SandboxApp
```

Run the executable from the repository root so that the renderer can find the shader files at `shaders/basic.vert` and `shaders/basic.frag`:

```bash
./build/sandbox/SandboxApp
```

If your generator places the executable elsewhere, use the corresponding path under `build/`.

## Controls

| Key | Action |
|---|---|
| `Space` | Pause or resume the simulation |
| `Right Arrow` | Advance one physics step while paused |
| `Esc` | Close the application |

## Current scope and limitations

This repository is a work in progress and currently focuses on 2D polygon rigid bodies and particle motion. The demo scene initializes dynamic square bodies and static wall geometry.

The current implementation should not be interpreted as a complete production physics engine. In particular:

- Collision handling checks body pairs directly, so pair checks grow quadratically with the number of bodies.
- A general-purpose broad-phase collision system is not currently included.
- A configurable friction/material model is not currently exposed.
- A comprehensive automated test suite is not currently included.

## License

No repository-level license is currently specified. Check the licenses of the vendored dependencies before redistributing the project.
