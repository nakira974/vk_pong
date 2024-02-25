#ifndef GLFW_FUN_H
#define GLFW_FUN_H

#include "std_c.h"
#include "ext.h"

/**
 * @brief Create a Vulkan window using GLFW.
 * @param width The width of the window
 * @param height The height of the window
 * @param title The title of the window
 * @return A pointer to the created Vulkan window
 */
GLFWwindow *window_create(int width, int height, const char *title);

/**
 * @brief Deletes a Vulkan window created with GLFW.
 * @param pWindow Pointer to the Vulkan window to be deleted
 */
void window_destroy(GLFWwindow *pWindow);

#endif // GLFW_FUN_H
