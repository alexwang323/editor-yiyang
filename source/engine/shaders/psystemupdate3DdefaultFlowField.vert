#version 400
/*uniform containing time between update steps*/
uniform float u_DelTime;
/*using image noise as means of assigning random start directions*/
uniform sampler2D u_RGNoise;
uniform sampler2D u_RGNoise1;
uniform vec3 u_Force;
uniform vec3 u_StartPos;
uniform float u_MinTheta;
uniform float u_MaxTheta;
uniform float u_MinSpeed;
uniform float u_MaxSpeed;
uniform float u_MinLife;
uniform float u_MaxLife;
uniform vec3 u_Scale;

layout (location = 0) in vec3 i_Pos;
layout (location = 1) in float i_Age;
layout (location = 2) in float i_Life;
layout (location = 3) in vec3 i_Vel;

out vec3 v_Pos;
out float v_Age;
out float v_Life;
out vec3 v_Vel;




// Generate a random unsigned int from two unsigned int values, using 16 pairs
// of rounds of the Tiny Encryption Algorithm. See Zafar, Olano, and Curtis,
// "GPU Random Numbers via the Tiny Encryption Algorithm"
uint tea(uint val0, uint val1)
{
  uint v0 = val0;
  uint v1 = val1;
  uint s0 = 0;

  for(uint n = 0; n < 16; n++)
  {
    s0 += 0x9e3779b9;
    v0 += ((v1 << 4) + 0xa341316c) ^ (v1 + s0) ^ ((v1 >> 5) + 0xc8013ea4);
    v1 += ((v0 << 4) + 0xad90777d) ^ (v0 + s0) ^ ((v0 >> 5) + 0x7e95761e);
  }

  return v0;
}

// Generate a random unsigned int in [0, 2^24) given the previous RNG state
// using the Numerical Recipes linear congruential generator
uint lcg(inout uint prev)
{
    uint LCG_A = 1664525u;
    uint LCG_C = 1013904223u;
    prev       = (LCG_A * prev + LCG_C);
    return prev & 0x00FFFFFF;
}

// Generate a random float in [0, 1) given the previous RNG state
float rnd(inout uint prev)
{
    return (float(lcg(prev)) / float(0x01000000));
}


void main()
{
    if (i_Age>=i_Life)
    {
         uint indrnd =uint(gl_VertexID);
         float rand = rnd(indrnd);
         float theta = u_MinTheta + rnd(indrnd) * (u_MaxTheta - u_MinTheta);
         float oang = u_MinTheta + rnd(indrnd) * (u_MaxTheta - u_MinTheta);
         float y = cos(theta);
         float x = sin(oang);
         float z = cos(oang);
         v_Pos = vec3(x*2.0f,0.0f,z*2.0f)+u_StartPos;
         v_Age = 0.0;
         v_Life = (u_MinLife + rnd(indrnd) * (u_MaxLife - u_MinLife));
         v_Vel= vec3(x,y,z) * (u_MinSpeed + rnd(indrnd) * (u_MaxSpeed - u_MinSpeed));
    }
    else
    {
        v_Pos = i_Pos + i_Vel*u_DelTime;
        v_Age = i_Age + u_DelTime;
        v_Life = i_Life;
        //offset positions
        vec3 tPos = v_Pos-u_StartPos + 0.5*u_Scale;
        tPos.x=tPos.x/u_Scale.x;
        tPos.z=tPos.z/u_Scale.z;
        tPos.y=tPos.y/u_Scale.y;
        vec2 tmp = texture(u_RGNoise,tPos.xy).rg;
        vec2 tmp1 = texture(u_RGNoise1,tPos.xz).rg;
        //so its -.5 to .5
        v_Vel = vec3((tmp1.x-0.5)*5.0f*u_Scale.x,(tmp.y - 0.5)*5.0f*u_Scale.y,(tmp1.y-0.5)*5.0f*u_Scale.z);

        //v_Vel = 2.0*u_MaxSpeed*texture(u_RGNoise,(1.0f/u_MaxLife/u_MaxSpeed)*i_Pos,0).xyz - vec3(u_MaxSpeed,u_MaxSpeed,u_MaxSpeed)
    }
}