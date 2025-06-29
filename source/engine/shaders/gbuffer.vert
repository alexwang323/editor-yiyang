#version 400

uniform mat4 u_view, u_viewinverse, u_projection, u_model, u_normaltransform;

layout (location = 0) in vec4 a_vertex;
layout (location = 1) in vec3 a_vertexnormal;
layout (location = 2) in vec2 a_vertextexture;
layout (location = 3) in vec3 a_vertextangent;

out vec3 i_worldpos, i_normalvector, i_tanvector;
out vec2 i_texcoord;

void main() {
	gl_Position = u_projection * u_view * u_model * a_vertex;

	i_worldpos = (u_model * a_vertex).xyz;

	i_normalvector = a_vertexnormal * mat3(u_normaltransform);

	i_tanvector = mat3(u_model) * a_vertextangent;

	i_texcoord = a_vertextexture;
}