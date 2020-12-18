struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 sampleDir    : DIRECTION;
};

TextureCube textureCube	    : register(t0);
SamplerState textureSampler : register(s0);

struct PSOutput
{
	float4 color	: SV_TARGET0;
	float4 normals	: SV_TARGET1;
	float depth : SV_TARGET2;
};

PSOutput main(VertexToPixel input) : SV_TARGET
{

	PSOutput output;

	output.color = float4(textureCube.Sample(textureSampler, input.sampleDir).rgb, 1);
	output.normals = float4(-input.sampleDir, 0);
	output.depth = 1;

	return output;

	//return float4(textureCube.Sample(textureSampler, input.sampleDir).rgb, 1);
}