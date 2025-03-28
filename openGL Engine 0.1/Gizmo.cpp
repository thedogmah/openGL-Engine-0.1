#include "Gizmo.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/compatibility.hpp> // for glm::lerp
#include "ModelLoader.h"
struct Ray;





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


//void Gizmo::onMouseClick(int mouseX, int mouseY, int screenWidth, int screenHeight, Camera& camera) {
//    // Convert screen coordinates to Normalized Device Coordinates (NDC)
//    float x = (2.0f * mouseX) / screenWidth - 1.0f;  // Map x to [-1, 1]
//    float y = 1.0f - (2.0f * mouseY) / screenHeight; // Map y to [-1, 1], flip Y-axis for top-left origin
//    float z = -1.0f;                                 // Near plane in NDC
//
//    // Create clip coordinates directly using GLM (no separate function)
//    glm::vec4 clipCoords(x, y, z, 1.0f); // Inline clip coordinate calculation: x, y, z from NDC, w = 1
//
//    // Get projection matrix and compute its inverse
//    //glm::mat4 proj = camera.getProjection();
//    glm::mat4 invProj = glm::inverse(projection); // GLM's built-in inverse
//
//    // Transform clip coordinates to eye (camera) space
//    glm::vec4 eyeCoords = invProj * clipCoords; // GLM matrix-vector multiplication
//
//    // Get view matrix and compute its inverse
//    glm::mat4 view = camera.getViewMatrix();
//    glm::mat4 invView = glm::inverse(view); // GLM's built-in inverse
//
//    // Transform eye coordinates to world space
//    glm::vec4 worldCoords = invView * eyeCoords; // GLM matrix-vector multiplication
//
//    // Perform perspective division to get the origin in world space
//    glm::vec3 origin = glm::vec3(worldCoords) / worldCoords.w; // GLM vec3 extraction and division
//
//    // Calculate direction from camera position to the origin
//    glm::vec3 camPos = camera.getPosition();
//    glm::vec3 direction = glm::normalize(origin - camPos); // GLM subtraction and normalization
//
//    clickRay.origin = origin;
//    clickRay.direction = direction;
//    clickRay.end = origin + direction * rayLength; // GLM vector addition and scaling
//
//}



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

    glGenVertexArrays(1, &rayVAO);
    glGenBuffers(1, &rayVBO);

    glBindVertexArray(rayVAO);
    glBindBuffer(GL_ARRAY_BUFFER, rayVBO);
    // No initial data yet; we'll update it dynamically in Draw()
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}void Gizmo::Draw() {
    this->use();
    ImGui::Begin("Gizmo Settings");
    ImGui::SliderFloat("Base Threshold", &baseThreshold, 0.1f, 15.0f, "%.2f");
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Base distance threshold for axis selection (world units)");
    ImGui::SliderFloat("Scale Factor", &scaleM, 0.01f, 1.5f, "%.3f");
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Scaling factor for dynamic threshold based on camera distance");
    ImGui::Selectable("Rotate/Move", &gizmoRotate);
    ImGui::Checkbox("Show Ray", &renderRay);
    if (ImGui::Button("Reset Gizmo")) {
        this->model = glm::mat4(1.0f);
    }
    ImGui::End();

    handleInput(window, camera.getViewMatrix(), projection, window_width, window_height);

    // --- Gizmo Rendering ---
    std::vector<float> gizmoVertices;
    for (const auto& arrow : arrows) {
        gizmoVertices.insert(gizmoVertices.end(), { 0.0f, 0.0f, 0.0f });
        gizmoVertices.insert(gizmoVertices.end(), { arrow.color.r, arrow.color.g, arrow.color.b });
        gizmoVertices.insert(gizmoVertices.end(), { arrow.direction.x, arrow.direction.y, arrow.direction.z });
        gizmoVertices.insert(gizmoVertices.end(), { arrow.color.r, arrow.color.g, arrow.color.b });
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, gizmoVertices.size() * sizeof(float), gizmoVertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLint modelLoc = glGetUniformLocation(this->ID, "model");
    GLint viewLoc = glGetUniformLocation(this->ID, "view");
    GLint projLoc = glGetUniformLocation(this->ID, "projection");

    // Apply gizmo's model matrix
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(activeModelPtr->GetModelMatrix()));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glLineWidth(7.0f);
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, arrows.size() * 2);
    glBindVertexArray(0);

    // --- Ray Rendering ---
    if (renderRay && !rays.empty()) {
        // Prepare vertex data for all rays
        std::vector<float> rayVertices;
        rayVertices.reserve(rays.size() * 12);  // 6 floats per vertex, 2 vertices per ray

        for (const Ray& ray : rays) {
            // Start vertex
            rayVertices.push_back(ray.origin.x);
            rayVertices.push_back(ray.origin.y);
            rayVertices.push_back(ray.origin.z);
            rayVertices.push_back(ray.color.r);
            rayVertices.push_back(ray.color.g);
            rayVertices.push_back(ray.color.b);

            // End vertex
            rayVertices.push_back(ray.end.x);
            rayVertices.push_back(ray.end.y);
            rayVertices.push_back(ray.end.z);
            rayVertices.push_back(ray.color.r);  // You could use different colors for end
            rayVertices.push_back(ray.color.g);
            rayVertices.push_back(ray.color.b);
        }

        // Upload all ray data at once
        glBindBuffer(GL_ARRAY_BUFFER, rayVBO);
        glBufferData(GL_ARRAY_BUFFER, rayVertices.size() * sizeof(float),
            rayVertices.data(), GL_STATIC_DRAW);

        // Set up vertex attributes (assuming position + color)
        glBindVertexArray(rayVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
            6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
            6 * sizeof(float), (void*)(3 * sizeof(float)));

        // Draw all rays in one call
        glm::mat4 identity = glm::mat4(1.0f);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(identity));
        glDrawArrays(GL_LINES, 0, rays.size() * 2);

        // Cleanup
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    glLineWidth(1.0f);
}

glm::vec2 projectToScreen(const glm::vec3& worldPos, const glm::mat4& view, const glm::mat4& projection, int width, int height) {
    glm::vec4 clip = projection * view * glm::vec4(worldPos, 1.0f);
    if (clip.w <= 0.0f) return glm::vec2(-1.0f, -1.0f); // Point behind camera
    glm::vec3 ndc = glm::vec3(clip) / clip.w; // Perspective division
    float screenX = (ndc.x + 1.0f) * 0.5f * width; // Map [-1,1] to [0,width]
    float screenY = (1.0f - ndc.y) * 0.5f * height; // Map [-1,1] to [0,height], flip Y
    return glm::vec2(screenX, screenY);
}

float distanceToSegment(const glm::vec2& P, const glm::vec2& A, const glm::vec2& B) {
    glm::vec2 AB = B - A;
    glm::vec2 AP = P - A;
    float lengthSquared = glm::dot(AB, AB);
    if (lengthSquared == 0.0f) return glm::length(AP); // A and B coincide
    float t = glm::dot(AP, AB) / lengthSquared; // Projection parameter
    t = glm::clamp(t, 0.0f, 1.0f); // Clamp to segment
    glm::vec2 projection = A + t * AB;
    return glm::length(P - projection);
}
void Gizmo::handleInput(GLFWwindow* window, const glm::mat4& view, const glm::mat4& projection,
    int windowWidth, int windowHeight) {
    static bool wasClicked = false;
    static int selectedAxis = -1;
    static glm::vec3 lockedAxis(0.0f);
    static double lastMouseX = 0.0, lastMouseY = 0.0;

    bool isClicked = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);



    // On initial mouse press: select the axis and lock the axis direction.
    if (isClicked && !wasClicked) {

        // --- Ray Casting for Visualization ---
        float x = (2.0f * mouseX) / windowWidth - 1.0f;
        float y = 1.0f - (2.0f * mouseY) / windowHeight;
        glm::vec4 clipCoords(x, y, -1.0f, 1.0f);
        glm::mat4 invProj = glm::inverse(projection);
        glm::vec4 eyeCoords = invProj * clipCoords;
        glm::mat4 invView = glm::inverse(camera.getViewMatrix());
        glm::vec4 worldCoords = invView * eyeCoords;
        glm::vec3 origin = glm::vec3(worldCoords) / worldCoords.w;
        glm::vec3 camPos = camera.getPosition();
        glm::vec3 direction = glm::normalize(origin - camPos);

        //float x = (2.0f * mouseX) / windowWidth - 1.0f;
        //float y = 1.0f - (2.0f * mouseY) / windowHeight;
        //glm::vec4 clipCoords(x, y, -1.0f, 1.0f);
        //glm::mat4 invProj = glm::inverse(projection);
        //glm::vec4 eyeCoords = invProj * clipCoords;
        //glm::mat4 invView = glm::inverse(camera.getViewMatrix());
        //glm::vec4 worldCoords = invView * eyeCoords;

        Ray newRay;  // Create a new ray - this is for rendering multiple clicks
        newRay.origin = glm::vec3(worldCoords) / worldCoords.w;
        newRay.direction = glm::normalize(newRay.origin - camera.getPosition());
        newRay.end = newRay.origin + newRay.direction * rayLength;
        newRay.color = glm::vec3(0.4f, 0.4f, 0.8f);  // Set your desired color

        rays.push_back(newRay);  // Add it to the vector
        if (rays.size() > 5)
        {
            rays.pop_front();

        }
      //  renderRay = true;
        glm::vec2 P_screen(static_cast<float>(mouseX), static_cast<float>(mouseY));
        glm::mat4 model = activeModelPtr->GetModelMatrix();
        float minDistance = std::numeric_limits<float>::max();
        selectedAxis = -1;

        for (int i = 0; i < 3; i++) { // X, Y, Z axes
            glm::vec3 start = glm::vec3(model[3]); // Gizmo origin
            glm::vec3 end = start + glm::vec3(model[i]); // Axis direction

            glm::vec2 A_screen = projectToScreen(start, view, projection, windowWidth, windowHeight);
            glm::vec2 B_screen = projectToScreen(end, view, projection, windowWidth, windowHeight);

            float distance = distanceToSegment(P_screen, A_screen, B_screen);

            if (distance < minDistance) {
                minDistance = distance;
                selectedAxis = i;
            }
        }

        float threshold = 20.0f;
        if (minDistance > threshold) {
            selectedAxis = -1;
        }
        else {
            // Lock the axis direction from the current model matrix.
            lockedAxis = glm::normalize(glm::vec3(model[selectedAxis]));
            lastMouseX = mouseX;
            lastMouseY = mouseY;
        }
    }
    // During drag: use the locked axis without recalculating it.
    else if (isClicked && wasClicked && selectedAxis != -1) {
        float deltaX = static_cast<float>(mouseX - lastMouseX);
        float deltaY = static_cast<float>(mouseY - lastMouseY);

        if (gizmoRotate) {
            float sensitivity = 0.005f;
            float rotationAmount = 0.0f;
            // Use the lockedAxis directly.
            if (selectedAxis == 0) {
                rotationAmount = -deltaY * sensitivity;
            }
            else if (selectedAxis == 1) {
                rotationAmount = deltaX * sensitivity;
            }
            else if (selectedAxis == 2) {
                rotationAmount = deltaX * sensitivity;
            }
            activeModelPtr->worldTransform = glm::rotate(activeModelPtr->GetModelMatrix(), rotationAmount, lockedAxis);
        }
        else {
            // Similar approach for translation if needed
            float sensitivity = 0.01f;
            glm::vec3 translation(0.0f);
            if (selectedAxis == 0) {
                translation = lockedAxis * (deltaX * sensitivity);
            }
            else if (selectedAxis == 1) {
                translation = lockedAxis * (-deltaY * sensitivity);
            }
            else if (selectedAxis == 2) {
                float amount = (std::abs(deltaX) > std::abs(deltaY)) ? deltaX * sensitivity : -deltaY * sensitivity;
                translation = lockedAxis * amount;
            }

            activeModelPtr->worldTransform[3][0] += translation.x;
            activeModelPtr->worldTransform[3][1] += translation.y;
            activeModelPtr->worldTransform[3][2] += translation.z;
        }
        lastMouseX = mouseX;
        lastMouseY = mouseY;
    }
    else if (!isClicked && wasClicked) {
        selectedAxis = -1;
      //  renderRay = true;
    }

    wasClicked = isClicked;
}
