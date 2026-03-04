#pragma once

namespace PhysicsEngine {
    namespace Constants {
        
        // --- 1. MATH CONSTANTS ---
        constexpr float PI       = 3.14159265359f;
        constexpr float TAU      = 6.28318530718f;
        constexpr float SQRT_2   = 1.41421356237f;
        constexpr float DEG2RAD  = PI / 180.0f;
        constexpr float RAD2DEG  = 180.0f / PI;

        // --- 2. PHYSICS CONSTANTS ---
        constexpr float GRAVITY_EARTH = -9.81f;
        constexpr float GRAVITY_MOON  = -1.62f;
        constexpr float GRAVITY_ZERO  =  0.0f;
        
        // Universal Gravitational Constant (if you build planetary physics)
        // Formula: F = G * ((m1 * m2) / r^2)
        constexpr float G = 6.67430e-11f; 
        constexpr float EPSILON = 0.0001f;
        constexpr float WALL_COR = .9f;

        constexpr float LBOUNDARY = -0.99f;
        constexpr float RBOUNDARY = 0.99f;
        constexpr float BBOUNDARY = -0.99f;
        constexpr float UBOUNDARY = 0.99f;
        constexpr float OFFSET = 0.001f; 

        constexpr float COLORR = 0.74f; 
        constexpr float COLORG = 0.01f; 
        constexpr float COLORB = 1.0f; 

        constexpr float THICCSIZE = 100.0f; 
        constexpr float THINSIZE = 25.0f; 
    }
}