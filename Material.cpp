#include "Material.h"

Material::Material(SimplePixelShader* _pixelShader, SimpleVertexShader* _vertexShader, DirectX::XMFLOAT4 _colorTint, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _textureSRV, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _metalSRV, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _roughSRV, Microsoft::WRL::ComPtr<ID3D11SamplerState> _samplerState)
{
	pixelShader = _pixelShader;
	vertexShader = _vertexShader;
	colorTint = _colorTint;
	textureSRV = _textureSRV;
	normalSRV = nullptr;
	metalSRV = _metalSRV;
	roughSRV = _roughSRV;
	samplerState = _samplerState;
}

Material::Material(SimplePixelShader* _pixelShader, SimpleVertexShader* _vertexShader, DirectX::XMFLOAT4 _colorTint, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _textureSRV, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _normalSRV, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _metalSRV, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _roughSRV, Microsoft::WRL::ComPtr<ID3D11SamplerState> _samplerState)
{
	pixelShader = _pixelShader;
	vertexShader = _vertexShader;
	colorTint = _colorTint;
	textureSRV = _textureSRV;
	normalSRV = _normalSRV;
	metalSRV = _metalSRV;
	roughSRV = _roughSRV;
	samplerState = _samplerState;
}

Material::~Material()
{
}

void Material::SetColorTint(float r, float g, float b, float a)
{
	colorTint = DirectX::XMFLOAT4(r, g, b, a);
}

DirectX::XMFLOAT4 Material::GetColorTint()
{
	return colorTint;
}

SimplePixelShader* Material::GetPixelShader()
{
	return pixelShader;
}

SimpleVertexShader* Material::GetVertexShader()
{
	return vertexShader;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Material::GetTextureSRV()
{
	return textureSRV;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Material::GetNormalSRV()
{
	return normalSRV;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Material::GetMetalSRV()
{
	return metalSRV;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Material::GetRoughSRV()
{
	return roughSRV;
}

Microsoft::WRL::ComPtr<ID3D11SamplerState> Material::GetSamplerState()
{
	return samplerState;
}
