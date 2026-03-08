#pragma once
#include "structs.h"
#include <vector>

namespace PhysicsEngine {
    struct Rigidbody2D {
        int ID;

        Vector2 position;
        Vector2 velocity;
        Vector2 acceleration;
        Vector2 force;

        float angPos;
        float angVel;
        float angAccn;
        float torque;

        float mass;
        float invMass;
        float momentInertia;
        float invMomentInertia;

        std::vector<PhysicsEngine::Vector2> localVertices;
        std::vector<PhysicsEngine::Vector2> worldVertices;
        std::vector<PhysicsEngine::Vector2> localNormals;
        std::vector<PhysicsEngine::Vector2> worldNormals;

        // rendering
        unsigned int VAO = 0; 
        unsigned int VBO = 0;

        Rigidbody2D(int id, float m, Vector2 pos, Vector2 vel, Vector2 accn, float moi, float angle, float w, float a, std::vector<PhysicsEngine::Vector2> lv) {
            ID = id;
            mass = m;
            invMass = (m > 0.0f) ? (1.0f / m) : 0.0f;
            position = pos;
            velocity = vel;
            acceleration = accn;
            momentInertia = moi;
            invMomentInertia = (moi > 0.0f) ? (1.0f / moi) : 0.0f;
            angPos = angle;
            angVel = w;
            angAccn = a;
            torque = 0;
            force = {0, 0};

            localVertices = lv;
            for (size_t i = 0; i < localVertices.size(); i++) {
                Vector2 p1 = localVertices[i];
                Vector2 p2 = localVertices[(i + 1) % localVertices.size()]; // handle 0 in loop
                Vector2 edge = { p2.x - p1.x, p2.y - p1.y };
                Vector2 n = { edge.y, -edge.x }; // normal
                
                float len = std::sqrt(n.x * n.x + n.y * n.y); // normalize
                localNormals.push_back({ n.x / len, n.y / len });
            }

            worldNormals.resize(localNormals.size());
            worldVertices.resize(localVertices.size());
            updateWorldVertices();
        }

        // static
        Rigidbody2D(Vector2 pos, float angle, std::vector<PhysicsEngine::Vector2> lv) {
            ID = -1;
            mass = 0;
            invMass = 0;
            position = pos;
            velocity = {0, 0};
            acceleration = {0, 0};
            momentInertia = 0;
            invMomentInertia = 0;
            angPos = angle;
            angVel = 0;
            angAccn = 0;
            torque = 0;
            force = {0, 0};

            localVertices = lv;
            for (size_t i = 0; i < localVertices.size(); i++) {
                Vector2 p1 = localVertices[i];
                Vector2 p2 = localVertices[(i + 1) % localVertices.size()]; // handle 0 in loop
                Vector2 edge = { p2.x - p1.x, p2.y - p1.y };
                Vector2 n = { edge.y, -edge.x }; // normal
                
                float len = std::sqrt(n.x * n.x + n.y * n.y); // normalize
                localNormals.push_back({ n.x / len, n.y / len });
            }

            worldNormals.resize(localNormals.size());
            worldVertices.resize(localVertices.size());
            updateWorldVertices();
        }
        
        void update(float dt, const PhysicsEngine::WorldBoundaries& wb);
        void updateWorldVertices();

        friend std::ostream& operator<<(std::ostream& os, const Rigidbody2D b) {
            os << "Rigidbody2D ID: " << b.ID;
            os << "\n    Mass: " << b.mass << "    Position: " << b.position << "    Velocity: " << b.velocity << "    Acceleration: " << b.acceleration;
            os << "\n    Inertia: " << b.momentInertia << "    Theta: " << b.angPos << "    Omega: " << b.angVel << "    Alpha: " << b.angAccn << "\n";
            return os;
        }
    };
}