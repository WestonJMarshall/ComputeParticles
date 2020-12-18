#include "SmokeParticleIncludes.hlsli"

cbuffer externalData : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
};

StructuredBuffer<Particle> Particles : register(t0);

struct SmokeVStoPS
{
	float4 position : SV_POSITION;
	float4 age	    : AGE;
	float3 center   : CENTER;
	float3 normal   : NORMAL;
	float2 uv		: TEXCOORD;
};

SmokeVStoPS main(uint threadID : SV_VertexID)
{
	SmokeVStoPS output;

	uint particleID = threadID / 4;
	uint cornerID = threadID % 4;

	Particle particle = Particles.Load(particleID);

	// Offsets for triangles
	float2 offsets[4];
	offsets[0] = float2(-1.0f, +1.0f);  // TL
	offsets[1] = float2(+1.0f, +1.0f);  // TR
	offsets[2] = float2(+1.0f, -1.0f);  // BR
	offsets[3] = float2(-1.0f, -1.0f);  // BL

	// Calc position of this corner
	float3 pos = particle.Position;
	pos += float3(view._11, view._12, view._13) * offsets[cornerID].x * particle.Size;
	pos += float3(view._21, view._22, view._23) * offsets[cornerID].y * particle.Size;

	matrix vp = mul(projection, view);
	matrix wvp = mul(world, vp);
	output.position = mul(wvp, float4(pos, 1.0f));

	output.center = mul(wvp, float4(particle.Position, 1.0f));

	float2 uvs[4];
	uvs[0] = float2(0, 0);  // TL
	uvs[1] = float2(1, 0);  // TR
	uvs[2] = float2(1, 1);  // BR
	uvs[3] = float2(0, 1);  // BL

	// Pass through
	output.uv = saturate(uvs[cornerID]);

	output.normal = float3(view._11, view._12, view._13);
	output.age = particle.Age;

	return output;
}