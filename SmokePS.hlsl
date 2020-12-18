#include "BasicShaderIncludes.hlsli"

struct SmokeVStoPS
{
	float4 position : SV_POSITION;
	float4 age	    : AGE;
	float3 center   : CENTER;
	float3 normal   : NORMAL;
	float2 uv		: TEXCOORD;
};

struct PSOutput
{
	float4 color	: SV_TARGET0;
	float4 normals	: SV_TARGET1;
	float depth : SV_TARGET2;
};

PSOutput main(SmokeVStoPS input) : SV_TARGET
{
	// Convert uv to -1 to 1
	input.uv = input.uv * 2 - 1;

	// Distance from center
	float fade = saturate(distance(float2(0,0), input.uv));

	//Creates circles
	clip(step((1 - fade), 0.01) == 0 ? 1 : -1);

	input.normal = normalize(input.normal);

	PSOutput output;

	output.color = clamp(input.age + 0.65f, 0.65f, 1);
	output.normals = float4(input.normal, 0);
	output.depth = input.position.b;

	return output;
}