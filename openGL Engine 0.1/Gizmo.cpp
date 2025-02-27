#include "Gizmo.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/compatibility.hpp> // for glm::lerp


Gizmo::Gizmo() : Shader("gizmo.vert", "gizmo.frag"), model(glm::mat4(1.0f)), modelStart(glm::mat4(1.0f)) {
    arrows = {
        {{1, 0, 0}, {1, 0, 0}},
        {{0, 1, 0}, {0, 1, 0}},
        {{0, 0, 1}, {0, 0, 1}}
    };
    setupBuffers();

}



Gizmo::~Gizmo() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}



void Gizmo::setupBuffers() {

    modelStart = glm::mat4(1.0f);
    std::vector<float> vertices;
    for (const auto& arrow : arrows) {
        // Start point (origin) with color
        vertices.insert(vertices.end(), { 0.0f, 0.0f, 0.0f });
        vertices.insert(vertices.end(), { arrow.color.r, arrow.color.g, arrow.color.b });

        // End point (direction) with color
        vertices.insert(vertices.end(), { arrow.direction.x, arrow.direction.y, arrow.direction.z });
        vertices.insert(vertices.end(), { arrow.color.r, arrow.color.g, arrow.color.b });
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Position Attribute (Location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color Attribute (Location 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


}

void Gizmo::Draw() {
    this->use();
    ImGui::Begin("Gizmo Settings"); // Start a new ImGui window

    // Slider for baseThreshold (range 0.1 to 5.0)
    ImGui::SliderFloat("Base Threshold", &baseThreshold, 0.1f, 5.0f, "%.2f");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Base distance threshold for axis selection (world units)");
    }

    // Slider for scaleM (range 0.01 to 0.5)
    ImGui::SliderFloat("Scale Factor", &scaleM, 0.01f, 0.5f, "%.3f");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Scaling factor for dynamic threshold based on camera distance");
    }
    ImGui::Selectable("Rotate/Move", &gizmoRotate);
    if (ImGui::Button("Reset Gizmo")) {
        this->model = glm::mat4(1.0f);
    
    }
    ImGui::End(); // End the ImGui window

    // Call handleInput to process mouse input and update the model matrix
    handleInput(window, camera.getViewMatrix(), projection, window_height, window_height);

    // Mouse handling
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    bool mouseClicked = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

    if (mouseClicked) {
        logger.AddLog("Mouse clicked", ImGuiLogger::LogType::Info);
        logger.AddLog("Mouse clicked", ImGuiLogger::LogType::RayCasting);
    }

    // Send matrices to shader
    GLint modelLoc = glGetUniformLocation(this->ID, "model");
    GLint viewLoc = glGetUniformLocation(this->ID, "view");
    GLint projLoc = glGetUniformLocation(this->ID, "projection");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); // Use class member
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Draw gizmo
    glLineWidth(6.0f);
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, arrows.size() * 2);
    glBindVertexArray(0);
    glLineWidth(1.0f);
}

void Gizmo::handleInput(GLFWwindow* window, const glm::mat4& view, const glm::mat4& projection, float width, float height) {
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    bool mouseClicked = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

    if (mouseClicked && !isRotating) {
        float x_ndc = 2.0f * (float)mouseX / width - 1.0f;
        float y_ndc = 1.0f - 2.0f * (float)mouseY / height;
        glm::vec4 clipNear(x_ndc, y_ndc, -1.0f, 1.0f);
        glm::vec4 eyeNear = glm::inverse(projection) * clipNear;
        eyeNear /= eyeNear.w;
        glm::vec4 worldNear = glm::inverse(view) * eyeNear;
        glm::vec3 cameraPos = glm::vec3(glm::inverse(view) * glm::vec4(0, 0, 0, 1));
        glm::vec3 rayDir = glm::normalize(glm::vec3(worldNear) - cameraPos);

        float cameraDistance = glm::length(cameraPos);
        float minDist = baseThreshold * (1.0f + cameraDistance * scaleM);
        logger.AddLog("Dynamic MinDist: " + std::to_string(minDist), ImGuiLogger::LogType::RayCasting);

        int bestAxis = -1;
        for (int i = 0; i < 3; i++) {
            glm::vec3 lineStart = glm::vec3(model * glm::vec4(0, 0, 0, 1));
            glm::vec3 lineEnd = glm::vec3(model * glm::vec4(arrows[i].direction, 1));
            float dist = rayLineDistance(cameraPos, rayDir, lineStart, lineEnd);
            if (dist < minDist) {
                minDist = dist;
                bestAxis = i;
            }
        }

        if (bestAxis != -1) {
            std::string axisName = (bestAxis == 0) ? "X" : (bestAxis == 1) ? "Y" : "Z";
            logger.AddLog("Axis Selected: " + axisName + " (Distance: " + std::to_string(minDist) + ")",
                ImGuiLogger::LogType::RayCasting);
            logger.AddLog("Mouse Pos: " + std::to_string(mouseX) + ", " + std::to_string(mouseY),
                ImGuiLogger::LogType::RayCasting);
            isRotating = true;
            selectedAxis = bestAxis;
            mouseInitial = glm::vec2((float)mouseX, (float)mouseY);
            modelInitial = model;
        }
        else {
            logger.AddLog("No Axis Selected (Closest Distance: " + std::to_string(minDist) + ")",
                ImGuiLogger::LogType::RayCasting);
        }
    }

    if (isRotating) {
        if (!mouseClicked) {
            isRotating = false;
        }
        else {
            glm::vec4 center = projection * view * model * glm::vec4(0, 0, 0, 1);
            center /= center.w;
            float centerX = (center.x * 0.5f + 0.5f) * width;
            float centerY = (1.0f - (center.y * 0.5f + 0.5f)) * height;

            glm::vec2 vector0 = mouseInitial - glm::vec2(centerX, centerY);
            glm::vec2 vector1 = glm::vec2((float)mouseX, (float)mouseY) - glm::vec2(centerX, centerY);

            if (glm::length(vector0) > 0.001f && glm::length(vector1) > 0.001f) {
                float delta = glm::length(vector1 - vector0); // Magnitude of mouse movement
                glm::vec3 axis(0.0f);
                axis[selectedAxis] = 1.0f; // Selected axis (X, Y, or Z)

                if (gizmoRotate) {
                    // Rotate mode
                    float theta = atan2(vector1.y, vector1.x) - atan2(vector0.y, vector0.x);
                    model = glm::rotate(modelInitial, theta, axis);
                }
                else {
                    // Translate mode
                    float sensitivity = 0.01f; // Adjust this value to control translation speed
                    float translation = delta * sensitivity; // Convert mouse movement to world units
                    if (vector1.y < vector0.y) translation = -translation; // Direction based on vertical movement
                    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), axis * translation);
                    model = modelInitial * translationMatrix;
                }
            }
        }
    }
}

float Gizmo::rayLineDistance(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& lineStart, const glm::vec3& lineEnd) {
    glm::vec3 d1 = rayDir;
    glm::vec3 d2 = lineEnd - lineStart;
    glm::vec3 d = lineStart - rayOrigin;

    float a = glm::dot(d1, d1);
    float b = glm::dot(d1, d2);
    float c = glm::dot(d2, d2);
    float e = glm::dot(d1, d);
    float f = glm::dot(d2, d);

    float denom = a * c - b * b;
    if (denom == 0.0f) return FLT_MAX;

    float t = (b * f - c * e) / denom;
    float s = (a * f - b * e) / denom;

    if (s < 0.0f) s = 0.0f;
    else if (s > 1.0f) s = 1.0f;

    if (c > 0.0f) t = (glm::dot(d1, (lineStart + s * (lineEnd - lineStart)) - rayOrigin)) / a;

    if (t < 0.0f) return FLT_MAX;

    glm::vec3 pointOnRay = rayOrigin + t * d1;
    glm::vec3 pointOnLine = lineStart + s * d2;
    return glm::distance(pointOnRay, pointOnLine);
}