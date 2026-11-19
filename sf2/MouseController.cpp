#include "MouseController.h"

void MouseController::onSetCursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    static bool first = true;

    const float x = static_cast<float>(xpos);
    const float y = static_cast<float>(ypos);

    if (first)
    {
        g_lastX = x;
        g_lastY = y;
        first = false;
    }

    const float xoffset = x - g_lastX;
    const float yoffset = g_lastY - y;

    g_lastX = x;
    g_lastY = y;

    g_dx += xoffset;
    g_dy += yoffset;
}

void MouseController::onSetMouseButtonCallback(GLFWwindow*, int btn, int action, int)
{
    if (btn == GLFW_MOUSE_BUTTON_LEFT)
    {
        g_leftDown = (action == GLFW_PRESS);
    }
    if (btn == GLFW_MOUSE_BUTTON_RIGHT)
    {
        g_rightDown = (action == GLFW_PRESS);
    }
}

void MouseController::onScrollCallback(GLFWwindow*, double, double yoffset)
{
    g_scroll += (float)yoffset * 0.1f; // akumulacja, jak dla ruchu kursora
    g_scrolled = true;
}

void MouseController::flush()
{
    g_leftClicked = g_leftWasDown && !g_leftDown;
    g_leftWasDown = g_leftDown;
    g_dx = g_dy = g_scroll = 0;
    g_scrolled = false;
}