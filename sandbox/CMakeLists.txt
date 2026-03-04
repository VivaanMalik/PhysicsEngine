# 1. CREATE THE EXECUTABLE
add_executable(SandboxApp main.cpp)

# 2. LINK IT TO THE ENGINE
target_link_libraries(SandboxApp PRIVATE PhysicsEngine)