#pragma once

#include <d3d11.h>

#include <wrl/client.h>

#include "Vertex.h"


class Mesh
{
public:
	Mesh(Vertex* vertArray, int numVerts, unsigned int* indexArray, int numIndices, ID3D11Device* device);
	Mesh(const char* objFile, ID3D11Device* device);
	~Mesh(void);

	ID3D11Buffer* GetVertexBuffer() { return vb; }
	ID3D11Buffer* GetIndexBuffer() { return ib; }
	int GetIndexCount() { return numIndices; }
	int GetVertexCount() { return pVertCounter; }
	uint32_t indexBuffSize();
	void SetBuffersAndDraw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
	unsigned int* indicesArray;
	Vertex* vertexArray;
	int pVertCounter;
private:
	ID3D11Buffer* vb = 0;
	ID3D11Buffer* ib = 0;
	int numIndices = 0;

	void CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices);
	void CreateBuffers(Vertex* vertArray, int numVerts, unsigned int* indexArray, int numIndices, ID3D11Device* device);
};

