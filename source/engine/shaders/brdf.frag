#version 400

out vec4 FragColor;

uniform vec3 u_diffuse, u_specular;
uniform float u_roughness;

uniform vec3 u_lightval;
uniform vec3 u_lightamb;

uniform sampler2D s_albedo;
uniform bool u_usingalbedo;

uniform sampler2D s_normal;
uniform bool u_usingnormal;

uniform sampler2D s_specular;
uniform bool u_usingspecular;

uniform sampler2D s_roughness;
uniform bool u_usingroughness;

uniform bool u_precalctangents;

uniform sampler2D s_shadowmap;

in vec3 i_worldpos, i_normalvector, i_lightvector, i_eyevector, i_tanvector;
in vec2 i_texcoord;

in vec4 i_shadowcoord;

//--------------------------------------------------
//                    GGX BRDF
//--------------------------------------------------
#define PI 3.1415926538

float D_GGX(float NoH, float a) {
    float a2 = a * a;
    float f = (NoH * a2 - NoH) * NoH + 1.0;
    return a2 / (PI * f * f);
}

float V_Smith_GGX(float NoV, float NoL, float a) {
    float a2 = a * a;
    float ggx_l = NoV * sqrt((-NoL * a2 * NoL) * NoL + a2);
    float ggx_v = NoL * sqrt((-NoV * a2 * NoV) * NoV + a2);
    return 0.5 / (ggx_v + ggx_l);
}

// funny fresnel optimization (saves 2 multiplications) found by
// https://github.com/GarrettGunnell/Disney-PBR/
vec3 F_Schlick(float u, vec3 f0) {
    float x = (1.0 - u);
    float x2 = x * x;
    return f0 + (vec3(1.0,1.0,1.0) - f0) * (x2 * x2 * x);
}

//--------------------------------------------------
//                SHADOW CALCULATION
//--------------------------------------------------

float CalculateShadows(vec4 shadowCoord, vec3 N, vec3 L) {
    vec3 projCoords = shadowCoord.xyz / shadowCoord.w;

    projCoords = projCoords * 0.5 + 0.5;

    // not used in pcf
    float closestDepth = texture(s_shadowmap, projCoords.xy).r;

    float currentDepth = projCoords.z;

    float bias = max(0.005 * (1.0 - dot(N, L)), 0.005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(s_shadowmap, 0);

    // hard coding a 3x3 pcf
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(s_shadowmap, projCoords.xy + vec2(x,y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    if (projCoords.z > 1.0) {
        shadow = 0.0;
    }

    return shadow;
}

void main() {
	vec3 N = normalize(i_normalvector);
    vec3 L = normalize(i_lightvector);
    vec3 V = normalize(i_eyevector);

    vec3 T;
    if (u_precalctangents) {
       T = normalize(i_tanvector);
    } else {
        vec3 q1 = dFdx(i_worldpos);
        vec3 q2 = dFdy(i_worldpos);
        vec2 st1 = dFdx(i_texcoord);
        vec2 st2 = dFdy(i_texcoord);
        T = normalize(q1 * st2.t - q2 * st1.t);
    }
    vec3 B = normalize(cross(T,N));
    
    vec3 Kd = u_diffuse;
    vec3 Ks = u_specular;
    float alpha = u_roughness;

	vec3 H = normalize(V+L);
    
    if (u_usingalbedo) {
        Kd = texture(s_albedo, i_texcoord).rgb;
    }
    Kd.x = pow(Kd.x, 2.2);
    Kd.y = pow(Kd.y, 2.2);
    Kd.z = pow(Kd.z, 2.2);

    if (u_usingnormal) {
        vec3 delta = texture(s_normal, i_texcoord).xyz * 2.0 - 1.0;
        N = delta.x * T + delta.y * B + delta.z * N;
    }

    // supports colored specularity, subject to change.
    if (u_usingspecular) {
        Ks = texture(s_specular, i_texcoord).rgb;
    }

    if (u_usingroughness) {
        alpha = texture(s_roughness, i_texcoord).r;
    }
    alpha = alpha * alpha;
    
    float NoH = clamp(dot(N, H), 0.0, 1.0);
    float LoH = clamp(dot(L, H), 0.0, 1.0);
    float NoV = abs(dot(N, V)) + 1e-8;
    float NoL = max(dot(N, L), 0.0);

    vec3 F = F_Schlick(LoH, Ks);

    // PHONG BRDF (calculated in place because its easy)
    //float D = ((alpha + 2.0) / (2.0 * PI)) * pow(NoH, alpha);
    //float G = 1.0 / (LH * LH);
    //vec3 BRDF = (Kd / PI) + (((D * G) * F) / 4.0);

    // GGX BRDF
    float D = D_GGX(NoH, alpha);
    float G = V_Smith_GGX(NoV, NoL, alpha);
    vec3 BRDF = (Kd / PI) + ((D * G) * F);

    //vec3 color = (u_lightamb * Kd) + (BRDF * NoL);

    vec3 ambient = u_lightamb * Kd;

    float shadow = CalculateShadows(i_shadowcoord, N, L);

    vec3 color = ambient + ((1.0 - shadow) * (BRDF * NoL * u_lightval));
    
//--------------------------------------------------
//                  DEBUG: SHADOWS
//--------------------------------------------------
    //vec3 projCoords = i_shadowcoord.xyz / i_shadowcoord.w;
    //projCoords = projCoords * 0.5 + 0.5;
    //FragColor = vec4(vec3(texture(s_shadowmap, projCoords.xy).r), 1.0);

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));
    FragColor = vec4(color, 1.0);
}