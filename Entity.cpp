#include "Entity.h"
#include <string>

Entity::Entity(Mesh* _mesh, Material* _material)
{
	transform = Transform();
	mesh = _mesh;
	material = _material;
}

Entity::~Entity()
{
}

void Entity::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, DirectX::XMFLOAT4X4* viewMatrix, DirectX::XMFLOAT4X4* projectionMatrix, Microsoft::WRL::ComPtr<ID3D11SamplerState> clampSampler, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cellRamp)
{
	material->GetVertexShader()->SetShader();
	material->GetPixelShader()->SetShader();

	SimpleVertexShader* vs = material->GetVertexShader(); // Simplifies next few lines
	vs->SetMatrix4x4("worldMatrix", transform.GetWorldMatrix());
	vs->SetMatrix4x4("viewMatrix", *viewMatrix);
	vs->SetMatrix4x4("projectionMatrix", *projectionMatrix);

	SimplePixelShader* ps = material->GetPixelShader(); // Simplifies next few lines
	ps->SetFloat4("colorTint", material->GetColorTint());
	ps->SetShaderResourceView("albedoTexture", material->GetTextureSRV().Get());
	ps->SetShaderResourceView("normalTexture", material->GetNormalSRV().Get());
	ps->SetShaderResourceView("metalnessTexture", material->GetMetalSRV().Get());
	ps->SetShaderResourceView("roughnessTexture", material->GetRoughSRV().Get());
	ps->SetShaderResourceView("cellRampTexture", cellRamp.Get());
	ps->SetSamplerState("textureSampler", material->GetSamplerState().Get());
	ps->SetSamplerState("clampSampler", clampSampler.Get());

	vs->CopyAllBufferData();
	ps->CopyAllBufferData();

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, mesh->GetVertexBuffer().GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(mesh->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

	context->DrawIndexed(
		mesh->GetIndexCount(),      // The number of indices to use (we could draw a subset if we wanted)
		0,							// Offset to the first index we want to use
		0);							// Offset to add to each index when looking up vertices
}

Mesh* Entity::GetMesh()
{
	return mesh;
}

Transform* Entity::GetTransform()
{
	return &transform;
}

Material* Entity::GetMaterial()
{
	return material;
}
