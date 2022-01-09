#version 460

layout(local_size_x = 32, local_size_y = 32) in;

struct Sphere
{
	vec3 cen;
	float rad;
};

layout(std430) buffer;

layout(binding = 0, location = 0, rgba32f) uniform image2D output_texture;

layout(binding = 1) buffer StateDataSSBO {
	vec4 cam_pos;
	vec4 cam_dir;
	uvec2 screen_size;
	int iFrame;
} state_data;

layout(binding = 2) buffer SceneDataSSBO {
	int spheres[];
} scene_data;

bool sphere_intersect(in vec3 ray_pos, in vec3 ray_dir, in vec3 sph_cen, in float sph_rad, inout float t)
{
	vec3 omc = ray_pos - sph_cen;
	float b = dot(ray_dir, omc);
	float c = dot(omc, omc) - sph_rad * sph_rad;
	float det = b * b - c;
	if (det >= 0.0)
	{
		t = min(-b - sqrt(det), -b + sqrt(det));
		return true;
	}
	return false;
}

void main()
{
	Sphere test_sphere;
	test_sphere.cen = vec3(0.0, 0.0, -10.0);
	test_sphere.rad = 6.0;

	ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);
	vec2 uv = storePos / vec2(800.0, 600.0);

	vec3 cam_pos = vec3(0.0, 0.0, 0.0);
	vec3 cam_forward = vec3(0.0, 0.0, -1.0);
	vec3 cam_left = vec3(1.0, 0.0, 0.0);
	vec3 cam_up = vec3(0.0, 1.0, 0.0);
	vec2 cam_size = vec2(8.0, 6.0);

	ivec2 dims = imageSize(output_texture); // fetch image dimensions
	vec3 x = (float(storePos.x * 2 - dims.x) / dims.x) * cam_size.x * cam_left;
	vec3 y = (float(storePos.y * 2 - dims.y) / dims.y) * cam_size.y * cam_up;
	vec3 ray_pos = cam_pos + x + y;
	vec3 ray_dir = cam_forward; // ortho

	float t;
	bool hit = sphere_intersect(ray_pos, ray_dir, test_sphere.cen, test_sphere.rad, t);
	// hit one or both sides
	if (hit) {
		imageStore(output_texture, storePos, vec4(t/10.0, 0.0, 0.0, 1.0));
	}
	else
	{
		imageStore(output_texture, storePos, vec4(0.0, 0.0, 0.0, 1.0));
	}

	//imageStore(output_texture, storePos, vec4(ray_pos.x, ray_pos.y, 0.0, 1.0));
}