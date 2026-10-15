#pragma once

#include <vector>
#include "structs.h"

namespace PhysicsEngine {
    namespace Shapes {  
        std::vector<PhysicsEngine::Vector2> Square = {{-1, -1}, {1, -1}, {1, 1}, {-1, 1}};
        std::vector<PhysicsEngine::Vector2> VWall = {{-1.0f, -6}, {1.0f, -6}, {1.0f, 6}, {-1.0f, 6}};
        std::vector<PhysicsEngine::Vector2> HWall = {{-8, -1.0f}, {8, -1.0f}, {8, 1.0f}, {-8, 1.0f}};
    }
}