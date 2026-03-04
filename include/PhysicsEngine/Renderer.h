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
        void init(PhysicsEngine::WorldBoundaries wb);
        void initRB2D(Rigidbody2D& body);
        void drawParticles(const std::vector<Particle>& particles);
        void drawContactPoints(const CollisionData& data);
        void drawRigidbody2D(const std::vector<Rigidbody2D>& rigidbody2D);
        void shutdown();

    private:
        unsigned int VAO, VBO, shaderProgram, debugVAO;
        unsigned int compileShader(unsigned int type, const char* source);
    };
}