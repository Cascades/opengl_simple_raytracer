#pragma once
#include "OpenGLRayTrace/camera.h"

class Window
{
public:
	double last_x = 0.0;
	double last_y = 0.0;
	bool first_mouse = true;
	float movement_speed = 0.1f;

	glm::uint32 screen_width = 800;
	glm::uint32 screen_height = 600;

	std::shared_ptr<OGLRT::Camera> camera = std::make_shared<OGLRT::Camera>();
	GLFWwindow* window;

	Window()
	{
		// glfw window creation
		// --------------------
		window = glfwCreateWindow(screen_width, screen_height, "LearnOpenGL", NULL, NULL);
		if (window == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return;
		}
		glfwMakeContextCurrent(window);

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		glfwSetWindowUserPointer(window, reinterpret_cast<void*>(this));
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		glfwSetCursorPosCallback(window, mouse_callback);
	}

	void processInput()
	{
		float local_movement_speed = movement_speed;
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{
			local_movement_speed *= 10.0f;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		{
			local_movement_speed /= 10.0f;
		}

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, true);
		}
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			camera->pos += camera->dir * local_movement_speed;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			camera->pos -= camera->right * local_movement_speed;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			camera->pos -= camera->dir * local_movement_speed;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			camera->pos += camera->right * local_movement_speed;
		}
	}

	// glfw: whenever the window size changed (by OS or user resize) this callback function executes
	// ---------------------------------------------------------------------------------------------
	static void framebuffer_size_callback([[maybe_unused]] GLFWwindow* window, int width, int height)
	{
		// make sure the viewport matches the new window dimensions; note that width and 
		// height will be significantly larger than specified on retina displays.
		glViewport(0, 0, width, height);
	}

	static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
	{
		Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
		if (win->first_mouse)
		{
			win->last_x = static_cast<float>(xpos);
			win->last_y = static_cast<float>(ypos);
			win->first_mouse = false;
		}

		double xoffset = xpos - win->last_x;
		double yoffset = win->last_y - ypos;

		win->last_x = xpos;
		win->last_y = ypos;

		win->camera->ProcessMouseMovement(static_cast<float>(xoffset), static_cast<float>(yoffset));
	}
};