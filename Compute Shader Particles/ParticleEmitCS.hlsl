
#include "ParticleIncludes.hlsli"
#include "SimplexNoise.hlsli"

cbuffer ExternalData : register(b0)
{
	float TotalTime;
	int EmitCount;
	int MaxParticles;
	int GridSize;
	//int indexCount;
}

// Order should match UpdateCS (RW binding issues)
RWStructuredBuffer<Particle> ParticlePool	: register(u0);
ConsumeStructuredBuffer<uint> DeadList		: register(u1);
//StructuredBuffer<uint> indexBuffer : register(t0);
StructuredBuffer<Vertex> vertexBuffer : register(t0);

//emit from mesh
//StructuredBuffer<uint> indices : register(t0);
// https://www.shadertoy.com/view/llGSzw
float hash1(uint n)
{
	// integer hash copied from Hugo Elias
	n = (n << 13U) ^ n;
	n = n * (n * n * 15731U + 789221U) + 1376312589U;
	return float(n & 0x7fffffffU) / float(0x7fffffff);
}
[numthreads(32, 1, 1)]
void main( uint3 id : SV_DispatchThreadID )
{

	//uint index = indexBuffer[id.x];
	// Outside range?
	if(id.x >= (uint)EmitCount) return;
	//float3 index = indexBuffer.Load(id.x);
	 // Grab a single index from the dead list
	float3 vertex = vertexBuffer.Load(id.x).Position;

	uint emitIndex = DeadList.Consume();

	// This shader very specifically emits particles in a grid shape
	// based on the index and a particular grid size, which is 
	// passed in from C++
	float3 gridPos;
	/*uint gridIndex = emitIndex;
	gridPos.x = gridIndex % (GridSize + 1);
	gridIndex /= (GridSize + 1) ;
	gridPos.y = gridIndex % (GridSize + 1);
	gridIndex /= (GridSize + 1);
	gridPos.z = gridIndex;
	*/
	//uint gridIndex = index;
	//index.x = emitIndex % (GridSize + index.x * 10);// 1);
	//gridIndex /= (GridSize + 1);
	//index.y = gridIndex % (GridSize + index.y * 10);//1);
	//	gridIndex /= (GridSize + 1);
	//index.z = index.z * 10;
	
	// Update it in the particle pool
	Particle emitParticle = ParticlePool.Load(emitIndex);

	// Color and position depend on the grid position and size
	emitParticle.Color = float4(vertex.x, vertex.y, 0.2, 1);
	emitParticle.Age = 0.0f;
	//emitParticle.Position = index;
	emitParticle.Position = vertex;// / -float3(GridSize / 20.0f, GridSize / 20.0f, -GridSize / 10.0f);
	emitParticle.Size =  0.05f;
	emitParticle.Velocity = float3(0, 0, 0);
	emitParticle.Alive = 1.0f;



	//ParticleSwapPool[emitIndex] = emitParticle;
	// Put it back 
	ParticlePool[emitIndex] = emitParticle;
}