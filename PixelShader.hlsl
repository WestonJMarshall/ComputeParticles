#include "BasicShaderIncludes.hlsli"

cbuffer ExternalData : register(b0)
{
	Light directionalLights[MAX_LIGHT_COUNT];
	Light pointLights[MAX_LIGHT_COUNT];

	float4 colorTint;
	float3 cameraPosition;
	float specularIntensity;
}

Texture2D albedoTexture: register(t0);
Texture2D normalTexture: register(t1);
Texture2D roughnessTexture: register(t2);
Texture2D metalnessTexture: register(t3);
SamplerState textureSampler : register(s0);


float4 main(NormalVTP input) : SV_TARGET
{
	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);

	return StandardPBRLighting(input, directionalLights, pointLights, colorTint, cameraPosition, albedoTexture, roughnessTexture, metalnessTexture, textureSampler);
}
