#version 460

layout(local_size_x = 32, local_size_y = 32) in;

layout(std430) buffer;

layout(binding = 0, location = 0, rgba32f) uniform image2D output_texture;

layout(std140, binding = 1) uniform StateDataUBO {
	vec4 cam_pos;
	vec4 cam_dir;
	vec4 cam_up;
	uvec2 screen_size;
	int iFrame;
} state_data;

layout(binding = 2) buffer SpherePosSSBO {
	vec4 sphere_pos[];
} sphere_pos_data;

layout(binding = 3) buffer SphereRadSSBO {
	float sphere_rads[];
} sphere_rad_data;

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

void get_ortho_ray(in ivec2 storePos, in vec2 cam_size, in vec3 cam_left, in vec3 cam_up, in vec3 cam_forward, out vec3 ray_pos, out vec3 ray_dir)
{
	ivec2 dims = imageSize(output_texture); // fetch image dimensions
	vec3 x = (float(storePos.x * 2 - dims.x) / dims.x) * cam_size.x * cam_left;
	vec3 y = (float(storePos.y * 2 - dims.y) / dims.y) * cam_size.y * cam_up;
	ray_pos = state_data.cam_pos.xyz + x + y;
	ray_dir = cam_forward; // ortho
}

void get_proj_ray(in ivec2 storePos, in vec2 cam_size, in vec3 cam_left, in vec3 cam_up, in vec3 cam_forward, out vec3 ray_pos, out vec3 ray_dir)
{
	ivec2 dims = imageSize(output_texture); // fetch image dimensions
	vec3 x = (float(storePos.x * 2 - dims.x) / dims.x) * cam_size.x * cam_left;
	vec3 y = (float(storePos.y * 2 - dims.y) / dims.y) * cam_size.y * cam_up;
	ray_pos = state_data.cam_pos.xyz;
	ray_dir = normalize(x + y + cam_forward * 10.0); // proj
}

void main()
{
	ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);
	imageStore(output_texture, storePos, vec4(0.0, 0.0, 0.0, 1.0));

	vec2 uv = storePos / vec2(800.0, 600.0);

	vec2 cam_size = vec2(8.0, 6.0);

	vec3 ray_pos;
	vec3 ray_dir;

	get_proj_ray(storePos, cam_size, -normalize(cross(state_data.cam_up.xyz, state_data.cam_dir.xyz)), state_data.cam_up.xyz, state_data.cam_dir.xyz, ray_pos, ray_dir);

	float max_t = 10000000000000000.0;

	for (int sphere = 0; sphere < sphere_pos_data.sphere_pos.length(); ++sphere)
	{
		vec3 test_sphere_pos = sphere_pos_data.sphere_pos[sphere].xyz;
		float test_sphere_rad = sphere_rad_data.sphere_rads[sphere];

		float t;
		bool hit = sphere_intersect(ray_pos, ray_dir, test_sphere_pos, test_sphere_rad / 2.0, t);
		// hit one or both sides
		if (hit && t < max_t) {
			max_t = t;
			imageStore(output_texture, storePos, vec4(t / 10.0, 0.0, 0.0, 1.0));
		}
	}
}