#version 400

uniform mat4 u_view, u_projection, u_model;

layout (location = 0) in vec4 a_vertex;
layout (location = 1) in vec2 a_vertextexture;

out vec2 i_texcoord;

void main()
{
    gl_Position = u_projection * u_view * u_model * a_vertex;

    i_texcoord = a_vertextexture;
}