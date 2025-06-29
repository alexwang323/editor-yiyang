#version 400

out vec4 FragColor;

in vec3 i_texCoord;

uniform samplerCube s_cubemap;

void main() {
	FragColor = texture(s_cubemap, i_texCoord);
	//FragColor.xyz = vec3(1.0, 1.0, 1.0);
}