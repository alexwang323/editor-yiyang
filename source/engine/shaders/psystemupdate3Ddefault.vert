#version 400
/*uniform containing time between update steps*/
uniform float u_DelTime;
/*using image noise as means of assigning random start directions*/
uniform sampler2D u_RGNoise;
uniform vec3 u_Force;
uniform vec3 u_StartPos;
uniform float u_MinTheta;
uniform float u_MaxTheta;
uniform float u_MinSpeed;
uniform float u_MaxSpeed;
uniform float u_MinLife;
uniform float u_MaxLife;

layout (location = 0) in vec3 i_Pos;
layout (location = 1) in float i_Age;
layout (location = 2) in float i_Life;
layout (location = 3) in vec3 i_Vel;

out vec3 v_Pos;
out float v_Age;
out float v_Life;
out vec3 v_Vel;

void main()
{
    if (i_Age>=i_Life)
    {
         ivec2 noise_coord = ivec2(gl_VertexID % 256, gl_VertexID / 256);
         vec3 rand = texelFetch(u_RGNoise, noise_coord, 0).rgb;
         float theta = u_MinTheta + rand.r * (u_MaxTheta - u_MinTheta);
         float oang = u_MinTheta + rand.g * (u_MaxTheta - u_MinTheta);
         float y = cos(theta);
         float x = sin(oang)*sin(theta);
         float z = cos(oang)*sin(theta);
         v_Pos = u_StartPos;
         v_Age = 0.0;
         v_Life = (u_MinLife + rand.g * (u_MaxLife - u_MinLife));
         v_Vel= vec3(x,y,z) * (u_MinSpeed + rand.b * (u_MaxSpeed - u_MinSpeed))+ u_Force * u_DelTime;
    }
    else
    {
        v_Pos= i_Pos + i_Vel*u_DelTime;
        v_Age = i_Age + u_DelTime;
        v_Life = i_Life;

        v_Vel = i_Vel -i_Vel*u_DelTime*u_DelTime + u_Force * u_DelTime;
    }
}