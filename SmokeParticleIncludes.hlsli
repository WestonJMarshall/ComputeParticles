
#define THREAD_MAX 32

#define EMITTER_SIZE_X 6
#define EMITTER_SIZE_Y 12
#define EMITTER_SIZE_Z 6

#define PARTICLE_CYCLE_TIME 6
#define PARTICLE_DEFINITIONS_LENGTH 512

struct Particle
{
	float3 Position;
	float Age;
	float Size;
	float3 empty;
};
