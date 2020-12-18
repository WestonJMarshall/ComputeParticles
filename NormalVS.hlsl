#include "BasicShaderIncludes.hlsli"

cbuffer ExternalData : register(b0)
{
	float4 colorTint;
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
}

NormalVTP main(VertexShaderInput input)
{
	// Set up output struct
	NormalVTP output;

	matrix wvp = mul(projectionMatrix, mul(viewMatrix, worldMatrix));
	output.position = mul(wvp, float4(input.position, 1.0f));
	output.worldPos = mul(worldMatrix, float4(input.position, 1.0f));

	output.normal = mul((float3x3)worldMatrix, input.normal);
	output.normal = normalize(output.normal);
	output.tangent = mul((float3x3)worldMatrix, input.tangent);
	output.tangent = normalize(output.tangent);
	output.uv = input.uv;

	// Whatever we return will make its way through the pipeline to the
	// next programmable stage we're using (the pixel shader for now)
	return output;
}