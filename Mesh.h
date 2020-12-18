#pragma once

#include "Vertex.h"
#include <wrl/client.h> 
#include <d3d11.h>
#include <vector>

class Mesh
{
public:
	Mesh(Vertex* vertexArray, unsigned int numVertices, unsigned int* indexArray, unsigned int numIndices, Microsoft::WRL::ComPtr<ID3D11Device> device);
	Mesh(const char* filePath, Microsoft::WRL::ComPtr<ID3D11Device> device);
	~Mesh();

	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	unsigned int GetIndexCount();

private:	
	void CreateBuffers(int vertNum, Vertex* verts, int indexNum, unsigned int* indices, Microsoft::WRL::ComPtr<ID3D11Device> device);

	void CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices);

	unsigned int indexCount = 0;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
};

