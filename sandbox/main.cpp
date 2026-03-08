#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <chrono>
#include "PhysicsEngine/Particle.h"
#include "PhysicsEngine/Rigidbody2D.h"
#include "PhysicsEngine/Renderer.h"
#include "PhysicsEngine/CollisionHandler.h"
#include "PhysicsEngine/structs.h"
#include "PhysicsEngine/Constants.h"
#include "PhysicsEngine/Shapes.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    PhysicsEngine::Vector2 screen = {800, 600};

    GLFWwindow* window = glfwCreateWindow(screen.x, screen.y, "My Physics Engine", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    bool unlockFPS = false;
    const float fixedDeltaTime = 1.0f / 120.0f;

    int frameWidth, frameHeight;
    glfwGetFramebufferSize(window, &frameWidth, &frameHeight);
    glViewport(0, 0, frameWidth, frameHeight);
    glfwSwapInterval(0);

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    PhysicsEngine::Renderer renderer;
    PhysicsEngine::CollisionHandler collisionHandler;

    PhysicsEngine::WorldBoundaries world = { 0.0f, 16.0f, 0.0f, 12.0f}; // l r b m
    renderer.init(world, screen);

    std::vector<PhysicsEngine::Particle> particles = {
        {1, {0, 8}, {2, 0}, {0, PhysicsEngine::Constants::GRAVITY_EARTH}},
        {1, {4, 4}, {5, 3}, {0, PhysicsEngine::Constants::GRAVITY_EARTH}},
        {1, {8, 1}, {0, 0}, {-1, -1}}
    };

    std::vector<PhysicsEngine::Rigidbody2D> bodies = {
        // wb
        {{-1, 6}, 0, PhysicsEngine::Shapes::VWall},
        {{17, 6}, 0, PhysicsEngine::Shapes::VWall},
        {{8, -1}, 0, PhysicsEngine::Shapes::HWall},
        {{8, 13}, 0, PhysicsEngine::Shapes::HWall},
        // wb
        {0, 1, {4, 5}, {10, 10}, {0, PhysicsEngine::Constants::GRAVITY_EARTH}, 14, 45*PhysicsEngine::Constants::DEG2RAD, -2, 0, PhysicsEngine::Shapes::Square},
        {1, 1, {10, 8}, {0, 0}, {0, PhysicsEngine::Constants::GRAVITY_EARTH}, 1, 0, 1, 0, PhysicsEngine::Shapes::Square},
        {2, 1, {2, 10}, {0, 0}, {0, PhysicsEngine::Constants::GRAVITY_EARTH}, 1, 0, 0.1, 0, PhysicsEngine::Shapes::Square}
        // {1, {1.1f, 6}, {-.01f, 0}, {0, PhysicsEngine::Constants::GRAVITY_EARTH}, 1, .01f, 0, 0, PhysicsEngine::Shapes::Square}
    };

    for (auto& b : bodies) {
        renderer.initRB2D(b);
    }

    // float deltaTime = 0.016f; // 60fps
    bool isPaused = true;
    bool spacePressedLastFrame = false;
    bool rightArrowPressedLastFrame = false;
    float accumulator = 0.0f;
    auto lastTime = std::chrono::high_resolution_clock::now();
    std::vector<PhysicsEngine::CollisionData> cDatas;

    while (!glfwWindowShouldClose(window)) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        // if (frameTime > 0.1f) frameTime = 0.1f;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        bool spacePressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
        bool rightArrowPressed = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;

        if (spacePressed && !spacePressedLastFrame) {
            isPaused = !isPaused;
        }
        
        bool shouldStep = false;
        if (rightArrowPressed && !rightArrowPressedLastFrame) {
            shouldStep = true;
        }

        spacePressedLastFrame = spacePressed;
        rightArrowPressedLastFrame = rightArrowPressed;

        if (!isPaused || shouldStep) {
            if (!isPaused) {
                accumulator += frameTime;
            } else {
                accumulator = fixedDeltaTime;
            }

            accumulator += frameTime;
            while (accumulator >= fixedDeltaTime) {
                for (auto& p : particles) p.update(fixedDeltaTime, world);
                for (auto& b : bodies) b.update(fixedDeltaTime, world);
                
                cDatas = collisionHandler.handleCollisions(bodies, renderer);
                
                accumulator -= fixedDeltaTime;

                if (shouldStep) break; // 1 iter
            }

            // DEBUG
            // for (auto& b : bodies) {
            //     if (b.position.y < 1.0f && b.ID != -1) {
            //         std::cout << b << std::endl;
            //         // isPaused = true;
            //     }
            // }
            // DEBUG
        }

        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        renderer.drawRigidbody2D(bodies, true);
        renderer.drawParticles(particles);
        for (PhysicsEngine::CollisionData cData : cDatas) renderer.drawContactPoints(cData);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        // collisionHandler.handleCollisions(bodies, renderer);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    renderer.shutdown();
    glfwTerminate();
    return 0;
}