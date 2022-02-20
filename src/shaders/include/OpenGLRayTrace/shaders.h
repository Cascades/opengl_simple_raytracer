#pragma once
#include <filesystem>
#include <string>
#include <fstream>

namespace OGLRT
{
	template<GLenum shader_type>
	requires(shader_type == GL_VERTEX_SHADER ||
		shader_type == GL_FRAGMENT_SHADER ||
		shader_type == GL_COMPUTE_SHADER)
	class Shader
	{
	private:
		GLuint shader = 0;

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

	public:
		Shader(std::filesystem::path const& shader_path)
		{
			std::string shader_string_code;
			file_to_string(shader_path, shader_string_code);
			const char* c_string_code = shader_string_code.c_str();

			shader = glCreateShader(shader_type);
			glShaderSource(shader, 1, &c_string_code, NULL);
			glCompileShader(shader);
		}

		operator GLuint() const { return shader; }
	};

	class ShaderProgram
	{
		GLuint program = 0;

		template<typename container_type>
		requires(std::is_same_v<container_type, GLuint> ||
			std::is_same_v<typename container_type::value_type, GLuint>)
			void create_shader_program(container_type shaders)
		{
			program = glCreateProgram();

			for (auto const& shader : shaders)
			{
				glAttachShader(program, shader);
			}

			glLinkProgram(program);

			GLint Success = 0;
			GLchar ErrorLog[1024] = { 0 };

			glGetProgramiv(program, GL_LINK_STATUS, &Success);

			if (Success == 0) {
				glGetProgramInfoLog(program, sizeof(ErrorLog), NULL, ErrorLog);
				fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
			}
		}

		template<>
		void create_shader_program(GLuint shader)
		{
			create_shader_program<std::array<GLuint, 1>>({ shader });
		}

	public:
		template<typename container_type>
		requires(std::is_same_v<container_type, GLuint> ||
			std::is_same_v<typename container_type::value_type, GLuint>)
			ShaderProgram(container_type shaders)
		{
			create_shader_program(shaders);
		}

		GLuint get_id()
		{
			return program;
		}

		operator GLuint() const { return program; }
	};

	template<GLenum shader_type, GLenum usage>
	requires(shader_type == GL_UNIFORM_BUFFER ||
		shader_type == GL_SHADER_STORAGE_BUFFER ||
		usage == GL_STREAM_DRAW ||
		usage == GL_STREAM_READ ||
		usage == GL_STREAM_COPY ||
		usage == GL_STATIC_DRAW ||
		usage == GL_STATIC_READ ||
		usage == GL_STATIC_COPY ||
		usage == GL_DYNAMIC_DRAW ||
		usage == GL_DYNAMIC_READ ||
		usage == GL_DYNAMIC_COPY)
		class BufferObject
	{
		GLuint buffer_object;

	public:

		BufferObject(GLsizeiptr size = 0)
		{
			glGenBuffers(1, &buffer_object);
			glBindBuffer(shader_type, buffer_object);
			glBufferData(shader_type, size, NULL, usage);
			glBindBuffer(shader_type, 0);
		}

		void buffer_object_block_binding(ShaderProgram program, std::string_view uniform_block_name, GLuint uniform_block_binding)
		{
			glBindBuffer(shader_type, buffer_object);
			glUniformBlockBinding(program.get_id(), glGetUniformBlockIndex(program.get_id(), uniform_block_name.data()), uniform_block_binding);
			glBindBufferBase(shader_type, uniform_block_binding, buffer_object);
			glBindBuffer(shader_type, 0);
		}

		void buffer_object_buffer_sub_data(GLintptr offset, GLsizeiptr size, void const* data)
		{
			glNamedBufferSubData(buffer_object, offset, size, data);
		}

		GLuint get_id()
		{
			return buffer_object;
		}

		operator GLuint() const { return buffer_object; }
	};
}