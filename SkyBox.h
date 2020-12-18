#pragma once
#include "Mesh.h"
#include "SimpleShader.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <WICTextureLoader.h>

class SkyBox
{
public:
	SkyBox(Mesh* _mesh, Microsoft::WRL::ComPtr<ID3D11SamplerState> _samplerState, Microsoft::WRL::ComPtr<ID3D11Device> device, 
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, const wchar_t* texturePath, const wchar_t* pixelShaderPath, const wchar_t* vertexShaderPath);
	~SkyBox();

	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, DirectX::XMFLOAT4X4* viewMatrix, DirectX::XMFLOAT4X4* projectionMatrix);

private:

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	Mesh* mesh;
	SimplePixelShader* pixelShader;
	SimpleVertexShader* vertexShader;
};

