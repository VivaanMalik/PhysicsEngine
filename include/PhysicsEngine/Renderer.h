#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Particle.h"
#include "Rigidbody2D.h"
#include "structs.h"

namespace PhysicsEngine {
    class Renderer {
    public:
        glm::mat4 projection;
        void init(PhysicsEngine::WorldBoundaries wb, Vector2 screen);
        void initRB2D(Rigidbody2D& body);
        void drawParticles(const std::vector<Particle>& particles);
        void drawContactPoints(const CollisionData& data);
        void drawRigidbody2D(const std::vector<Rigidbody2D>& rigidbody2D, bool labelVertices = false);
        void shutdown();

    private:
        Vector2 worldToPixels(Vector2 worldcoord);
        unsigned int VAO, VBO, shaderProgram, debugVAO;
        unsigned int compileShader(unsigned int type, const char* source);
        PhysicsEngine::WorldBoundaries world;
        Vector2 windowsize;
    };
}