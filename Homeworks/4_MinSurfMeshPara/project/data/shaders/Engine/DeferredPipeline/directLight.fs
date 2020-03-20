#version 330 core

#include "../BRDF/Frostbite.h"
#include "../../Math/intersect.h"
#include "../../Math/Segment.h"

#include "../Light/AreaLight.h"
#include "../Light/LTC.h"z

// ----------------- 输入输出

out vec3 FragColor;

in vec2 TexCoords;

// ----------------- 常量

#define MAX_POINT_LIGHTS 8
#define MAX_DIRECTIONAL_LIGHTS 8
#define MAX_SPOT_LIGHTS 8
#define MAX_SPHERE_LIGHTS 8
#define MAX_DISK_LIGHTS 8
#define MAX_AREA_LIGHTS 8
#define MAX_CAPSULE_LIGHTS 8

// ----------------- Uniform

// 96
struct DirectionalLight{
	vec3 L;         // 12   0
	vec3 dir;       // 12   16
	mat4 ProjView;  // 64   32
};

// 160
layout (std140) uniform Camera {
	mat4 view;			// 64	0	64
	mat4 projection;	// 64	64	64
	vec3 viewPos;		// 12	128	144
	float nearPlane;	// 4	144	148
	float farPlane;		// 4	148	152
	float fov;			// 4	152	156
	float ar;			// 4	156	160
};

layout (std140) uniform AreaLights {
	int numAreaLight;// 16
	AreaLight areaLights[MAX_AREA_LIGHTS];// 64 * MAX_AREA_LIGHTS = 64 * 8 = 512
};

uniform sampler2D GBuffer0;
uniform sampler2D GBuffer1;
uniform sampler2D GBuffer2;
uniform sampler2D GBuffer3;

uniform float lightNear;
uniform float lightFar;

// ----------------- 函数声明

vec3 BRDFd(int ID, vec3 norm, vec3 wo, vec3 wi, vec3 albedo, float metallic, float roughness);
vec3 BRDFs(int ID, vec3 norm, vec3 wo, vec3 wi, vec3 albedo, float metallic, float roughness);
void BRDF(out vec3 diffuse, out vec3 spec, int ID, vec3 norm, vec3 wo, vec3 wi, vec3 albedo, float metallic, float roughness);

float Fwin(float d, float radius);

float SpotLightDirFalloff(vec3 wi, int id);
float LinearizeDepth(float depth, float near, float far) {
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));
}
float PerpDepth(float linearDepth, float near, float far) {
	float z = (near + far - 2.0 * near * far / linearDepth) / (far-near);
	return (z + 1.0) / 2.0;
}

// ----------------- 主函数

void main() {
	// 获取属性值
	vec4 data0 = texture(GBuffer0, TexCoords);
	vec4 data1 = texture(GBuffer1, TexCoords);
	vec4 data2 = texture(GBuffer2, TexCoords);
	vec4 data3 = texture(GBuffer3, TexCoords);
	
	int ID = int(data0.w);
	vec3 pos = data0.xyz;
	vec3 norm = normalize(data1.xyz);
	vec3 albedo = data2.xyz;
	float roughness = data1.w;
	roughness = mix(0.02, roughness, roughness);
	float metallic = data2.w;
	float ao = data3.w;
	
	vec3 wo = normalize(viewPos - pos);
	vec3 R = reflect(-wo, norm);
	
	if(ID == 3) { // emission
		FragColor = dot(norm, wo) > 0 ? albedo : vec3(0);
		return;
	}
	
	// for ID 0, 1, 2
	
	// 采样光源
	vec3 result = vec3(0);
	
	// area light
	for(int i=0; i<numAreaLight; i++) {
#if 1
		vec3 F0 = MetalWorkflowF0(albedo, metallic);
		vec3 spec = LTC_Spec(norm, wo, pos, F0, roughness, areaLights[i]);
		vec3 FrR = SchlickFrR(wo, norm, F0, roughness);
		vec3 diffuse = (1 - metallic) * albedo * FrR * INV_PI * LTC_Diffuse(norm, wo, pos, roughness, areaLights[i]);
		result += diffuse + spec;
#else
		vec3 illuminanceD = AreaLight_Illuminance(areaLights[i], pos, norm);
		
		vec3 MRP = AreaLight_MRP(areaLights[i], pos, R);
		vec3 PtoL = MRP - pos;
		float dist2 = dot(PtoL, PtoL);
		float dist = sqrt(dist2);
		vec3 wi = PtoL / dist;
		
		vec3 fd, fs;
		BRDF(fd, fs, ID, norm, wo, wi, albedo, metallic, roughness);
		
		float attenuation = step(0, dot(-wi, areaLights[i].dir)) / max(0.0001, dist2);
		vec3 illuminanceS = areaLights[i].luminance * attenuation;
		float cosTheta = max(0, dot(wi, norm));
		result += illuminanceD * fd + cosTheta * illuminanceS * fs;
#endif
	}
	
    FragColor = result;
}

// ----------------- 函数定义

void BRDF(out vec3 fd, out vec3 fs, int ID, vec3 norm, vec3 wo, vec3 wi, vec3 albedo, float metallic, float roughness) {
	if(ID == 2)
		BRDF_Frostbite(fd, fs, norm, wo, wi, albedo, metallic, roughness);
	else{
		fd = vec3(0);
		fs = vec3(0);
	}
}

vec3 BRDFd(int ID, vec3 norm, vec3 wo, vec3 wi, vec3 albedo, float metallic, float roughness) {
	if (ID == 2)
		return BRDFd_Frostbite(norm, wo, wi, albedo, metallic, roughness);
	else
		return vec3(0);
}

vec3 BRDFs(int ID, vec3 norm, vec3 wo, vec3 wi, vec3 albedo, float metallic, float roughness) {
	if (ID == 2)
		return BRDFs_Frostbite(norm, wo, wi, albedo, metallic, roughness);
	else
		return vec3(0);
}
