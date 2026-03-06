#include "PhysicsEngine/Renderer.h"
#include "PhysicsEngine/structs.h"
#include "PhysicsEngine/Constants.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

namespace PhysicsEngine {

    // write scaling func

    // read shader data
    std::string readFile(const char* filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cout << "ERROR::IO: Failed to open file: " << filePath << std::endl;
            return "";
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        
        return buffer.str() + "\0";
    }

    Vector2 Renderer::worldToPixels(Vector2 worldCoord) {
        float x_pct = (worldCoord.x - world.left) / (world.right - world.left);
        float y_pct = (worldCoord.y - world.bottom) / (world.top - world.bottom);

        return {
            x_pct * windowsize.x,           // X stays normal
            (1.0f - y_pct) * windowsize.y   // Flip Y: 1.0 (top) becomes 0, 0.0 (bottom) becomes windowHeight
        };
    }

    void Renderer::init(PhysicsEngine::WorldBoundaries wb, Vector2 screen) { // bottom left is (-1, -1)
        // glm::ortho(left, right, bottom, top, zNear, zFar)
        projection = glm::ortho(wb.left, wb.right, wb.bottom, wb.top, -1.0f, 1.0f);
        world = wb;
        windowsize = screen;

        std::string vertexShaderCode = readFile("shaders/basic.vert");
        std::string fragmentShaderCode = readFile("shaders/basic.frag");

        const char* vertexShaderSource = vertexShaderCode.c_str();
        const char* fragmentShaderSource = fragmentShaderCode.c_str();

        unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
        unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        // Setup GPU Memory (VAO/VBO)
        glGenVertexArrays(1, &VAO);
        glGenVertexArrays(1, &debugVAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(debugVAO); 
        glBindBuffer(GL_ARRAY_BUFFER, VBO); 
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vector2), (void*)0);
        glEnableVertexAttribArray(0);
        
        glEnable(GL_PROGRAM_POINT_SIZE); // Allows us to change point size in shader
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive blending: perfect for glow
    }

    void Renderer::initRB2D(Rigidbody2D& body) {
        glGenVertexArrays(1, &body.VAO);
        glGenBuffers(1, &body.VBO);

        glBindVertexArray(body.VAO);
        glBindBuffer(GL_ARRAY_BUFFER, body.VBO);

        glBufferData(GL_ARRAY_BUFFER, body.localVertices.size() * sizeof(Vector2), body.localVertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vector2), (void*)0);
        glEnableVertexAttribArray(0);
        
        glBindVertexArray(0);
    }

    void Renderer::drawParticles(const std::vector<Particle>& particles) {
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        
        glm::mat4 model = glm::mat4(1.0f);
        int modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        
        int projLoc = glGetUniformLocation(this->shaderProgram, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        
        // Send new physics positions to GPU
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), particles.data(), GL_DYNAMIC_DRAW);

        // set color
        int colorLoc = glGetUniformLocation(shaderProgram, "objectColor");
        int sizeLoc = glGetUniformLocation(shaderProgram, "pointSize");

        glUniform1f(sizeLoc, PhysicsEngine::Constants::THINSIZE);
        glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 0.8f); 
        glDrawArrays(GL_POINTS, 0, particles.size());
    }

    void Renderer::drawContactPoints(const CollisionData& data) {
        std::cout << data << std::endl;
        if (!data.collided || data.contactPoint.empty()) return;

        std::vector<Vector2> points;
        std::vector<Particle> particlePoints;
        for (const auto& cp : data.contactPoint) {
            points.push_back(cp.position);
            Particle p = {1, cp.position, {0, 0}, {0, 0}};
            particlePoints.push_back(p);
            std::cout << p << std::endl;
        }

        this->drawParticles(particlePoints);

        // glUseProgram(shaderProgram);
        // int projLoc = glGetUniformLocation(shaderProgram, "projection");
        // glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // int modelLoc = glGetUniformLocation(shaderProgram, "model");
        // glm::mat4 identity = glm::mat4(1.0f);
        // glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(identity));

        // int sizeLoc = glGetUniformLocation(shaderProgram, "pointSize");
        // glUniform1f(sizeLoc, 100.0f);

        // int colorLoc = glGetUniformLocation(shaderProgram, "objectColor");
        // glUniform4f(colorLoc, 1.0f, 0.0f, 0.0f, 0.8f); 

        // glBindVertexArray(debugVAO); 
        // glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(Vector2), points.data(), GL_DYNAMIC_DRAW);

        // // Draw as GL_POINTS
        // glDrawArrays(GL_POINTS, 0, points.size());
    }

    void Renderer::drawRigidbody2D(const std::vector<Rigidbody2D>& rigidbody2Ds, bool labelVertices) {
        glUseProgram(shaderProgram);

        int projLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        int modelLoc = glGetUniformLocation(shaderProgram, "model");

        int sizeLoc = glGetUniformLocation(shaderProgram, "pointSize");        
        int colorLoc = glGetUniformLocation(shaderProgram, "objectColor");

        for (const auto& body : rigidbody2Ds) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(body.position.x, body.position.y, 0.0f));
            model = glm::rotate(model, body.angPos, glm::vec3(0.0f, 0.0f, 1.0f));
            
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glBindVertexArray(body.VAO);
            
            glLineWidth(PhysicsEngine::Constants::THINSIZE);
            glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 0.8f); // Neon Blue Alpha
            glDrawArrays(GL_LINE_LOOP, 0, body.localVertices.size());
        }
        if (labelVertices) {
            if (ImGui::GetCurrentContext() == nullptr) {
                std::cout << "ERROR: NO ImGui CONTEXT" << std::endl;
                return;
            }

            ImGui::Begin("DebugOverlay", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground);
            
            for (const auto& body : rigidbody2Ds) {
                if (body.worldVertices.empty() || body.worldVertices.size() != body.worldNormals.size()) {
                    std::cout << "n(worldVertices): " << body.worldVertices.size() << ", n(worldNormals): " << body.worldNormals.size() << std::endl;
                    continue; 
                }

                for (size_t i = 0; i < body.worldVertices.size(); ++i) {
                    Vector2 v = body.worldVertices[i];
                    
                    // Draw Vertex Index
                    std::string label = std::to_string(i);
                    Vector2 vPixelPos = worldToPixels(v);
                    ImGui::GetForegroundDrawList()->AddText(
                        ImVec2(vPixelPos.x, vPixelPos.y), 
                        IM_COL32(255, 255, 0, 255), 
                        label.c_str()
                    );
                    
                    // Draw Normals
                    Vector2 nextV = body.worldVertices[(i + 1) % body.worldVertices.size()];
                    Vector2 nextVPixelPos = worldToPixels(nextV);
                    Vector2 mid = {(vPixelPos.x + nextVPixelPos.x) / 2.0f, (vPixelPos.y + nextVPixelPos.y) / 2.0f};
                    
                    // Double check index i is safe for worldNormals
                    Vector2 n = body.worldNormals[i]*20.0f; 
                    
                    ImGui::GetForegroundDrawList()->AddLine(
                        ImVec2(mid.x, mid.y), 
                        ImVec2(mid.x + n.x, mid.y - n.y), 
                        IM_COL32(0, 255, 255, 255)
                    );
                }
            }
            ImGui::End();
        }
    }

    void Renderer::shutdown() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteProgram(shaderProgram);
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    // Helper to compile shaders and print errors
    unsigned int Renderer::compileShader(unsigned int type, const char* source) {
        unsigned int shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, NULL);
        glCompileShader(shader);
        
        int success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        return shader;
    }
}