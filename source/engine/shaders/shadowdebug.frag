#version 400

out vec4 FragColor;

in vec2 i_texcoord;

uniform sampler2D s_depthmap;
uniform float u_near;
uniform float u_far;

float LinearizeDepth(float depth) {
	
	float z = depth * 2.0 - 1.0;
	return (2.0 * u_near * u_far) / (u_far + u_near - z * (u_far - u_near));

}

void main() {
	
	float depthValue = texture(s_depthmap, i_texcoord).r;

	if (depthValue != 0.0)
		FragColor = vec4(vec3(depthValue), 1.0);
	else
		FragColor.xz = i_texcoord;

	//FragColor = vec4(vec3(depthValue), 1.0);
}