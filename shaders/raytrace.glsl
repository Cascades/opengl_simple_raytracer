#version 460

layout(local_size_x = 32, local_size_y = 32) in;

layout(std430) buffer;

layout(binding = 0, location = 0, rgba32f) uniform image2D output_texture;

layout(binding = 1) buffer StateDataSSBO {
	vec4 viewPos;
	uvec2 screen_size;
	int iFrame;
} state_data;

layout(binding = 2) buffer SceneDataSSBO {
	int lightcuts[];
} scene_data;

void main()
{
	ivec2 storePos = ivec2(gl_GlobalInvocationID.xy); \
	imageStore(output_texture, storePos, vec4(0.0, 0.0, 1.0, 1.0));
}