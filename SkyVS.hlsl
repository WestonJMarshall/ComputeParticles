struct VertexShaderInput
{
	float3 position		: POSITION;
	float3 normal       : NORMAL;
	float3 tangent      : TANGENT;
	float2 uv           : TEXCOORD;
};

struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 sampleDir    : DIRECTION;
};

cbuffer ExternalData : register(b0)
{
	matrix viewMatrix;
	matrix projectionMatrix;
}

VertexToPixel main(VertexShaderInput input)
{
	VertexToPixel output;

	matrix viewNoTranslation = viewMatrix;
	viewNoTranslation._14 = 0;
	viewNoTranslation._24 = 0;
	viewNoTranslation._34 = 0;

	matrix vp = mul(projectionMatrix, viewNoTranslation);
	output.position = mul(vp, float4(input.position, 1.0f));
	output.position.z = output.position.w;

	output.sampleDir = input.position;

	return output;
}