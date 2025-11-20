#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>

#include "../Core/Shader.h"
#include "../Core/Model.h"
#include "../Core/Camera.h"
#include "../Core/Skybox.h"
#include "../Utils/Callbacks.h"
#include "../Utils/InputHandler.h"
#include "../Utils/Globals.h"
#include "../Core/Plane.h" 
#include "../Logic/OrbitSystem.h" 
#include "../Logic/LightingSystem.h" 

const float SIZE_SKY = 1.0f;
const float NEAR_PLANE = 0.1f;
const float FAR_PLANE = 500.0f;

int main() {
    // --- 1. ІНІЦІАЛІЗАЦІЯ ---
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

    // --- 2. РЕСУРСИ (ШЕЙДЕРИ ТА МОДЕЛІ) ---
    Shader ourShader("../resources/shaders/vertex.shader", "../resources/shaders/fragment.shader");
    Shader brightShader("../resources/shaders/bright_vertex.shader", "../resources/shaders/bright_fragment.shader");
    Shader planeShader("../resources/shaders/plane_vertex.shader", "../resources/shaders/plane_fragment.shader");
    Shader skyboxShader("../resources/shaders/skybox.vert", "../resources/shaders/skybox.frag");
    
    std::cout << "Loading models..." << std::endl;
    Model ourModel("../resources/models/doctor-strenge/doctor-strenge.obj");
    // Змінна для керування розміром персонажа
    float ourModelScale = 1.0f; // Початковий масштаб

    // Model ourModel("../resources/models/Kipfel/Kipfel .obj");
    // float ourModelScale = 0.03f;

    Model sharedDagger("../resources/models/dagger/dagger.obj");           
    Model sharedFireball("../resources/models/asteroid-balls/asteroid-hot.obj"); 
    Model energyOrbModel("../resources/models/magma-ball/magma-ball.obj");

    std::vector<std::string> faces = {
        "../resources/textures/skybox_space/right.jpg",  
        "../resources/textures/skybox_space/left.jpg",   
        "../resources/textures/skybox_space/top.jpg",    
        "../resources/textures/skybox_space/bottom.jpg", 
        "../resources/textures/skybox_space/back.jpg",   
        "../resources/textures/skybox_space/front.jpg"   
    };
    Skybox skybox(faces, SIZE_SKY);

    Plane groundPlane("../resources/textures/plane/mondo-grass-300-mm-architextures.jpg", 100.0f);
    groundPlane.setPosition(glm::vec3(0.0f, -0.1f, 0.0f));
    groundPlane.setScale(glm::vec3(100.0f, 1.0f, 100.0f));

    // --- 3. СИСТЕМИ ---
    OrbitSystem orbitSystem; 

    // --- 4. ГОЛОВНИЙ ЦИКЛ ---
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Ввід
        processInput(window); // Камера (через InputHandler.cpp)
        orbitSystem.ProcessInput(window, deltaTime); // Об'єкти сцени (Кулі, Кинджали, Сфера)

        // Оновлення матриць
        glm::mat4 projection = glm::perspective(glm::radians(localCamera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, NEAR_PLANE, FAR_PLANE);
        glm::mat4 view = localCamera.GetViewMatrix();

        // Оновлення світла (Централізовано!)
        LightingSystem::UpdateLights(ourShader, orbitSystem, currentFrame, true);
        LightingSystem::UpdateLights(planeShader, orbitSystem, currentFrame, false); // Для підлоги без specular

        // --- Рендеринг ---
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 1. Підлога
        planeShader.use();
        planeShader.setMat4("view", view);
        planeShader.setMat4("projection", projection);
        groundPlane.Draw(planeShader);

        // 2. Скайбокс
        skyboxShader.use();
        skybox.Draw(skyboxShader, view, projection);

        // 3. Доктор Стрендж
        ourShader.use();
        ourShader.setFloat("time", currentFrame);
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        glm::mat4 ourModelModel = glm::mat4(1.0f);
        ourModelModel = glm::scale(ourModelModel, glm::vec3(ourModelScale));

        ourShader.setMat4("model", ourModelModel);
        ourModel.Draw(ourShader);

        // 4. Кинджали
        auto daggerMatrices = orbitSystem.GetDaggerMatrices(currentFrame);
        ourShader.use(); 
        for (const auto& matrix : daggerMatrices) {
            ourShader.setMat4("model", matrix);
            sharedDagger.Draw(ourShader);
        }

        // 5. Орбітальні Вогняні Кулі
        brightShader.use();
        brightShader.setFloat("time", currentFrame);
        brightShader.setMat4("projection", projection);
        brightShader.setMat4("view", view);
        auto fireballMatrices = orbitSystem.GetFireballMatrices(currentFrame);
        for (const auto& matrix : fireballMatrices) {
            brightShader.setMat4("model", matrix);
            sharedFireball.Draw(brightShader);
        }

        // 6. Центральна Енергетична Сфера
        glm::mat4 energyOrbMatrix = orbitSystem.GetEnergyOrbMatrix(currentFrame);
        brightShader.setMat4("model", energyOrbMatrix);
        energyOrbModel.Draw(brightShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}