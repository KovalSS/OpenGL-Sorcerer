#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#include "Shader.h"
#include "Model.h"
#include "Camera.h"
#include "Skybox.h"
#include "Callbacks.h"
#include "InputHandler.h"
#include "Globals.h"

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
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Завантаження GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Налаштування OpenGL
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // Завантаження шейдерів
    Shader ourShader("../resources/shaders/vertex.shader", "../resources/shaders/fragment.shader");
    
    // Завантаження моделі
    std::cout << "Loading model..." << std::endl;
    Model ourModel("../resources/models/doctor-strenge/doctor-strenge.obj");
    
    Shader skyboxShader("../resources/shaders/skybox.vert", "../resources/shaders/skybox.frag");
    
    // Завантажуємо cubemap текстури
    std::vector<std::string> faces = {
        "../resources/textures/skybox/nx.png",
        "../resources/textures/skybox/nz.png",
        "../resources/textures/skybox/py.png", 
        "../resources/textures/skybox/ny.png",
        "../resources/textures/skybox/px.png",
        "../resources/textures/skybox/nx.png"
    };
    
    Skybox skybox(faces, SIZE_SKY);
    
    while (!glfwWindowShouldClose(window)) {
        // Таймінги
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Обробка вводу
        processInput(window);

        // Очищення буферів
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // --- МАЛЮЄМО SKYBOX ПЕРШИМ ---
        skyboxShader.use();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 
                                            (float)SCR_WIDTH / (float)SCR_HEIGHT, 
                                            NEAR_PLANE, FAR_PLANE);
        glm::mat4 view = camera.GetViewMatrix();
        skybox.Draw(skyboxShader, view, projection);

        // --- МАЛЮЄМО DOCTOR STRANGE ---
        ourShader.use();
        ourShader.setFloat("time", glfwGetTime());
        
        // Освітлення
        ourShader.setVec3("lightPos", glm::vec3(2.0f, 5.0f, 2.0f));
        ourShader.setVec3("viewPos", camera.Position);

        // Матриці перетворення
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
        model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));

        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        ourShader.setMat4("model", model);

        // Малювання моделі
        ourModel.Draw(ourShader);

        // Перевірка помилок OpenGL
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cout << "OpenGL error: " << error << std::endl;
        }

        // Обмін буферів та опитування подій
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}