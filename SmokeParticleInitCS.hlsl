#include "SmokeParticleIncludes.hlsli"

cbuffer ExternalData : register(b0)
{
	float TotalParticles;
	float3 empty;
}

Texture2D smokeAgeLookup	           : register(t0);
SamplerState smokeAgeSampler           : register(s0);

RWStructuredBuffer<Particle> Particles : register(u0);

[numthreads(THREAD_MAX, 1, 1)]
void main( uint3 threadID : SV_DispatchThreadID )
{
	Particle particle = Particles.Load(threadID.x);

	particle.Age = smokeAgeLookup.SampleLevel(smokeAgeSampler, float2(threadID.x / TotalParticles, 0), 0).r;

	Particles[threadID.x] = particle;
}