#pragma once
#include "Mesh.h"
#include "Transform.h"
#include "Material.h"

using namespace DirectX;

class Entity
{
public:
	Entity(Mesh* _mesh, Material* _material);
	~Entity();

	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, DirectX::XMFLOAT4X4* viewMatrix, DirectX::XMFLOAT4X4* projectionMatrix, Microsoft::WRL::ComPtr<ID3D11SamplerState> clampSampler, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cellRamp);

	Mesh* GetMesh();
	Transform* GetTransform();
	Material* GetMaterial();

private:
	Transform transform;
	Mesh* mesh;
	Material* material;
};

