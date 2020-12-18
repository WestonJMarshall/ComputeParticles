#include "SkyBox.h"
#include "DDSTextureLoader.h"

SkyBox::SkyBox(Mesh* _mesh, Microsoft::WRL::ComPtr<ID3D11SamplerState> _samplerState, Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, const wchar_t* texturePath, const wchar_t* pixelShaderPath, const wchar_t* vertexShaderPath)
{
	vertexShader = nullptr;
	pixelShader = nullptr;

	mesh = _mesh;
	samplerState = _samplerState;

	vertexShader = new SimpleVertexShader(device.Get(), context.Get(), vertexShaderPath);
	pixelShader = new SimplePixelShader(device.Get(), context.Get(), pixelShaderPath);

	D3D11_RASTERIZER_DESC rasterizerDescription = {};
	rasterizerDescription.FillMode = D3D11_FILL_SOLID;
	rasterizerDescription.CullMode = D3D11_CULL_FRONT;
	device.Get()->CreateRasterizerState(&rasterizerDescription, rasterizerState.GetAddressOf());

	D3D11_DEPTH_STENCIL_DESC depthStencilDescription = {};
	depthStencilDescription.DepthEnable = true;
	depthStencilDescription.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	device.Get()->CreateDepthStencilState(&depthStencilDescription, depthStencilState.GetAddressOf());

	DirectX::CreateDDSTextureFromFile(device.Get(), context.Get(), texturePath, 0, textureSRV.GetAddressOf());
}

SkyBox::~SkyBox()
{
	delete vertexShader;
	delete pixelShader;
}

void SkyBox::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, DirectX::XMFLOAT4X4* viewMatrix, DirectX::XMFLOAT4X4* projectionMatrix)
{
	context.Get()->RSSetState(rasterizerState.Get());
	context->OMSetDepthStencilState(depthStencilState.Get(), 0);

	vertexShader->SetShader();
	pixelShader->SetShader();

	vertexShader->SetMatrix4x4("viewMatrix", *viewMatrix);
	vertexShader->SetMatrix4x4("projectionMatrix", *projectionMatrix);

	pixelShader->SetSamplerState("textureSampler", samplerState.Get());
	pixelShader->SetShaderResourceView("textureCube", textureSRV.Get());

	vertexShader->CopyAllBufferData();
	pixelShader->CopyAllBufferData();

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, mesh->GetVertexBuffer().GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(mesh->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

	context->DrawIndexed(mesh->GetIndexCount(), 0, 0);		

	context.Get()->RSSetState(0);
	context->OMSetDepthStencilState(0, 0);
}
