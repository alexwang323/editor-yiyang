#version 400

layout (location = 0) out vec3 g_position;
layout (location = 1) out vec3 g_normal;
layout (location = 2) out vec4 g_albedo;
layout (location = 3) out vec4 g_specular;

// default materials
uniform vec3 u_diffuse, u_specular;
uniform float u_roughness;

// textured materials
uniform sampler2D s_albedo;
uniform bool u_usingalbedo;

uniform sampler2D s_normal;
uniform bool u_usingnormal;

uniform sampler2D s_specular;
uniform bool u_usingspecular;

uniform sampler2D s_roughness;
uniform bool u_usingroughness;

uniform bool u_precalctangents;

in vec3 i_worldpos, i_normalvector, i_tanvector;
in vec2 i_texcoord;

void main() {
	// with displacement mapping, this gets changed
	g_position = i_worldpos;

	vec3 N = normalize(i_normalvector);

	vec3 T;
    if (u_precalctangents) {
		T = normalize(i_tanvector);
    } else {
        vec3 q1 = dFdx(i_worldpos);
        vec3 q2 = dFdy(i_worldpos);
        vec2 st1 = dFdx(i_texcoord);
        vec2 st2 = dFdy(i_texcoord);
        T = normalize(q1 * st2.t - q2 * st1.t);
    }
    vec3 B = normalize(cross(T,N));

	if (u_usingnormal) {
        vec3 delta = texture(s_normal, i_texcoord).xyz * 2.0 - 1.0;
        N = normalize(delta.x * T + delta.y * B + delta.z * N);
	}

	g_normal.xyz = N.xyz;

	if (u_usingalbedo) {
		g_albedo.rgb = texture(s_albedo, i_texcoord).rgb;
	} else {
		g_albedo.rgb = u_diffuse;
	}

	if (u_usingspecular) {
		g_specular.rgb = texture(s_specular, i_texcoord).rgb;
	} else {
		g_specular.rgb = u_specular.rgb;
	}

	if (u_usingroughness) {
		g_specular.a = texture(s_roughness, i_texcoord).r;
	} else {
		g_specular.a = u_roughness;
	}

}