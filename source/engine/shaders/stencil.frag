#version 400

//out vec4 FragColor;

uniform sampler2DMS g_position_ms;
uniform sampler2DMS g_normal_ms;
uniform sampler2DMS g_albedo_ms;
uniform sampler2DMS g_specular_ms;

uniform int u_samples;

in vec2 i_texcoord;

#define STENCIL_EPSILON 0.5

void main() {
	
	float depth_diff = 0.0;
	vec3 nrm_diff = vec3(0.0);
	vec4 col_diff = vec4(0.0);

	// comparing diff between pairs, accumulate in _diff vectors
	for (int i = 1; i < u_samples; ++i) {
		ivec2 uv_pos = ivec2(i_texcoord * vec2(textureSize(g_position_ms)));
		ivec2 uv_nrm = ivec2(i_texcoord * vec2(textureSize(g_normal_ms)));
		ivec2 uv_col = ivec2(i_texcoord * vec2(textureSize(g_albedo_ms)));

		vec4 pos_a = texelFetch(g_position_ms, uv_pos, i-1);
		vec4 pos_b = texelFetch(g_position_ms, uv_pos, i);
		depth_diff += abs(pos_a.z - pos_b.z);

		vec4 nrm_a = texelFetch(g_normal_ms, uv_nrm, i-1);
		vec4 nrm_b = texelFetch(g_normal_ms, uv_nrm, i);
		nrm_diff += abs(nrm_a.xyz - nrm_b.xyz);
		
		vec4 col_a = texelFetch(g_albedo_ms, uv_col, i-1);
		vec4 col_b = texelFetch(g_albedo_ms, uv_col, i);
		col_diff += abs(col_a - col_b);
	}

	//debug stencil view
	//if (depth_diff + length(nrm_diff) + length(col_diff) > STENCIL_EPSILON) {
	//	FragColor.xyz = vec3(1.0);
	//} else {
	//	FragColor.xyz = vec3(0.0);
	//}
	//return;

	if (depth_diff + length(nrm_diff) + length(col_diff) < STENCIL_EPSILON) {
		discard;
	}
}