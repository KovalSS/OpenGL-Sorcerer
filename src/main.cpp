#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>

#include "Shader.h"
#include "Model.h"
#include "Camera.h"
#include "Skybox.h"
#include "Callbacks.h"
#include "InputHandler.h"
#include "Globals.h"
#include "Plane.h" 
#include "OrbitSystem.h" // <-- ПІДКЛЮЧИЛИ НАШ НОВИЙ КЛАС

const float SIZE_SKY = 1.0f;
const float NEAR_PLANE = 0.1f;
const float FAR_PLANE = 500.0f;

int main() {
    // --- ІНІЦІАЛІЗАЦІЯ (GLFW, Window, GLAD, OpenGL) ---
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

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // --- ШЕЙДЕРИ ---
    Shader ourShader("../resources/shaders/vertex.shader", "../resources/shaders/fragment.shader");
    Shader brightShader("../resources/shaders/bright_vertex.shader", "../resources/shaders/bright_fragment.shader");
    Shader planeShader("../resources/shaders/plane_vertex.shader", "../resources/shaders/plane_fragment.shader");
    Shader skyboxShader("../resources/shaders/skybox.vert", "../resources/shaders/skybox.frag");
    
    // --- МОДЕЛІ ---
    std::cout << "Loading models..." << std::endl;
    Model ourModel("../resources/models/doctor-strenge/doctor-strenge.obj");
    Model sharedDagger("../resources/models/dagger/dagger.obj");           
    Model sharedFireball("../resources/models/asteroid-balls/asteroid-hot.obj"); 
    
    // --- ЕЛЕМЕНТИ СЦЕНИ ---
    std::vector<std::string> faces = {
        "../resources/textures/best_skybox/px.png", "../resources/textures/best_skybox/nx.png",
        "../resources/textures/best_skybox/py.png", "../resources/textures/best_skybox/ny.png",
        "../resources/textures/best_skybox/pz.png", "../resources/textures/best_skybox/nz.png" 
    };
    Skybox skybox(faces, SIZE_SKY);

    Plane groundPlane("../resources/textures/plane/mondo-grass-300-mm-architextures.jpg", 100.0f);
    groundPlane.setPosition(glm::vec3(0.0f, -0.1f, 0.0f));
    groundPlane.setScale(glm::vec3(100.0f, 1.0f, 100.0f));

    // --- СИСТЕМА ОРБІТ (НОВА!) ---
    OrbitSystem orbitSystem;

    // --- ГОЛОВНИЙ ЦИКЛ ---
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        
        // 1. Обробка логіки орбіт (клавіші 1/2 +/-)
        orbitSystem.ProcessInput(window, deltaTime);

        // Очищення
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(localCamera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, NEAR_PLANE, FAR_PLANE);
        glm::mat4 view = localCamera.GetViewMatrix();

        // 2. Отримуємо дані від системи орбіт
        std::vector<glm::vec3> lightPositions = orbitSystem.GetFireballPositions(currentFrame);
        std::vector<glm::mat4> fireballMatrices = orbitSystem.GetFireballMatrices(currentFrame, lightPositions);
        std::vector<glm::mat4> daggerMatrices = orbitSystem.GetDaggerMatrices(currentFrame);

        // 3. Оновлюємо світло в шейдерах
        ourShader.use();
        ourShader.setVec3("viewPos", localCamera.Position);
        planeShader.use();
        planeShader.setVec3("viewPos", localCamera.Position);

        for (int i = 0; i < 8; i++) {
            std::string num = std::to_string(i);
            glm::vec3 pos = lightPositions[i];

            // Налаштування світла (однакове для обох шейдерів)
            auto setLightParams = [&](Shader& s) {
                s.use();
                s.setVec3("pointLights[" + num + "].position", pos);
                s.setVec3("pointLights[" + num + "].ambient",  glm::vec3(0.05f));
                s.setVec3("pointLights[" + num + "].diffuse",  glm::vec3(0.8f, 0.3f, 0.0f)); 
                s.setVec3("pointLights[" + num + "].specular", glm::vec3(1.0f, 0.5f, 0.0f)); 
                s.setFloat("pointLights[" + num + "].constant", 1.0f);
                s.setFloat("pointLights[" + num + "].linear", 0.09f);
                s.setFloat("pointLights[" + num + "].quadratic", 0.032f);
            };

            setLightParams(ourShader);
            setLightParams(planeShader);
        }

        // 4. Малюємо сцену
        
        // Підлога
        planeShader.use();
        planeShader.setMat4("view", view);
        planeShader.setMat4("projection", projection);
        planeShader.setMat4("model", glm::mat4(1.0f));
        groundPlane.Draw(planeShader);

        // Скайбокс
        skyboxShader.use();
        skybox.Draw(skyboxShader, view, projection);

        // Доктор Стрендж
        ourShader.use();
        ourShader.setFloat("time", currentFrame);
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        ourShader.setMat4("model", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f)), glm::vec3(1.0f)));
        ourModel.Draw(ourShader);

        // Кинджали
        for (const auto& matrix : daggerMatrices) {
            ourShader.setMat4("model", matrix);
            sharedDagger.Draw(ourShader);
        }

        // Вогняні кулі
        brightShader.use();
        brightShader.setFloat("time", currentFrame);
        brightShader.setMat4("projection", projection);
        brightShader.setMat4("view", view);
        
        for (const auto& matrix : fireballMatrices) {
            brightShader.setMat4("model", matrix);
            sharedFireball.Draw(brightShader);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}