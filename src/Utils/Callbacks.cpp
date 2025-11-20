#include "Callbacks.h"
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    WindowContext* context = static_cast<WindowContext*>(glfwGetWindowUserPointer(window));
    if (!context || !context->camera) return; 

    if (context->firstMouse) {
        context->lastX = (float)xpos;
        context->lastY = (float)ypos;
        context->firstMouse = false;
    }

    float xoffset = (float)xpos - context->lastX;
    float yoffset = context->lastY - (float)ypos; 
    context->lastX = (float)xpos;
    context->lastY = (float)ypos;

    context->camera->ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    WindowContext* context = static_cast<WindowContext*>(glfwGetWindowUserPointer(window));
    if (!context || !context->camera) return; 
    
    context->camera->ProcessMouseScroll((float)yoffset);
}