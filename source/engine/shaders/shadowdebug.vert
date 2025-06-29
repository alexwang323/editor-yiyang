#version 400

layout (location = 0) in vec4 a_vertex;
layout (location = 1) in vec2 a_vertextexture;

out vec2 i_texcoord;

void main() {
	i_texcoord = a_vertextexture;
	gl_Position = a_vertex;
}