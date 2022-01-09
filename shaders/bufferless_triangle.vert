#version 460 core
#extension GL_KHR_vulkan_glsl : enable
layout (location = 0) out vec2 outUV;
void main()
{
	float x = -1.0 + float((gl_VertexID & 1) << 2);
   float y = -1.0 + float((gl_VertexID & 2) << 1);
   outUV.x = (x + 1.0) * 0.5;
   outUV.y = (y + 1.0) * 0.5;
   gl_Position = vec4(x, y, 0, 1);
};