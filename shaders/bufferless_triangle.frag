#version 460 core
layout (binding = 0, location = 0) uniform sampler2D input_texture;
layout (location = 0) out vec4 FragColor;
layout (location = 0) in vec2 inUV;
void main()
{
	FragColor = texture(input_texture, inUV);
};