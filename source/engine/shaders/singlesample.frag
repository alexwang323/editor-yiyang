#version 400

out vec4 FragColor;

uniform sampler2DMS g_position_ms;
uniform sampler2DMS g_normal_ms;
uniform sampler2DMS g_albedo_ms;
uniform sampler2DMS g_specular_ms;

uniform sampler2D s_shadowmap;
uniform mat4 u_shadowmatrix;

in vec2 i_texcoord;

uniform vec3 u_eyepos;
uniform vec3 u_lightpos;
uniform vec3 u_lightval;
uniform vec3 u_lightamb;

//--------------------------------------------------
//                    GGX BRDF
//--------------------------------------------------
// maybe move this to a different header?

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

vec3 F_Schlick(float u, vec3 f0) {
    return f0 + (vec3(1.0,1.0,1.0) - f0) * pow(1.0 - u, 5.0);
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
    
    float bias = max(0.0025 * (1.0 - dot(N, L)), 0.0025);

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

//--------------------------------------------------

void main() {

	ivec2 uv_pos = ivec2(i_texcoord * vec2(textureSize(g_position_ms)));
	ivec2 uv_nrm = ivec2(i_texcoord * vec2(textureSize(g_normal_ms)));
	ivec2 uv_col = ivec2(i_texcoord * vec2(textureSize(g_albedo_ms)));
	ivec2 uv_spc = ivec2(i_texcoord * vec2(textureSize(g_specular_ms)));

	vec3 FragPos = texelFetch(g_position_ms, uv_pos, 0).rgb;
	vec3 N = texelFetch(g_normal_ms, uv_nrm, 0).rgb;
	vec3 Kd = texelFetch(g_albedo_ms, uv_col, 0).rgb;
	vec3 Ks = texelFetch(g_specular_ms, uv_spc, 0).rgb;
    float alpha = texelFetch(g_specular_ms, uv_spc, 0).a;
    alpha = alpha * alpha;

	vec3 V = normalize(u_eyepos - FragPos);

	// move to loop for point lights
	vec3 L = normalize(u_lightpos - FragPos);
	vec3 H = normalize(V + L);

    Kd.x = pow(Kd.x, 2.2);
    Kd.y = pow(Kd.y, 2.2);
    Kd.z = pow(Kd.z, 2.2);

    float NoH = clamp(dot(N, H), 0.0, 1.0);
    float LoH = clamp(dot(L, H), 0.0, 1.0);
    float NoV = abs(dot(N, V)) + 0.00000001;
    float NoL = max(dot(N, L), 0.0);

    vec3 F = F_Schlick(LoH, Ks);
    float D = D_GGX(NoH, alpha);
    float G = V_Smith_GGX(NoV, NoL, alpha);
    vec3 BRDF = (Kd / PI) + ((D * G) * F);

    vec3 ambient = u_lightamb * Kd;
    
    vec4 ShadowCoord = u_shadowmatrix * vec4(FragPos, 1.0);
    float shadow = CalculateShadows(ShadowCoord, N, L);

    // non-shadowed version
    //vec3 color = (u_lightamb * Kd) + (BRDF * NoL * u_lightval);
    vec3 color = ambient + ((1.0 - shadow) * (BRDF * NoL * u_lightval));

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));
    FragColor = vec4(color, 1.0);

    //gl_FragDepth = FragPos.z;

    //FragColor.xy = i_texcoord;
}