#version 330 core

uniform mat4 u_view, u_viewinverse, u_projection, u_model, u_normaltransform;
uniform vec3 u_lightpos;

layout (location = 0) in vec4 a_vertex;
layout (location = 1) in vec3 a_vertexnormal;
layout (location = 2) in vec2 a_vertextexture;
layout (location = 3) in vec3 a_vertextangent;

out vec3 i_normalvector, i_lightvector, i_eyevector;
out vec2 i_texcoord;

void main() {
	gl_Position = u_projection * u_view * u_model * a_vertex;

	vec3 worldPos = (u_model * a_vertex).xyz;

	i_normalvector = a_vertexnormal * mat3(u_normaltransform);

	i_lightvector = u_lightpos - worldPos;

	i_eyevector = (u_viewinverse*vec4(0,0,0,1)).xyz - worldPos;

	i_texcoord = a_vertextexture;
}