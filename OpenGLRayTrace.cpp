#include "OpenGLRayTrace.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <filesystem>

// Minimal test example from: https://github.com/litasa/Minimal-OpenGL-GLFW-GLEW

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void file_to_string(std::filesystem::path const& shader_path, std::string& out_string)
{
	std::ifstream shader_file_stream;
	shader_file_stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	shader_file_stream.open(shader_path);
	std::stringstream shader_string_stream;
	shader_string_stream << shader_file_stream.rdbuf();
	shader_file_stream.close();
	out_string = shader_string_stream.str();
}

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	//init glew after the context have been made
	glewInit();


	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	GLuint compute_output_texture;
	glGenTextures(1, &compute_output_texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, compute_output_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
	glBindImageTexture(0, compute_output_texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	std::string ray_code;
	file_to_string(std::filesystem::current_path().parent_path() / "shaders/raytrace.glsl", ray_code);
	const char* ray_c_string_code = ray_code.c_str();

	GLuint ray_shader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(ray_shader, 1, &ray_c_string_code, NULL);
	glCompileShader(ray_shader);

	GLuint ray_program = glCreateProgram();
	glAttachShader(ray_program, ray_shader);
	glLinkProgram(ray_program);

	std::string vert_code;
	file_to_string(std::filesystem::current_path().parent_path() / "shaders/bufferless_triangle.vert", vert_code);
	const char* vert_c_string_code = vert_code.c_str();

	std::string frag_code;
	file_to_string(std::filesystem::current_path().parent_path() / "shaders/bufferless_triangle.frag", frag_code);
	const char* frag_c_string_code = frag_code.c_str();

	GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert_shader, 1, &vert_c_string_code, NULL);
	glCompileShader(vert_shader);

	GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_shader, 1, &frag_c_string_code, NULL);
	glCompileShader(frag_shader);

	GLuint presentation_program = glCreateProgram();
	glAttachShader(presentation_program, vert_shader);
	glAttachShader(presentation_program, frag_shader);
	glLinkProgram(presentation_program);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	glGetProgramiv(presentation_program, GL_LINK_STATUS, &Success);

	if (Success == 0) {
		glGetProgramInfoLog(presentation_program, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
	}

	unsigned int dummy_vao;
	glGenVertexArrays(1, &dummy_vao);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// input
		// -----
		processInput(window);

		glUseProgram(ray_program);
		glUniform1i(0, 0); //program must be active

		glDispatchCompute(SCR_WIDTH / 16, SCR_HEIGHT / 16, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		glUseProgram(0);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(presentation_program);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, compute_output_texture);

		glBindVertexArray(dummy_vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glUseProgram(0);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}
