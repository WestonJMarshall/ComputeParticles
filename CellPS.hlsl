#include "BasicShaderIncludes.hlsli"

cbuffer ExternalData : register(b0)
{
	Light directionalLights[MAX_LIGHT_COUNT];
	Light pointLights[MAX_LIGHT_COUNT];

	float4 colorTint;
	float3 cameraPosition;
}

Texture2D albedoTexture: register(t0);
Texture2D normalTexture: register(t1);
Texture2D roughnessTexture: register(t2);
Texture2D metalnessTexture: register(t3);
Texture2D cellRampTexture: register(t4);
SamplerState textureSampler: register(s0);
SamplerState clampSampler: register(s1);

struct PSOutput
{
	float4 color	: SV_TARGET0;
	float4 normals	: SV_TARGET1;
	float depth     : SV_TARGET2;
};

PSOutput main(NormalVTP input) : SV_TARGET
{
	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);

	//Generate lighting value
	float4 cellLighting = CellLighting(input, directionalLights, pointLights, colorTint, cameraPosition, albedoTexture, normalTexture, roughnessTexture, metalnessTexture, textureSampler);

	PSOutput output;

	//Create a smoother cell shade by pulling two lighting colors and combining them
	float rampValue = 1 - ((cellLighting.r + cellLighting.g + cellLighting.b) / 3.0f);
	rampValue *= 0.3f;
	output.color = cellRampTexture.Sample(clampSampler, float2(rampValue, 0));
	rampValue *= 1.5f;
	output.color *= cellRampTexture.Sample(clampSampler, float2(rampValue, 0));

	output.color *= float4(pow(albedoTexture.Sample(textureSampler, input.uv).rgb, 2.2f), 1);
	output.normals = float4(input.normal, 0);
	output.depth = input.position.b;

	return output;
}