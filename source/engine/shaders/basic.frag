#version 400

out vec4 FragColor;

// UNIFORMS
uniform vec3 u_diffuse;
uniform vec3 u_specular;
uniform float u_shininess;

uniform sampler2D s_texture;
uniform bool u_usingtexture;

// INPUT
in vec3 i_normalvector, i_lightvector, i_eyevector;
in vec2 i_texcoord;

void main() {
    vec3 N = normalize(i_normalvector);
    vec3 L = normalize(i_lightvector);

    vec3 Kd = u_diffuse;

    if (u_usingtexture) {
        Kd = texture(s_texture, i_texcoord).xyz;
    }
    
    FragColor.xyz = vec3(0.5,0.5,0.5)*Kd + Kd*max(dot(L,N),0.0);
}