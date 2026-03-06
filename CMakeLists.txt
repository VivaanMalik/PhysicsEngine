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

file(GLOB_RECURSE ENGINE_SOURCES "src/*.cpp")
list(APPEND ENGINE_SOURCES "vendor/glad/src/glad.c")

# ADD IMGUI SOURCE FILES
list(APPEND ENGINE_SOURCES 
    "vendor/imgui/imgui.cpp"
    "vendor/imgui/imgui_draw.cpp"
    "vendor/imgui/imgui_widgets.cpp"
    "vendor/imgui/imgui_tables.cpp"
    "vendor/imgui/backends/imgui_impl_glfw.cpp"
    "vendor/imgui/backends/imgui_impl_opengl3.cpp"
)

add_library(PhysicsEngine STATIC ${ENGINE_SOURCES})

# --- 3. INCLUDE DIRECTORIES ---
target_include_directories(PhysicsEngine PUBLIC 
    "include"
    "vendor/glm"
    "vendor/glad/include"
    "vendor/glfw/include"
    "vendor/imgui"           # Main ImGui headers
    "vendor/imgui/backends"  # Backend headers
)

target_link_libraries(PhysicsEngine PUBLIC glfw)

# --- 5. THE SANDBOX ---
add_subdirectory(sandbox)