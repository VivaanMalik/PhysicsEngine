#include "PhysicsEngine/Particle.h"
#include "PhysicsEngine/Constants.h"
#include "PhysicsEngine/structs.h"

namespace PhysicsEngine {
    void Particle::update(float dt, const PhysicsEngine::WorldBoundaries& bounds) {
        Vector2 forceaccn = force * invMass;
        Vector2 netaccn = acceleration+forceaccn;

        velocity+=netaccn*dt;
        position+=velocity*dt;

        force = {0, 0};

        // jugaad collision 
        // opengl boundary iz -1.0 to 1.0
        if (position.y < bounds.bottom) {
            position.y = bounds.bottom+Constants::OFFSET;
            velocity.y *= -1*Constants::WALL_COR; // coeff of restitution
        }
        else if (position.y > bounds.top) {
            position.y = bounds.top-Constants::OFFSET;
            velocity.y *= -1*Constants::WALL_COR; // coeff of restitution
        }
        else if (position.x > bounds.right) {
            position.x = bounds.right-Constants::OFFSET;
            velocity.x *= -1*Constants::WALL_COR; // coeff of restitution
        }
        else if (position.x < bounds.left) {
            position.x = bounds.left+Constants::OFFSET;
            velocity.x *= -1*Constants::WALL_COR; // coeff of restitution
        }
    }
}