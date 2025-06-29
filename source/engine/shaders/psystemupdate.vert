#version 400
/*uniform containing time between update steps*/
uniform float u_DelTime;
/*using image noise as means of assigning random start directions*/
uniform sampler2D u_RGNoise;
uniform vec2 u_Force;
uniform vec2 u_StartPos;
uniform float u_MinTheta;
uniform float u_MaxTheta;
uniform float u_MinSpeed;
uniform float u_MaxSpeed;
uniform float u_MinLife;
uniform float u_MaxLife;

layout (location = 0) in vec2 i_Pos;
layout (location = 1) in float i_Age;
layout (location = 2) in float i_Life;
layout (location = 3) in vec2 i_Vel;

out vec2 v_Pos;
out float v_Age;
out float v_Life;
out vec2 v_Vel;

void main()
{
    if (i_Age>=i_Life)
    {
        ivec2 noise_coord = ivec2(gl_VertexID % 256, gl_VertexID / 256);
        vec2 rand = texelFetch(u_RGNoise, noise_coord, 0).rg;
         float theta = u_MinTheta + rand.r*(u_MaxTheta - u_MinTheta);
         float y = cos(theta);
         float x = sin(theta);
         v_Pos = u_StartPos;
         v_Age = 0.0;
         v_Life =(u_MinLife + rand.g * (u_MaxLife - u_MinLife));
         v_Vel= vec2(x, y) * (u_MinSpeed + rand.g * (u_MaxSpeed - u_MinSpeed))+ u_Force * u_DelTime;
    }
    else
    {
        ivec2 noise_coord = ivec2(128*(i_Pos.x+1), 128*(i_Pos.y+1));
        vec2 rand = texelFetch(u_RGNoise, noise_coord, 0).rg;
        v_Vel = 0.5*(vec2(rand.r-.5, rand.g-.5)+i_Vel)+u_Force*u_DelTime;
        v_Pos= i_Pos + i_Vel*u_DelTime;
        v_Age = i_Age + u_DelTime;
        v_Life = i_Life;
    }
}