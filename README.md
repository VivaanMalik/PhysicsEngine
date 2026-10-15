# Physics Engine

A 2D rigid-body physics engine written in **C++20**, featuring polygon collision detection, impulse-based collision response, rotational dynamics, and real-time OpenGL visualization.

The project implements the core physics and collision pipeline, with OpenGL and supporting libraries used for rendering, window management, mathematics, and debug overlays.

## Features

- **Rigid-body dynamics:** Linear and angular position, velocity, acceleration, force, and torque.
- **Polygon geometry:** Convex polygon bodies represented by local and world-space vertices and edge normals.
- **Collision detection:** Separating Axis Theorem (SAT), testing axes derived from both polygons.
- **Contact generation:** Reference/incident edge selection and clipping to generate contact points and penetration depths.
- **Collision response:** Impulse-based resolution accounting for linear and angular motion, mass, and moment of inertia.
- **Static bodies:** Zero-mass rigid bodies for walls and other immovable geometry.
- **Particle simulation:** Particle motion under acceleration and applied forces.
- **OpenGL rendering:** Real-time rendering of particles and polygon outlines.
- **Debug visualization:** Contact points, vertex indices, and polygon normals.
- **Simulation controls:** Pause/resume, single-step simulation, and keyboard-driven exit.
- **Fixed-step physics:** Physics updates use a fixed timestep of $1/120$ second.

## Physics and Collision Pipeline

### 1. Motion Integration

Particles and rigid bodies update their physical state over a timestep. Rigid bodies integrate linear and angular motion using acceleration from applied forces and torques.

For a body with mass $m$, moment of inertia $I$, force $F$, and torque $\tau$:

$$
a = \frac{F}{m}, \qquad \alpha = \frac{\tau}{I}
$$

The implementation uses inverse mass and inverse moment of inertia, with zero values for static bodies.

### 2. Polygon Transformations

Each rigid body stores its geometry in local coordinates. Its world-space vertices and normals are updated using the body's rotation and translation, allowing collision detection to operate on transformed geometry.

### 3. Separating Axis Theorem

For each pair of bodies, the collision handler tests the edge normals of both polygons. The polygons are projected onto each candidate axis.

If any axis has disjoint projection intervals, the polygons do not intersect. Otherwise, the smallest overlap provides a candidate penetration depth and collision normal.

### 4. Contact Generation

The collision handler selects reference and incident edges and clips the incident edge against the reference edge's side planes. The resulting points become contact points for collision response and can be visualized by the renderer.

### 5. Impulse-Based Response

At each contact, the solver computes relative velocity, including contributions from angular velocity. It calculates a normal impulse using the bodies' inverse masses and inverse moments of inertia, then applies the resulting linear and angular velocity changes.

The current collision response uses a restitution coefficient of $0.8$.

## Tech Stack

| Technology | Role |
|---|---|
| C++20 | Physics engine and simulation code |
| OpenGL 3.3 | Real-time rendering |
| GLFW | Window and input management |
| GLAD | OpenGL function loading |
| GLM | Mathematics and transformation matrices |
| Dear ImGui | Debug overlay and visualization |
| CMake | Build configuration |

The physics types, motion integration, polygon transformations, SAT collision detection, contact generation, and impulse response are implemented in the project.

## Project Structure

```text
PhysicsEngine/
├── include/
│   └── PhysicsEngine/
│       ├── CollisionHandler.h
│       ├── Constants.h
│       ├── Particle.h
│       ├── Renderer.h
│       ├── Rigidbody2D.h
│       ├── Shapes.h
│       └── structs.h
├── src/
│   ├── CollisionHandler.cpp
│   ├── Particle.cpp
│   ├── Renderer.cpp
│   └── Rigidbody2D.cpp
├── shaders/
│   ├── basic.vert
│   └── basic.frag
├── sandbox/
│   ├── CMakeLists.txt
│   └── main.cpp
├── vendor/
├── CMakeLists.txt
├── buildrun.sh
└── imgui.ini
```

### Key Files

| File | Responsibility |
|---|---|
| `CollisionHandler.cpp` | SAT collision detection, contact generation, and collision response |
| `Rigidbody2D.cpp` | Rigid-body dynamics and polygon transformations |
| `Particle.cpp` | Particle integration and boundary handling |
| `Renderer.cpp` | OpenGL rendering and debug visualization |
| `Shapes.h` | Polygon and wall geometry |
| `sandbox/main.cpp` | Simulation entry point and demo scene |
| `shaders/` | Vertex and fragment shaders |

## Requirements

- C++ compiler with C++20 support
- CMake 3.15 or newer
- Git
- Desktop graphics environment supporting OpenGL 3.3

The repository includes graphics dependencies under `vendor/`. On Linux, system development packages required by GLFW may also be necessary.

## Build and Run

Clone the repository:

```bash
git clone https://github.com/VivaanMalik/PhysicsEngine.git
cd PhysicsEngine
```

The repository includes a helper script for configuring, building, and launching the application:

```bash
chmod +x buildrun.sh
./buildrun.sh
```

Alternatively, build using CMake:

```bash
cmake -S . -B build
cmake --build build --target SandboxApp
```

Run the executable from the repository root so the application can locate its shader files:

```bash
./build/sandbox/SandboxApp
```

The executable's location may vary depending on the CMake generator and target configuration.

## Controls

| Key | Action |
|---|---|
| `Space` | Pause or resume the simulation |
| `Right Arrow` | Advance one physics step while paused |
| `Esc` | Exit the application |

## Current Scope and Limitations

The project focuses on 2D polygon rigid bodies and particle motion. The demo initializes dynamic square bodies and static wall geometry.

Current limitations include:

- Collision handling checks body pairs directly, resulting in $O(N^2)$ pair checks for $N$ bodies.
- A general-purpose broad-phase collision system is not included.
- A configurable friction and material model is not currently exposed.
- A comprehensive automated test suite is not currently included.

## License

No repository-level license is currently specified. Check the licenses of the vendored dependencies before redistributing the project.