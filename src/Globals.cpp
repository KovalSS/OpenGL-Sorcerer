#include "Globals.h"
#include <glm/glm.hpp>

// Визначення глобальних змінних
Camera camera(glm::vec3(0.0f, 1.0f, 5.0f));
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;