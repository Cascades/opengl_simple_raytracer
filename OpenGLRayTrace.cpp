#include "OpenGLRayTrace.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <fstream>
#include <filesystem>
#include <array>

#include "OpenGLRayTrace/camera.h"

// Minimal test example from: https://github.com/litasa/Minimal-OpenGL-GLFW-GLEW

//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void processInput(GLFWwindow* window);

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

class Window
{
public:
	double last_x = 0.0;
	double last_y = 0.0;
	bool first_mouse = true;

	std::shared_ptr<OGLRT::Camera> camera = std::make_shared<OGLRT::Camera>();
	GLFWwindow* window;

	Window()
	{
		// glfw window creation
		// --------------------
		window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
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
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, true);
		}
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			camera->pos += camera->dir * 0.1f;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			camera->pos -= camera->right * 0.1f;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			camera->pos -= camera->dir * 0.1f;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			camera->pos += camera->right * 0.1f;
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

struct Spheres
{
	std::vector<glm::vec4> pos;
	std::vector<glm::float32> rads;
};

struct StateData
{
	glm::vec4 cam_pos;
	glm::vec4 cam_dir;
	glm::vec4 cam_up;
	glm::uvec2 screen_size;
	glm::int64 iFrame;
};

class Scene
{
public:
	Spheres spheres;
};

int main()
{
	auto scene = std::make_shared<Scene>();

	for (int i = 0; i < 5; ++i)
	{
		scene->spheres.pos.emplace_back(-12.0f + static_cast<float>(i) * 6.0f, 0.0f, -10.0f, 0.0f);
		scene->spheres.rads.emplace_back(6.0f);
	}

	StateData state_data{};

	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X

	auto win = std::make_shared<Window>();
	
	//init glew after the context have been made
	glewInit();


	//glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

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

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	glGetProgramiv(ray_program, GL_LINK_STATUS, &Success);

	if (Success == 0) {
		glGetProgramInfoLog(ray_program, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
	}

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

	Success = 0;

	glGetProgramiv(presentation_program, GL_LINK_STATUS, &Success);

	if (Success == 0) {
		glGetProgramInfoLog(presentation_program, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
	}

	unsigned int dummy_vao;
	glGenVertexArrays(1, &dummy_vao);

	unsigned int state_data_ubo;
	glGenBuffers(1, &state_data_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, state_data_ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(StateData), NULL, GL_STATIC_DRAW); // allocate 152 bytes of memory
	glUniformBlockBinding(ray_program, glGetUniformBlockIndex(ray_program, "StateDataUBO"), 1);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, state_data_ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(*win->camera), win->camera.get());
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	unsigned int scene_sphere_pos_ssbo;
	glGenBuffers(1, &scene_sphere_pos_ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, scene_sphere_pos_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, scene->spheres.pos.size() * sizeof(decltype(scene->spheres.pos)::value_type), NULL, GL_STATIC_DRAW); // allocate 152 bytes of memory
	glUniformBlockBinding(ray_program, glGetUniformBlockIndex(ray_program, "SpherePosSSBO"), 2);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, scene_sphere_pos_ssbo);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, scene->spheres.pos.size() * sizeof(decltype(scene->spheres.pos)::value_type), scene->spheres.pos.data());
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	unsigned int scene_sphere_rad_ssbo;
	glGenBuffers(1, &scene_sphere_rad_ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, scene_sphere_rad_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, scene->spheres.rads.size() * sizeof(decltype(scene->spheres.rads)::value_type), NULL, GL_STATIC_DRAW); // allocate 152 bytes of memory
	glUniformBlockBinding(ray_program, glGetUniformBlockIndex(ray_program, "SphereRadSSBO"), 3);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, scene_sphere_rad_ssbo);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, scene->spheres.rads.size() * sizeof(decltype(scene->spheres.rads)::value_type), scene->spheres.rads.data());
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(win->window))
	{
		// input
		// -----
		win->processInput();

		state_data.cam_pos = glm::vec4(win->camera->pos, 0.0f);
		state_data.cam_dir = glm::vec4(win->camera->dir, 0.0f);
		state_data.cam_up = glm::vec4(win->camera->up, 0.0f);
		state_data.screen_size = glm::uvec2(SCR_WIDTH, SCR_HEIGHT);
		state_data.iFrame = 0;

		glBindBuffer(GL_UNIFORM_BUFFER, state_data_ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(StateData), &state_data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

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
		glfwSwapBuffers(win->window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}