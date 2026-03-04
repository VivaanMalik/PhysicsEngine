cmake_minimum_required(VERSION 3.15)
project(MyPhysicsEngine VERSION 1.0 LANGUAGES CXX C) # Note: Added 'C' for glad.c

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- 1. VENDOR LIBRARIES ---
# Turn off GLFW's extra garbage so it compiles faster
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/glfw)

# --- 2. THE ENGINE LIBRARY ---
# Grab all the engine .cpp files
file(GLOB_RECURSE ENGINE_SOURCES "src/*.cpp")
# Manually add the GLAD .c file so it compiles with our engine
list(APPEND ENGINE_SOURCES "vendor/glad/src/glad.c")

add_library(PhysicsEngine STATIC ${ENGINE_SOURCES})

# --- 3. INCLUDE DIRECTORIES ---
# Tell the engine where to find all the vendor headers
target_include_directories(PhysicsEngine PUBLIC 
    "include"
    "vendor/glm"
    "vendor/glad/include"
    "vendor/glfw/include"
)

# --- 4. LINKING ---
# Glue GLFW to our engine
target_link_libraries(PhysicsEngine PUBLIC glfw)

# --- 5. THE SANDBOX ---
add_subdirectory(sandbox)