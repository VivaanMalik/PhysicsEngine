#pragma once
#include "structs.h"
#include <ostream>

namespace PhysicsEngine {
    struct Particle {
        Vector2 position;
        Vector2 velocity;
        Vector2 acceleration;
        Vector2 force;

        float mass;
        float invMass;

        Particle(float m, Vector2 pos, Vector2 vel, Vector2 accn) {
            mass = m;
            invMass = (m > 0.0f) ? (1.0f / m) : 0.0f;
            position = pos;
            velocity = vel;
            acceleration = accn;
            force = {0, 0};
        }
        
        void update(float dt, const PhysicsEngine::WorldBoundaries& wb);

        friend std::ostream& operator<<(std::ostream& os, const Particle& p) {
            os << "Particle[M: " << p.mass << ", Position: " << p.position << ", Velocity: " << p.velocity << ", Acceleration: " << p.acceleration << "]";
            return os;
        }
    };
}