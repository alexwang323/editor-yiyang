#version 400

out vec4 FragColor;

in vec2 i_texcoord;

uniform sampler2D s_buffer;

void main() {
	
	//float depthValue = texture(s_depthmap, i_texcoord).r;


	//if (depthValue != 0.0)
	//	FragColor = vec4(vec3(depthValue), 1.0);
	//else
	//	FragColor.xz = i_texcoord;

	//FragColor = vec4(vec3(depthValue), 1.0);

	FragColor = vec4(texture(s_buffer, i_texcoord).rgb, 1.0);
}