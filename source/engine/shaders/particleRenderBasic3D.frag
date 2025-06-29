#version 400
precision mediump float;

out vec4 o_FragColor;
#define INVERSE_SQRT_TWO_PI 0.39894
#define INVERSE_STANDARD_DEVIATION 2

void main() {
  
  vec2 circCoord = 2.0 * gl_PointCoord - 1.0;
  float tmp = INVERSE_SQRT_TWO_PI*INVERSE_STANDARD_DEVIATION * exp(-0.5*dot(circCoord, circCoord)*INVERSE_STANDARD_DEVIATION*INVERSE_STANDARD_DEVIATION);
  o_FragColor = vec4(1.0,1.0,1.0,tmp);
  if (dot(circCoord, circCoord) > 1.0)
  {
    discard;
  }
}