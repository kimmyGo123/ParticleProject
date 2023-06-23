#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include "SimpleShader.h"
#include "Camera.h"
#include "Mesh.h"

struct Particle
{
	DirectX::XMFLOAT4 Color;
	float Age;
	DirectX::XMFLOAT3 Position;
	float Size;
	DirectX::XMFLOAT3 Velocity;
	float Alive;
	DirectX::XMFLOAT3 padding;
};

struct ParticleSort
{
	unsigned int index;
	float distanceSq;
};

class Emitter
{
public:
	Emitter(
		unsigned int maxParticles,
		float emissionRate, // Particles per second
		float lifetime,
		ID3D11Device* device,
		ID3D11DeviceContext* context,
		SimpleComputeShader* deadListInitCS,
		SimpleComputeShader* emitCS,
		SimpleComputeShader* updateCS,
		SimpleComputeShader* copyDrawCountCS,
		SimpleVertexShader* particleVS,
		SimpleGeometryShader* particleGS,
		SimplePixelShader* particlePS,
		SimpleVertexShader* particleVSnoGS,
		Mesh* spawnObj);
	~Emitter();

	bool GetUseGeometryShader() { return useGeometryShader; }
	void UseGeometryShader(bool use) { useGeometryShader = use; }
	void Update(float dt, float tt);
	void Draw(Camera* camera, bool additive);

private:

	// Emitter settings
	unsigned int maxParticles;
	float lifetime;
	float emissionRate;
	float timeBetweenEmit;
	float emitTimeCounter;

	// Drawing
	bool useGeometryShader;
	ID3D11Buffer* indexBuffer;
	ID3D11BlendState* additiveBlend;
	ID3D11DepthStencilState* depthWriteOff;

	// DirectX stuff
	ID3D11DeviceContext* context;
	SimpleComputeShader* emitCS;
	SimpleComputeShader* updateCS;
	SimpleComputeShader* copyDrawCountCS;
	SimpleVertexShader* particleVS;
	SimpleVertexShader* particleVSnoGS;
	SimpleGeometryShader* particleGS;
	SimplePixelShader* particlePS;

	// Indirect draw buffer
	ID3D11Buffer* drawArgsBuffer;
	
	// Particle views
	ID3D11UnorderedAccessView* particlePoolUAV;
	ID3D11ShaderResourceView* particlePoolSRV;
	ID3D11UnorderedAccessView* particleDeadUAV;
	ID3D11UnorderedAccessView* particleDrawUAV;
	ID3D11ShaderResourceView* particleDrawSRV;
	ID3D11UnorderedAccessView* drawArgsUAV;
	Mesh* particleOBJ;

	ID3D11Buffer* mIndexBuffer;
	ID3D11UnorderedAccessView* indexUAV;
	ID3D11ShaderResourceView* meshIndexSRV;

	ID3D11Buffer* vBuffer;
//	ID3D11UnorderedAccessView* vBufferUAV;
	ID3D11ShaderResourceView* vBufferSRV;

};

