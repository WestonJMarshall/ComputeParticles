#pragma once
#include "SimpleShader.h"
#include <DirectXMath.h>
#include <wrl/client.h>
#include <d3d11.h>

class Material
{
public:

	Material(SimplePixelShader* _pixelShader, SimpleVertexShader* _vertexShader, DirectX::XMFLOAT4 _colorTint,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _textureSRV, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _metalSRV,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _roughSRV, Microsoft::WRL::ComPtr<ID3D11SamplerState> _samplerState);

	Material(SimplePixelShader* _pixelShader, SimpleVertexShader* _vertexShader, DirectX::XMFLOAT4 _colorTint,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _textureSRV, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _normalSRV,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _metalSRV, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _roughSRV, 
		Microsoft::WRL::ComPtr<ID3D11SamplerState> _samplerState);

	~Material();

	void SetColorTint(float r, float g, float b, float a);
	DirectX::XMFLOAT4 GetColorTint();
	float GetSpecularIntensity();
	SimplePixelShader* GetPixelShader();
	SimpleVertexShader* GetVertexShader();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetTextureSRV();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetNormalSRV();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetMetalSRV();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetRoughSRV();
	Microsoft::WRL::ComPtr<ID3D11SamplerState> GetSamplerState();

private:

	DirectX::XMFLOAT4 colorTint;

	SimplePixelShader* pixelShader;
	SimpleVertexShader* vertexShader;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metalSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalSRV;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
};

