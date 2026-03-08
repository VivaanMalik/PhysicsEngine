#include "PhysicsEngine/CollisionHandler.h"
#include "PhysicsEngine/structs.h"
#include "PhysicsEngine/Renderer.h"
#include "PhysicsEngine/Constants.h"
#include <limits>
#include <iostream>

namespace PhysicsEngine {
    std::vector<CollisionData> CollisionHandler::handleCollisions(std::vector<Rigidbody2D>& bodies, PhysicsEngine::Renderer renderer) {
        std::vector<CollisionData> datas;
        for (size_t i = 0; i < bodies.size(); ++i) {
            for (size_t j = i + 1; j < bodies.size(); ++j) {
                Rigidbody2D* a = &bodies[i];
                Rigidbody2D* b = &bodies[j];

                CollisionData data = checkSAT(a, b);
                if (data.collided) {
                    renderer.drawContactPoints(data);
                    datas.push_back(data);
                    resolveCollision(a, b, data);
                }
            }
        }
        return datas;
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

    std::vector<Vector2> CollisionHandler::clipEdgeFromNormal(Edge edge, Vector2 normal, Vector2 offset) {
        float dot1 =  PhysicsEngine::Vector2Dot(edge.v1-offset, normal);
        float dot2 =  PhysicsEngine::Vector2Dot(edge.v2-offset, normal);

        bool dirn1 = dot1>=0.0f;
        bool dirn2 = dot2>=0.0f;

        std::vector<Vector2> clippedPoints;
        if (dirn1) clippedPoints.push_back(edge.v1); 
        if (dirn2) clippedPoints.push_back(edge.v2);

        // atp either 0, 1, 2. If 1, we want intersection pt
        if (dirn1 != dirn2) {
            float t = dot1 / (dot1 - dot2);
            Vector2 intersectionPt = edge.v1 + (edge.v2 - edge.v1) * t;
            clippedPoints.push_back(intersectionPt);
        }

        return clippedPoints;
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

        float maxDot = 0.0f;
        int indx = -1;
        for (int i = 0; i < axes.size(); i++) {
            float dot = std::abs(PhysicsEngine::Vector2Dot(collisionNormal, axes[i]));
            if (dot>maxDot) {
                maxDot = dot;
                indx = i;
            }
        }
        
        cData.collided = true;
        cData.depth = minOverlap;

        Vector2 bodyAToB = b->position - a->position;
        if (PhysicsEngine::Vector2Dot(collisionNormal, bodyAToB) < 0) {
            collisionNormal = collisionNormal * -1.0f;
        }

        Rigidbody2D* ref;
        Rigidbody2D* inc;
        bool flipped = false;

        float maxDotA = 0;
        for (Vector2 n : a->worldNormals) {
            float dot = std::abs(PhysicsEngine::Vector2Dot(collisionNormal, n));
            if (dot > maxDotA) maxDotA = dot;
        }
        float maxDotB = 0;
        for (Vector2 n : b->worldNormals) {
            float dot = std::abs(PhysicsEngine::Vector2Dot(collisionNormal, n));
            if (dot > maxDotB) maxDotB = dot;
        }

        if (maxDotA >= maxDotB) {
            ref = a; inc = b;
        } else {
            ref = b; inc = a;
            flipped = true; 
        }
        
        Vector2 refNormal = flipped ? -collisionNormal : collisionNormal;
        cData.normal = collisionNormal;

        // clipping and poc
        Edge refEdge = getBestEdge(ref->worldNormals, ref->worldVertices, refNormal);
        Edge incEdge = getBestEdge(inc->worldNormals, inc->worldVertices, -refNormal);

        Vector2 v1 = refEdge.v1;
        Vector2 v2 = refEdge.v2;
        Vector2 edgeDirn = PhysicsEngine::Vector2Norm(v2 - v1);

        std::vector<Vector2> clippedPoints = clipEdgeFromNormal(incEdge, edgeDirn, v1);
        if (clippedPoints.size()==0) return cData;

        clippedPoints = clipEdgeFromNormal({clippedPoints[0], clippedPoints[1]}, -edgeDirn, v2);
        if (clippedPoints.size()==0) return cData;

        clippedPoints = clipEdgeFromNormal({clippedPoints[0], clippedPoints[1]}, -collisionNormal, v1);

        for (int i = 0; i < clippedPoints.size(); i++) {
            cData.contactPoint.push_back({clippedPoints[i], -(PhysicsEngine::Vector2Dot(clippedPoints[i]-v1, refNormal))});
        }

        return cData;
    }

    void CollisionHandler::resolveCollision(Rigidbody2D* a, Rigidbody2D* b, CollisionData collisionData) {
        for (int i = 0; i < collisionData.contactPoint.size(); i++) {
            Vector2 ra = collisionData.contactPoint[i].position-a->position;
            Vector2 rb = collisionData.contactPoint[i].position-b->position;
            Vector2 va = a->velocity+(((Vector2){-ra.y, ra.x})*a->angVel);
            Vector2 vb = b->velocity+(((Vector2){-rb.y, rb.x})*b->angVel);
            Vector2 vrel = vb-va;

            if (PhysicsEngine::Vector2Dot(vrel, collisionData.normal) > 0) continue;;

            float e = .8f;

            float j =  -(1+e)*PhysicsEngine::Vector2Dot(vrel, collisionData.normal);
            float raxn = Vector2Cross(ra, collisionData.normal);
            float rbxn = Vector2Cross(rb, collisionData.normal);
            float denominator = (a->invMass+b->invMass+(raxn*raxn*a->invMomentInertia)+(rbxn*rbxn*b->invMomentInertia));
            if (denominator<PhysicsEngine::Constants::EPSILON) continue;
            j/=denominator;
            j/=(float)collisionData.contactPoint.size();
            if (std::isnan(j) || std::isinf(j)) continue;
            Vector2 Impulse = collisionData.normal * j;
            a->velocity-=Impulse*a->invMass;
            a->angVel-=PhysicsEngine::Vector2Cross(ra, Impulse)*a->invMomentInertia;
            b->velocity+=Impulse*b->invMass;
            b->angVel+=PhysicsEngine::Vector2Cross(rb, Impulse)*b->invMomentInertia;

            const float percent = 0.5f;
            const float slop = 0.0001f;
            if (collisionData.contactPoint[i].depth > slop) {
                float correctionMag = (collisionData.contactPoint[i].depth / denominator) * percent / (float)collisionData.contactPoint.size();
                Vector2 correctionVec = collisionData.normal * correctionMag;
                a->position -= correctionVec * a->invMass;
                a->angPos   -= raxn * correctionMag * a->invMomentInertia;
                b->position += correctionVec * b->invMass;
                b->angPos   += rbxn * correctionMag * b->invMomentInertia;
            }
        }
    }
}