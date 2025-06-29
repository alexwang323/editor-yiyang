#version 400

layout (location = 0) in vec4 a_vertex;

uniform mat4 u_lightprojection, u_lightview, u_model;

//out vec4 i_position;

void main() {
	gl_Position = u_lightprojection * u_lightview * u_model * a_vertex;

	//i_position = gl_Position;
}