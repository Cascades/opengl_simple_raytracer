#include "OpenGLRayTrace.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <iostream>
#include <filesystem>
#include <array>
#include <memory>
#include <vector>

#include "OpenGLRayTrace/camera.h"
#include "OpenGLRayTrace/window.h"
#include "OpenGLRayTrace/filesystem.h"
#include "OpenGLRayTrace/shaders.h"

void opengl_debug_callback([[maybe_unused]] GLenum source,
	[[maybe_unused]] GLenum type,
	[[maybe_unused]] GLuint id,
	[[maybe_unused]] GLenum severity,
	[[maybe_unused]] GLsizei length,
	[[maybe_unused]] const GLchar* message,
	[[maybe_unused]] const void* userParam)
{
	std::cout << "GL Error:" << std::endl;
	std::cout << message << std::endl;
}

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

	scene->spheres.pos.emplace_back(0.0f);
	scene->spheres.rads.emplace_back(1000.0f);

	for (int i = 0; i < 5; ++i)
	{
		scene->spheres.pos.emplace_back(-12.0f + static_cast<float>(i) * 6.0f, 0.0f, -10.0f, 0.0f);
		scene->spheres.rads.emplace_back(6.0f);
	}

	StateData state_data{};

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	auto win = std::make_shared<Window>();
	
	glewInit();

	glDebugMessageCallback(opengl_debug_callback, nullptr);

	GLuint compute_output_texture;
	glGenTextures(1, &compute_output_texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, compute_output_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, win->screen_width, win->screen_height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glBindImageTexture(0, compute_output_texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	OGLRT::Shader<GL_COMPUTE_SHADER> ray_shader{ OGLRT::get_shaders_directory() / "raytrace.glsl" };
	OGLRT::ShaderProgram ray_program{ static_cast<GLuint>(ray_shader) };

	OGLRT::Shader<GL_VERTEX_SHADER> vert_shader{ OGLRT::get_shaders_directory() / "bufferless_triangle.vert" };
	OGLRT::Shader<GL_FRAGMENT_SHADER> frag_shader{ OGLRT::get_shaders_directory() / "bufferless_triangle.frag" };
	OGLRT::ShaderProgram presentation_program{ std::array<GLuint, 2>{vert_shader, frag_shader} };

	unsigned int dummy_vao;
	glGenVertexArrays(1, &dummy_vao);

	OGLRT::BufferObject<GL_UNIFORM_BUFFER, GL_STATIC_DRAW> state_data_ubo{
		sizeof(OGLRT::Camera) };
	state_data_ubo.buffer_object_block_binding(ray_program, "StateDataUBO", 1);
	state_data_ubo.buffer_object_buffer_sub_data(0, 
		sizeof(OGLRT::Camera),
		win->camera.get());

	OGLRT::BufferObject<GL_SHADER_STORAGE_BUFFER, GL_STATIC_DRAW> scene_sphere_pos_ssbo{
		static_cast<GLsizeiptr>(scene->spheres.pos.size() * sizeof(decltype(scene->spheres.pos)::value_type)) };
	scene_sphere_pos_ssbo.buffer_object_block_binding(ray_program, "SpherePosSSBO", 2);
	scene_sphere_pos_ssbo.buffer_object_buffer_sub_data(0, 
		scene->spheres.pos.size() * sizeof(decltype(scene->spheres.pos)::value_type),
		scene->spheres.pos.data());

	OGLRT::BufferObject<GL_SHADER_STORAGE_BUFFER, GL_STATIC_DRAW> scene_sphere_rad_ssbo{
		static_cast<GLsizeiptr>(scene->spheres.rads.size() * sizeof(decltype(scene->spheres.rads)::value_type)) };
	scene_sphere_rad_ssbo.buffer_object_block_binding(ray_program, "SphereRadSSBO", 3);
	scene_sphere_rad_ssbo.buffer_object_buffer_sub_data(0,
		scene->spheres.rads.size() * sizeof(decltype(scene->spheres.rads)::value_type),
		scene->spheres.rads.data());

	unsigned int sphere_map_tex;
	glGenTextures(1, &sphere_map_tex);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, sphere_map_tex);

	int width, height, nrChannels;
	std::cout << "Loading image into main memory" << std::endl;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load("assets/blaubeuren_church_square_4k.png", &width, &height, &nrChannels, 0);
	std::cout << "Image loaded with:" << std::endl;
	std::cout << "|_ width: " << width << std::endl;
	std::cout << "|_ height: " << height << std::endl;
	std::cout << "|_ channels: " << nrChannels << std::endl;

	if (data)
	{
		std::cout << "Loading image into GPU memory" << std::endl;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		std::cout << "Texture generated" << std::endl;
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	std::cout << "CPU image freed" << std::endl;

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, sphere_map_tex);

	glUseProgram(ray_program.get_id());
	glUniform1i(glGetUniformLocation(ray_program.get_id(), "sphere_map"), 1);

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
		state_data.screen_size = glm::uvec2(win->screen_width, win->screen_height);
		state_data.iFrame = 0;

		glBindBuffer(GL_UNIFORM_BUFFER, state_data_ubo.get_id());
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(StateData), &state_data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glUseProgram(ray_program.get_id());
		glUniform1i(0, 0); //program must be active

		glDispatchCompute(win->screen_width / 16, win->screen_height / 16, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		glUseProgram(0);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(presentation_program.get_id());
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