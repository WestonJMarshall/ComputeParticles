#include "SmokeParticleIncludes.hlsli"

cbuffer ExternalData : register(b0)
{
	float TimeStep;
    float TotalParticles;
    float2 padding;
}

RWStructuredBuffer<Particle> Particles : register(u0);

Texture2D smokeDefinitions             : register(t0);
SamplerState smokeDefinitionsSampler   : register(s0);

/*STEPS:----------------------------------------
----LOAD PARTICLE FROM STRUCTURED BUFFER
----UPDATE AGE VALUE
----PULL POSITION VALUE FROM DEFINITIONS TEXTURE
----SET PARTICLE SIZE
----REPLACE UPDATED PARTICLE IN STRUCTURED BUFFER 
------------------------------------------------*/
[numthreads(THREAD_MAX, 1, 1)]
void main( uint3 threadID : SV_DispatchThreadID )
{
	 Particle particle = Particles.Load(threadID.x);

     //CURRENTLY every PARTICLE_CYCLE_TIME seconds particle goes from 0 -> 1 age value
     particle.Age += TimeStep / PARTICLE_CYCLE_TIME;
     particle.Age = particle.Age % 1.0f;
    
    //Sample definitions texture at age to retrieve particle position
    float2 lookUpValue = float2(threadID.x / TotalParticles, particle.Age);
    particle.Position = smokeDefinitions.SampleLevel(smokeDefinitionsSampler, lookUpValue, 0).rgb;
    particle.Position.r *= EMITTER_SIZE_X;
    particle.Position.g *= EMITTER_SIZE_Y;
    particle.Position.b *= EMITTER_SIZE_Z;

    particle.Size = sin(particle.Age) * 3.0f;

	Particles[threadID.x] = particle;
}