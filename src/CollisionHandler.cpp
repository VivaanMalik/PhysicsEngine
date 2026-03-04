#include "PhysicsEngine/CollisionHandler.h"
#include "PhysicsEngine/structs.h"
#include "PhysicsEngine/Renderer.h"
#include <limits>

namespace PhysicsEngine {
    bool CollisionHandler::handleCollisions(std::vector<Rigidbody2D>& bodies, PhysicsEngine::Renderer renderer) {
        bool collision = false;
        for (size_t i = 0; i < bodies.size(); ++i) {
            for (size_t j = i + 1; j < bodies.size(); ++j) {
                Rigidbody2D* a = &bodies[i];
                Rigidbody2D* b = &bodies[j];

                CollisionData data = checkSAT(a, b);
                if (data.collided) {
                    renderer.drawContactPoints(data);
                    collision = true;
                    // resolveCollision(a, b, data);
                }
            }
        }
        return collision;
    }

    Vector2 CollisionHandler::projectPolygon(const std::vector<Vector2>& vertices, Vector2 axis) {
        float min = PhysicsEngine::Vector2Dot(vertices[0], axis);
        float max = min;
        for (Vector2 v : vertices) {
            float p = PhysicsEngine::Vector2Dot(v, axis);
            if (p<min) min=p;
            else if (p>max) max=p;
        }
        return {min, max};
    }

    std::vector<Vector2> CollisionHandler::clip(const std::vector<Vector2>& points, Vector2 normal, float offset) {
        std::vector<Vector2> out;
        if (points.empty()) return out;
        for (size_t i = 0; i < points.size(); ++i) {
            Vector2 v1 = points[i];
            Vector2 v2 = points[(i + 1) % points.size()]; // Loop back to start for closed shapes

            float d1 = PhysicsEngine::Vector2Dot(normal, v1) - offset;
            float d2 = PhysicsEngine::Vector2Dot(normal, v2) - offset;

            // If the start point is "inside" the plane, keep it
            if (d1 >= 0) out.push_back(v1);

            // If the segment crosses the plane, calculate and add the intersection point
            if (d1 * d2 < 0) {
                float t = d1 / (d1 - d2);
                Vector2 intersection = v1 + (v2 - v1) * t;
                out.push_back(intersection);
            }
        }
        return out;
    }

    Edge CollisionHandler::getBestEdge(const std::vector<Vector2>& normals, const std::vector<Vector2>& vertices, Vector2 targetnormal) {
        float maxDot = -std::numeric_limits<float>::infinity();
        int indx = 0;
        int len = normals.size();

        for (int i = 0; i < len; i++) {
            float dot = PhysicsEngine::Vector2Dot(normals[i], targetnormal);

            if (dot>maxDot) {
                maxDot = dot;
                indx = i;
            }

        }
        return {vertices[indx], vertices[(indx+1)%len]};
    }

    CollisionData CollisionHandler::checkSAT(Rigidbody2D* a, Rigidbody2D* b) {
        PhysicsEngine::CollisionData cData;
        std::vector<Vector2> axes = a->worldNormals;
        axes.insert(axes.end(), b->worldNormals.begin(), b->worldNormals.end());
        
        float minOverlap = std::numeric_limits<float>::infinity(); // same as depth
        Vector2 collisionNormal;

        for (Vector2 axis : axes) {
            Vector2 aProj = projectPolygon(a->worldVertices, axis);
            Vector2 bProj = projectPolygon(b->worldVertices, axis);

            float overlap = std::min(aProj.y, bProj.y) - std::max(aProj.x, bProj.x); // min(max) - max(min)
            if (overlap<0.0f) return cData;

            if (overlap < minOverlap) {
                minOverlap = overlap;
                collisionNormal = axis;

            }
        }
        
        cData.collided = true;
        cData.depth = minOverlap;

        // ensures collision normal in dirn of a to b
        Vector2 direction = b->position - a->position;
        if (PhysicsEngine::Vector2Dot(direction, collisionNormal) < 0) {
            collisionNormal = collisionNormal * -1.0f;
        }

        cData.normal = collisionNormal;

        // clipping and poc
        Edge refEdge = getBestEdge(a->worldNormals, a->worldVertices, collisionNormal);
        Edge incEdge = getBestEdge(b->worldNormals, b->worldVertices, -collisionNormal);

        Vector2 v1 = refEdge.v1;
        Vector2 v2 = refEdge.v2;
        Vector2 edgeDirn = PhysicsEngine::Vector2Norm(v2 - v1);

        std::vector<Vector2> clippedPoints = {incEdge.v1, incEdge.v2};

        clippedPoints = clip(clippedPoints, -edgeDirn, PhysicsEngine::Vector2Dot(-edgeDirn, v1));
        if (clippedPoints.size() < 2) return cData;

        clippedPoints = clip(clippedPoints, edgeDirn, PhysicsEngine::Vector2Dot(edgeDirn, v2));
        if (clippedPoints.size() < 2) return cData;

        Vector2 refNormal = {-edgeDirn.y, edgeDirn.x}; // The normal of the edge itself
        float maxDepth = PhysicsEngine::Vector2Dot(refNormal, v1);

        for (Vector2 p : clippedPoints) {
            float depth = PhysicsEngine::Vector2Dot(refNormal, p) - maxDepth;
            if (depth <= 0) {
                cData.contactPoint.push_back({p, depth});
            }
        }
        return cData;
    }
}