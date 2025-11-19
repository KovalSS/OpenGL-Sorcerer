#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath> // Для sin, cos

#include "Shader.h"
#include "Model.h"
#include "Camera.h"
#include "Skybox.h"
#include "Callbacks.h"
#include "InputHandler.h"
#include "Globals.h"
#include "Plane.h" 

// Константи
const float SIZE_SKY = 1.0f;
const float NEAR_PLANE = 0.1f;
const float FAR_PLANE = 500.0f;

int main() {
    // Ініціалізація GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Створення вікна
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Model Viewer", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    Camera localCamera(glm::vec3(0.0f, 1.0f, 5.0f)); 
    
    WindowContext context;
    context.camera = &localCamera;
    context.lastX = SCR_WIDTH / 2.0f;
    context.lastY = SCR_HEIGHT / 2.0f;
    context.firstMouse = true;
    
    glfwSetWindowUserPointer(window, &context); 
    
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // Шейдери
    Shader ourShader("../resources/shaders/vertex.shader", "../resources/shaders/fragment.shader");
    Shader brightShader("../resources/shaders/bright_vertex.shader", "../resources/shaders/bright_fragment.shader");
    Shader planeShader("../resources/shaders/plane_vertex.shader", "../resources/shaders/plane_fragment.shader");
    Shader skyboxShader("../resources/shaders/skybox.vert", "../resources/shaders/skybox.frag");
    
    // Моделі
    std::cout << "Loading models..." << std::endl;
    Model ourModel("../resources/models/doctor-strenge/doctor-strenge.obj");
    Model sharedDagger("../resources/models/dagger/dagger.obj");           
    Model sharedFireball("../resources/models/asteroid-balls/asteroid-hot.obj"); 
    
    // Скайбокс
    std::vector<std::string> faces = {
        "../resources/textures/best_skybox/px.png",
        "../resources/textures/best_skybox/nx.png",
        "../resources/textures/best_skybox/py.png",   
        "../resources/textures/best_skybox/ny.png",
        "../resources/textures/best_skybox/pz.png",
        "../resources/textures/best_skybox/nz.png" 
    };
    Skybox skybox(faces, SIZE_SKY);

    // Підлога
    Plane groundPlane("../resources/textures/plane/mondo-grass-300-mm-architextures.jpg", 100.0f);
    groundPlane.setPosition(glm::vec3(0.0f, -0.1f, 0.0f));
    groundPlane.setScale(glm::vec3(100.0f, 1.0f, 100.0f));

    const float scale_dagger = 0.3f;
    const float scale_fireball = 0.01f;
    
    // --- ЗМІННІ ДЛЯ ОРБІТ ---
    // Кинджали (Daggers)
    float daggerOrbitRadius = 1.8f;   
    float daggerOrbitSpeed = 0.5f;    
    float daggerOrbitHeight = 1.3f;   
    
    // Вогняні кулі (Fireballs)
    float fireballOrbitRadius = 2.2f; 
    float fireballOrbitSpeed = -0.5f; 
    float fireballOrbitHeight = 0.8f; 
    
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        // ====================================================================================
        // ЛОГІКА УПРАВЛІННЯ (Клавіші 1/2 + Плюс/Мінус) + ПІДТРИМКА NUMPAD
        // ====================================================================================
        
        // Перевіряємо чи натиснуто "+" (звичайний або NumPad)
        bool isPlusPressed = (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS);
        // Перевіряємо чи натиснуто "-" (звичайний або NumPad)
        bool isMinusPressed = (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS);
        
        // --- Режим 1: Вогняні Кулі (Fireballs) ---
        // Додано GLFW_KEY_KP_1
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS) {
            if (isPlusPressed) {
                fireballOrbitRadius += 2.0f * deltaTime; 
                fireballOrbitSpeed -= 1.0f * deltaTime;  
            }
            if (isMinusPressed) {
                fireballOrbitRadius -= 2.0f * deltaTime; 
                fireballOrbitSpeed += 1.0f * deltaTime;
                
                if (fireballOrbitRadius < 1.5f) fireballOrbitRadius = 1.5f;
                if (fireballOrbitSpeed > -0.1f) fireballOrbitSpeed = -0.1f; 
            }
        }

        // --- Режим 2: Кинджали (Daggers) ---
        // Додано GLFW_KEY_KP_2
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS) {
            if (isPlusPressed) {
                daggerOrbitRadius += 2.0f * deltaTime; 
                daggerOrbitSpeed += 1.0f * deltaTime; 
            }
            if (isMinusPressed) {
                daggerOrbitRadius -= 2.0f * deltaTime; 
                daggerOrbitSpeed -= 1.0f * deltaTime;
                
                if (daggerOrbitRadius < 1.0f) daggerOrbitRadius = 1.0f;
                if (daggerOrbitSpeed < 0.1f) daggerOrbitSpeed = 0.1f; 
            }
        }

        // Очищення
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(localCamera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, NEAR_PLANE, FAR_PLANE);
        glm::mat4 view = localCamera.GetViewMatrix();

        // ====================================================================================
        // 0. РОЗРАХУНОК ПОЗИЦІЙ КУЛЬ
        // ====================================================================================
        
        std::vector<glm::vec3> currentFireballPositions;
        float fireballBaseAngle = currentFrame * fireballOrbitSpeed; 

        for (int i = 0; i < 8; i++) {
            float angle = fireballBaseAngle + glm::radians(i * 45.0f + 22.5f);
            float x = cos(angle) * fireballOrbitRadius;
            float z = sin(angle) * fireballOrbitRadius;
            currentFireballPositions.push_back(glm::vec3(x, fireballOrbitHeight, z));
        }

        // ====================================================================================
        // 1. ОНОВЛЮЄМО ДЖЕРЕЛА СВІТЛА В ШЕЙДЕРАХ
        // ====================================================================================
        
        ourShader.use();
        ourShader.setVec3("viewPos", localCamera.Position);

        planeShader.use();
        planeShader.setVec3("viewPos", localCamera.Position);

        for (int i = 0; i < 8; i++) {
            std::string number = std::to_string(i);
            glm::vec3 pos = currentFireballPositions[i];

            // --- ourShader (Доктор) ---
            ourShader.use();
            ourShader.setVec3("pointLights[" + number + "].position", pos);
            ourShader.setVec3("pointLights[" + number + "].ambient",  glm::vec3(0.05f, 0.05f, 0.05f));
            ourShader.setVec3("pointLights[" + number + "].diffuse",  glm::vec3(0.8f, 0.3f, 0.0f)); 
            ourShader.setVec3("pointLights[" + number + "].specular", glm::vec3(1.0f, 0.5f, 0.0f)); 
            ourShader.setFloat("pointLights[" + number + "].constant", 1.0f);
            ourShader.setFloat("pointLights[" + number + "].linear", 0.09f);
            ourShader.setFloat("pointLights[" + number + "].quadratic", 0.032f);

            // --- planeShader (Трава) ---
            planeShader.use();
            planeShader.setVec3("pointLights[" + number + "].position", pos);
            planeShader.setVec3("pointLights[" + number + "].ambient",  glm::vec3(0.05f, 0.05f, 0.05f));
            planeShader.setVec3("pointLights[" + number + "].diffuse",  glm::vec3(0.8f, 0.3f, 0.0f)); 
            planeShader.setVec3("pointLights[" + number + "].specular", glm::vec3(1.0f, 0.5f, 0.0f)); 
            planeShader.setFloat("pointLights[" + number + "].constant", 1.0f);
            planeShader.setFloat("pointLights[" + number + "].linear", 0.09f);
            planeShader.setFloat("pointLights[" + number + "].quadratic", 0.032f);
        }

        // ====================================================================================
        // 2. МАЛЮВАННЯ СЦЕНИ
        // ====================================================================================

        // Підлога
        planeShader.use();
        planeShader.setMat4("view", view);
        planeShader.setMat4("projection", projection);
        glm::mat4 planeModel = glm::mat4(1.0f); 
        planeShader.setMat4("model", planeModel);
        groundPlane.Draw(planeShader);

        // Скайбокс
        skyboxShader.use();
        skybox.Draw(skyboxShader, view, projection);

        // Доктор Стрендж
        ourShader.use();
        ourShader.setFloat("time", currentFrame);
        glm::mat4 model = glm::mat4(1.0f);
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);

        // Кинджали
        float daggerBaseAngle = currentFrame * daggerOrbitSpeed; 
        ourShader.use();
        for (int i = 0; i < 8; i++) {
            glm::mat4 orbitMatrix = glm::mat4(1.0f);
            float angle = daggerBaseAngle + glm::radians(i * 45.0f);
            
            float x = cos(angle) * daggerOrbitRadius;
            float z = sin(angle) * daggerOrbitRadius;
            
            orbitMatrix = glm::translate(orbitMatrix, glm::vec3(x, daggerOrbitHeight, z));
            
            glm::vec3 orbitCenter = glm::vec3(0.0f, daggerOrbitHeight, 0.0f);
            glm::vec3 orbitPos = glm::vec3(x, daggerOrbitHeight, z);
            glm::vec3 directionToCenter = glm::normalize(orbitCenter - orbitPos);
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
            glm::vec3 right = glm::normalize(glm::cross(up, directionToCenter));
            glm::vec3 actualUp = glm::normalize(glm::cross(directionToCenter, right));
            
            orbitMatrix[0] = glm::vec4(right, 0.0f);
            orbitMatrix[1] = glm::vec4(actualUp, 0.0f);
            orbitMatrix[2] = glm::vec4(directionToCenter, 0.0f);
            orbitMatrix = glm::scale(orbitMatrix, glm::vec3(scale_dagger));
            
            ourShader.setMat4("model", orbitMatrix);
            sharedDagger.Draw(ourShader);
        }

        // Вогняні кулі
        brightShader.use();
        brightShader.setFloat("time", currentFrame);
        brightShader.setMat4("projection", projection);
        brightShader.setMat4("view", view);
        
        for (int i = 0; i < 8; i++) {
            glm::mat4 fireballMatrix = glm::mat4(1.0f);
            glm::vec3 pos = currentFireballPositions[i];
            
            fireballMatrix = glm::translate(fireballMatrix, pos);
            
            glm::vec3 orbitCenter = glm::vec3(0.0f, fireballOrbitHeight, 0.0f);
            glm::vec3 directionToCenter = glm::normalize(orbitCenter - pos);
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
            glm::vec3 right = glm::normalize(glm::cross(up, directionToCenter));
            glm::vec3 actualUp = glm::normalize(glm::cross(directionToCenter, right));
            
            fireballMatrix[0] = glm::vec4(right, 0.0f);
            fireballMatrix[1] = glm::vec4(actualUp, 0.0f);
            fireballMatrix[2] = glm::vec4(directionToCenter, 0.0f);

            fireballMatrix = glm::rotate(fireballMatrix, currentFrame * 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
            float pulse = sin(currentFrame * 3.0f) * 0.2f + 1.0f;
            fireballMatrix = glm::scale(fireballMatrix, glm::vec3(scale_fireball * pulse));
            
            brightShader.setMat4("model", fireballMatrix);
            sharedFireball.Draw(brightShader);
        }

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) std::cout << "OpenGL error: " << error << std::endl;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}