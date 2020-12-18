#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

struct Light
{
	DirectX::XMFLOAT4 ambientColor;

	DirectX::XMFLOAT4 diffuseColor;

	DirectX::XMFLOAT4 direction;

	DirectX::XMFLOAT4 position;

	DirectX::XMFLOAT4 valid;
};
