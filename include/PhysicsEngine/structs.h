#pragma once

#include <math.h>
#include <vector>
#include <ostream>

namespace PhysicsEngine {
    struct WorldBoundaries {
        float left, right, bottom, top;
    };

    struct Vector2 {
        float x, y;

        Vector2 operator+(const Vector2& other) const {
            return { x + other.x, y + other.y };
        }

        void operator+=(const Vector2& other) {
            x += other.x;
            y += other.y;
        }

        Vector2 operator-(const Vector2& other) const {
            return { x - other.x, y - other.y };
        }

        Vector2 operator-() const {
            return { -x, -y };
        }

        void operator-=(const Vector2& other) {
            x -= other.x;
            y -= other.y;
        }
        
        Vector2 operator*(float scalar) const {
            return { x * scalar, y * scalar };
        }

        Vector2 operator/(float scalar) const {
            return { x / scalar, y / scalar };
        }

        friend std::ostream& operator<<(std::ostream& os, const Vector2& v) {
            os << "{" << v.x << ", " << v.y << "}";
            return os;
        }
    };

    struct ContactPoint {
        Vector2 position;
        float depth;

        friend std::ostream& operator<<(std::ostream& os, const ContactPoint& cp) {
            os << "ContactPoint[Position: " << cp.position << ", Depth: " << cp.depth << "]";
            return os;
        }
    };

    struct CollisionData {
        bool collided = false;
        float depth;
        Vector2 normal;
        std::vector<ContactPoint> contactPoint;

        friend std::ostream& operator<<(std::ostream& os, const CollisionData& cd) {
            os << "CollisionData[Collided: " << ((cd.collided) ? "true" : "false") << ", Depth: " << cd.depth << ", Normal: " << cd.normal << ", ContactPoints: {";
            for (int i = 0; i < cd.contactPoint.size(); i++) {
                if (i!=0) {
                    os << ", ";
                }
                os << cd.contactPoint[i];
            }
            os << "}]";
            return os;
        }
    };

    inline float Vector2Dot(Vector2 a, Vector2 b) {
        return a.x*b.x + a.y*b.y;
    }

    inline float Vector2Cross(Vector2 a, Vector2 b) {
        return a.x*b.y - a.y*b.x;
    }

    inline Vector2 Vector2Norm(Vector2 v) {
        float mag = sqrt(v.x*v.x + v.y*v.y);
        return v/mag;
    }

    struct Edge {
        Vector2 v1;
        Vector2 v2;
    };
}