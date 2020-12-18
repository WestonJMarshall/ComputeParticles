#include "SmokeParticleIncludes.hlsli"

cbuffer ExternalData : register(b0)
{
	int VertsPerParticle;
	int TotalParticles;
}

RWBuffer<uint> DrawArgs : register(u0);
RWStructuredBuffer<Particle> Particles : register(u1);

[numthreads(1, 1, 1)]
void main( uint3 threadID : SV_DispatchThreadID )
{
	DrawArgs[0] = TotalParticles * VertsPerParticle; // Total vertices
	DrawArgs[1] = 1; 
	DrawArgs[2] = 0; 
	DrawArgs[3] = 0; 
	DrawArgs[5] = 0; 
}