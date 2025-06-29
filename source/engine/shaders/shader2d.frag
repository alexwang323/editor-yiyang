#version 400

out vec4 FragColor;

in vec2 i_texcoord;

// non-pbr, but same binds for RenderComponent reuse
uniform vec3 u_diffuse;

uniform sampler2D s_albedo;
uniform bool u_usingalbedo;

void main() {

	vec3 Kd;

	if (u_usingalbedo) {
		Kd = texture(s_albedo, i_texcoord).rgb;
	} else {
		Kd = u_diffuse;
	}

	FragColor = vec4(Kd, 1.0);
}