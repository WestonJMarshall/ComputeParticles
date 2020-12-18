#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h> 
#include "WICTextureLoader.h"
#include "SimpleShader.h"
#include "Camera.h"
#include "Lights.h"

struct Particle
{
	DirectX::XMFLOAT3 Position;
	float Age;
	float Size;
	DirectX::XMFLOAT3 padding;
};

class Emitter
{
public:

	Emitter(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
		SimpleComputeShader* initCS,
		SimpleComputeShader* updateCS, 
		SimpleComputeShader* argumentPassCS,
		SimpleVertexShader* particleVS,
		SimplePixelShader* particlePS,
		const wchar_t* initPath, const wchar_t* definitionsPath);
	~Emitter();

	void Update(float dt);
	void Draw(DirectX::XMFLOAT4X4* view, DirectX::XMFLOAT4X4* proj, Light* light, Microsoft::WRL::ComPtr<ID3D11SamplerState> clampSampler, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cellRamp);

private:

	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencil;

	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	SimpleComputeShader* updateCS;
	SimpleComputeShader* argumentPassCS;
	SimpleVertexShader* particleVS;
	SimplePixelShader* particlePS;

	// Indirect draw buffer
	Microsoft::WRL::ComPtr<ID3D11Buffer> drawArgsBuffer;

	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particlesUAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particlesSRV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> drawArgsUAV;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particleDefinitionsSRV;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> particleDefinitionsSamplerState;
};

