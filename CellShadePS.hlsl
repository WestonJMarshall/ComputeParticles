
cbuffer ExternalData : register(b0)
{
	float pixelWidth;
	float pixelHeight;
	float depthAdjust;
	float normalAdjust;
}

struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float2 uv           : TEXCOORD0;
};

Texture2D pixels			: register(t0);
Texture2D normals			: register(t1);
Texture2D depth				: register(t2);
SamplerState samplerOptions	: register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
	float2 offsets[8] =
	{
		float2(-pixelWidth, 0),
		float2(+pixelWidth, 0),
		float2(0, -pixelHeight),
		float2(0, +pixelHeight),
		float2(-pixelWidth, -pixelWidth),
		float2(+pixelWidth, +pixelWidth),
		float2(+pixelWidth, -pixelHeight),
		float2(-pixelHeight, +pixelHeight),
	};

	float depths[8] = {0,0,0,0,0,0,0,0};
	for (int i = 1; i < 8; i++)
	{
		depths[i] = depth.Sample(samplerOptions, input.uv + offsets[i]).r;
	}
	depths[0] = depth.Sample(samplerOptions, input.uv).r;


	// d = difference in depth of surrounding pixels
	float d = 0;
	for (int i = 1; i < 8; i++)
	{
		d += abs(depths[0] - depths[i]);
	}

	float dt = pow(saturate(d), depthAdjust);

	// Sample the normals of this pixel and the surrounding pixels
	float3 np = normals.Sample(samplerOptions, input.uv).rgb;
	float3 nl = normals.Sample(samplerOptions, input.uv + offsets[0]).rgb;
	float3 nr = normals.Sample(samplerOptions, input.uv + offsets[1]).rgb;
	float3 nu = normals.Sample(samplerOptions, input.uv + offsets[2]).rgb;
	float3 nd = normals.Sample(samplerOptions, input.uv + offsets[3]).rgb;

	// n = difference in normal of surrounding pixels
	float3 n =
		abs(np - nl) +
		abs(np - nr) +
		abs(np - nu) +
		abs(np - nd);

	// Total the components
	float nt = pow(saturate(n.x + n.y + n.z), normalAdjust);

	//Find outline color, very close normals will cause outline to be ignored
	float outline = max(dt, nt);
	if (nt < 0.001f) { outline = 0.0f; }
	float3 color = pixels.Sample(samplerOptions, input.uv).rgb;
	float3 finalColor = lerp(color, float3(0, 0, 0), outline);

	return float4(finalColor, 1);
}