#version 400
precision mediump float;

layout (location = 0) in vec2 i_Pos;
layout (location = 1) in float i_Age;
layout (location = 2) in float i_Life;
layout (location = 3) in vec2 i_Vel;

void main()
{
  gl_PointSize = 10.0*(i_Life-i_Age+1.0)/(i_Life+1.0);
  gl_Position = vec4(i_Pos,0,1);
}
