#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>

#include "Shader.h"
#include "Model.h"
#include "Camera.h"
#include "Skybox.h"
#include "Callbacks.h"
#include "InputHandler.h"
#include "Globals.h"
#include "Plane.h" 
#include "OrbitSystem.h" 

const float SIZE_SKY = 1.0f;
const float NEAR_PLANE = 0.1f;
const float FAR_PLANE = 500.0f;

int main() {
    // --- ІНІЦІАЛІЗАЦІЯ ---
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
    
    Model energyOrbModel("../resources/models/magma-ball/magma-ball.obj");

    // --- ЕЛЕМЕНТИ СЦЕНИ ---
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

    OrbitSystem orbitSystem;

    // ==========================================================================
    // НАЛАШТУВАННЯ ЕНЕРГЕТИЧНОЇ СФЕРИ (ЗМІННІ)
    // ==========================================================================
    
    // Основні параметри, що змінюються кнопками
    float orbHeight = 3.0f;         
    float orbBaseScale = 0.8f;      
    float orbIntensity = 1.0f;      
    
    // Параметри анімації
    float orbPulseSpeed = 4.0f;     
    float orbRotateSpeed = 5.0f;    
    
    // НОВІ ПАРАМЕТРИ ПУЛЬСАЦІЇ
    // Мінімальний та максимальний множник розміру під час пульсації
    // (наприклад, від 0.8 до 1.2 від базового розміру)
    float orbPulseMin = 0.75f; 
    float orbPulseMax = 1.1f;

    // Пропорція впливу пульсації на світло. 
    // 1.0 = світло змінюється так само як розмір. 
    // 2.0 = світло змінюється вдвічі сильніше за розмір.
    float orbLightPulseRatio = 1.5f; 

    // Базові кольори світла сфери
    glm::vec3 orbColorDiffuseBase = glm::vec3(1.5f, 1.5f, 0.0f); 
    glm::vec3 orbColorAmbientBase = glm::vec3(0.3f, 0.3f, 0.0f); 
    // ---------------------------------------------------------------

    // --- ГОЛОВНИЙ ЦИКЛ ---
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        orbitSystem.ProcessInput(window, deltaTime);

        // --- ЛОГІКА УПРАВЛІННЯ СФЕРОЮ (Клавіша 3 + Плюс/Мінус) ---
        bool isPlus = (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS);
        bool isMinus = (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS);
        bool isMode3 = (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_3) == GLFW_PRESS);

        if (isMode3) {
            if (isPlus) {
                orbHeight += 2.0 * deltaTime;      
                orbBaseScale += 1.5f * deltaTime;  
                orbIntensity += 1.0f * deltaTime;   
            }
            if (isMinus && orbHeight > 2.5f) {
                orbHeight -= 2.0f * deltaTime;
                orbBaseScale -= 1.5f * deltaTime;
                orbIntensity -= 1.0f * deltaTime;
            }
        }
        
        // --- РОЗРАХУНОК ПУЛЬСАЦІЇ (РОЗМІР ТА СВІТЛО) ---
        
        // 1. Отримуємо синус від -1 до 1
        float sineValue = sin(currentFrame * orbPulseSpeed);
        // 2. Переводимо в діапазон від 0 до 1
        float normalizedSine = (sineValue + 1.0f) / 2.0f;
        // 3. Інтерполюємо між Min та Max (отримуємо точний множник розміру)
        float currentPulseFactor = orbPulseMin + (normalizedSine * (orbPulseMax - orbPulseMin));

        // 4. Розраховуємо множник для світла на основі пульсації та пропорції
        // (currentPulseFactor - 1.0f) дає відхилення від норми (наприклад, -0.2 або +0.2)
        // Множимо це відхилення на Ratio і додаємо назад до 1.0
        float lightPulseFactor = 1.0f + ((currentPulseFactor - 1.0f) * orbLightPulseRatio);
        // Переконаємось, що світло не стає від'ємним (хоча при розумних налаштуваннях не повинно)
        if(lightPulseFactor < 0.1f) lightPulseFactor = 0.1f;


        // --- Розрахунок матриці для Сфери ---
        glm::vec3 energyOrbPos = glm::vec3(0.0f, orbHeight, 0.0f); 
        float energyOrbRotateSpeed = currentFrame * orbRotateSpeed; 
        
        glm::mat4 energyOrbModelMatrix = glm::mat4(1.0f);
        energyOrbModelMatrix = glm::translate(energyOrbModelMatrix, energyOrbPos);
        energyOrbModelMatrix = glm::rotate(energyOrbModelMatrix, energyOrbRotateSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
        energyOrbModelMatrix = glm::rotate(energyOrbModelMatrix, energyOrbRotateSpeed * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
        // Масштабуємо: Базовий розмір * Фактор пульсації
        energyOrbModelMatrix = glm::scale(energyOrbModelMatrix, glm::vec3(orbBaseScale * currentPulseFactor));

        // --- Отримання даних орбіт ---
        std::vector<glm::vec3> lightPositions = orbitSystem.GetFireballPositions(currentFrame);
        std::vector<glm::mat4> fireballMatrices = orbitSystem.GetFireballMatrices(currentFrame, lightPositions);
        std::vector<glm::mat4> daggerMatrices = orbitSystem.GetDaggerMatrices(currentFrame);

        // --- Матриці камери ---
        glm::mat4 projection = glm::perspective(glm::radians(localCamera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, NEAR_PLANE, FAR_PLANE);
        glm::mat4 view = localCamera.GetViewMatrix();

        // ====================================================================================
        // 1. ОНОВЛЕННЯ СВІТЛА В ШЕЙДЕРАХ
        // ====================================================================================
        
        // Світло = База * Інтенсивність(від кнопок) * Пульсація(від розміру і пропорції)
        glm::vec3 currentOrbDiffuse = orbColorDiffuseBase * orbIntensity * lightPulseFactor;
        glm::vec3 currentOrbAmbient = orbColorAmbientBase * orbIntensity * lightPulseFactor; 
        
        // --- 1.1 ЦЕНТРАЛЬНЕ СВІТЛО (Index 0) ---
        auto setEnergyOrbLight = [&](Shader& s) {
            s.use();
            s.setVec3("pointLights[0].position", energyOrbPos);
            s.setVec3("pointLights[0].ambient",  currentOrbAmbient); 
            s.setVec3("pointLights[0].diffuse",  currentOrbDiffuse); 
            // Спекуляр теж пульсує
            s.setVec3("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 0.5f) * orbIntensity * lightPulseFactor); 
            s.setFloat("pointLights[0].constant", 1.0f);
            // Затухання залежить від загальної інтенсивності (чим яскравіше, тим менше затухання)
            s.setFloat("pointLights[0].linear", 0.14f / orbIntensity);  
            s.setFloat("pointLights[0].quadratic", 0.07f / orbIntensity); 
        };
        setEnergyOrbLight(ourShader);
        
        // Для трави вимикаємо блік (specular)
        planeShader.use();
        planeShader.setVec3("pointLights[0].position", energyOrbPos);
        planeShader.setVec3("pointLights[0].ambient",  currentOrbAmbient);
        planeShader.setVec3("pointLights[0].diffuse",  currentOrbDiffuse);
        planeShader.setVec3("pointLights[0].specular", glm::vec3(0.0f)); 
        planeShader.setFloat("pointLights[0].constant", 1.0f);
        planeShader.setFloat("pointLights[0].linear", 0.14f / orbIntensity);
        planeShader.setFloat("pointLights[0].quadratic", 0.07f / orbIntensity);


        // --- 1.2 ОРБІТАЛЬНІ КУЛІ (Index 1-8) ---
        for (int i = 0; i < 8; i++) { 
            std::string num = std::to_string(i + 1); 
            glm::vec3 pos = lightPositions[i]; 

            auto setFireballLightParams = [&](Shader& s) {
                s.use();
                s.setVec3("pointLights[" + num + "].position", pos);
                s.setVec3("pointLights[" + num + "].ambient",  glm::vec3(0.05f));
                s.setVec3("pointLights[" + num + "].diffuse",  glm::vec3(0.8f, 0.3f, 0.0f)); 
                s.setVec3("pointLights[" + num + "].specular", glm::vec3(1.0f, 0.5f, 0.0f)); 
                s.setFloat("pointLights[" + num + "].constant", 1.0f);
                s.setFloat("pointLights[" + num + "].linear", 0.09f);
                s.setFloat("pointLights[" + num + "].quadratic", 0.032f);
            };
            setFireballLightParams(ourShader);
            
            planeShader.use();
            planeShader.setVec3("pointLights[" + num + "].position", pos);
            planeShader.setVec3("pointLights[" + num + "].ambient",  glm::vec3(0.05f));
            planeShader.setVec3("pointLights[" + num + "].diffuse",  glm::vec3(0.8f, 0.3f, 0.0f)); 
            planeShader.setVec3("pointLights[" + num + "].specular", glm::vec3(0.0f)); 
            planeShader.setFloat("pointLights[" + num + "].constant", 1.0f);
            planeShader.setFloat("pointLights[" + num + "].linear", 0.09f);
            planeShader.setFloat("pointLights[" + num + "].quadratic", 0.032f);
        }

        // ====================================================================================
        // 2. РЕНДЕРИНГ
        // ====================================================================================

        // Очищення
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

        // Орбітальні Вогняні Кулі (Всі 8 штук!)
        brightShader.use();
        brightShader.setFloat("time", currentFrame);
        brightShader.setMat4("projection", projection);
        brightShader.setMat4("view", view);
        for (int i = 0; i < 8; i++) { 
            brightShader.setMat4("model", fireballMatrices[i]);
            sharedFireball.Draw(brightShader);
        }

        // Центральна Енергетична Сфера
        brightShader.setMat4("model", energyOrbModelMatrix);
        energyOrbModel.Draw(brightShader);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) std::cout << "OpenGL error: " << error << std::endl;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}