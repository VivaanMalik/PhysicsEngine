#pragma once
#include <vector>
#include "structs.h" 
#include "Renderer.h"
#include "Rigidbody2D.h"

namespace PhysicsEngine {
    class CollisionHandler {
    public:
        bool handleCollisions(std::vector<Rigidbody2D>& bodies, PhysicsEngine::Renderer renderer);

        CollisionData checkSAT(Rigidbody2D* a, Rigidbody2D* b);
    private:
        Vector2 projectPolygon(const std::vector<Vector2>& vertices, Vector2 axis);
        std::vector<Vector2> clip(const std::vector<Vector2>& points, Vector2 normal, float offset);      
        Edge getBestEdge(const std::vector<Vector2>& normals, const std::vector<Vector2>& vertices, Vector2 targetnormal);
        // void resolveCollision(Rigidbody2D* a, Rigidbody2D* b, const CollisionData& data);
    };
}