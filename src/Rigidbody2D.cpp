#include "PhysicsEngine/Rigidbody2D.h"
#include "PhysicsEngine/Constants.h"
#include "PhysicsEngine/structs.h"
#include <math.h>

namespace PhysicsEngine {
    void Rigidbody2D::update(float dt, const PhysicsEngine::WorldBoundaries& bounds) {
        Vector2 forceaccn = force * invMass;
        Vector2 netaccn = acceleration+forceaccn;

        float angForceaccn = torque * invMomentInertia;
        float angNetaccn = angAccn + angForceaccn;

        velocity+=netaccn*dt;
        position+=velocity*dt;

        angVel+=angNetaccn*dt;
        angPos+=angVel*dt;

        if (velocity.x > PhysicsEngine::Constants::EPSILON || velocity.y > PhysicsEngine::Constants::EPSILON || angVel > PhysicsEngine::Constants::EPSILON) {
            updateWorldVertices();
        }

        force = {0, 0};
        torque = 0;

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

    void Rigidbody2D::updateWorldVertices() {
        float cosA = cos(angPos);
        float sinA = sin(angPos);

        for (size_t i = 0; i < localVertices.size(); ++i) {
            worldVertices[i] = {localVertices[i].x * cosA - localVertices[i].y * sinA, localVertices[i].x * sinA + localVertices[i].y * cosA};
            worldNormals[i] = {localNormals[i].x * cosA - localNormals[i].y * sinA, localNormals[i].x * sinA + localNormals[i].y * cosA};

            worldVertices[i] += position;
        }
    }
}