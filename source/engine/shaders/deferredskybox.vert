#version 400

layout (location = 0) in vec3 a_vertex;

out vec3 i_texCoord;

uniform mat4 u_projection;
uniform mat4 u_view;

void main() {
	i_texCoord = a_vertex;
	vec4 pos = u_projection * u_view * vec4(a_vertex,1.0);
	gl_Position = pos.xyww;
}