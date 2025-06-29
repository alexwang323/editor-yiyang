#version 400
precision mediump float;
uniform mat4 u_view, u_projection;
layout (location = 0) in vec3 i_Pos;
layout (location = 1) in float i_Age;
layout (location = 2) in float i_Life;
layout (location = 3) in vec3 i_Vel;

void main()
{
  gl_PointSize = 10.0*(i_Age)/(i_Life+0.001f)+5.0f;
  gl_Position = u_projection * u_view * vec4(i_Pos,1);
}
