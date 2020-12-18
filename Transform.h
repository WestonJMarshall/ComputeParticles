#pragma once
#include <DirectXMath.h>

class Transform
{
public:
	Transform();

	bool dirty;

	void SetPosition(float x, float y, float z);
	void SetRotationYPR(float z, float y, float x);
	void SetScale(float x, float y, float z);

	void TranslateAbsolute(float x, float y, float z);
	void TranslateRelative(float alongRight, float alongUp, float alongForward);
	void RotateYPR(float pitch, float yaw, float roll);
	//void Rotate(float x, float y, float z);
	void Scale(float x, float y, float z);

	DirectX::XMFLOAT3* GetPosition();
	DirectX::XMFLOAT3 GetRotationYPR();
	DirectX::XMFLOAT3 GetScale();

	DirectX::XMFLOAT4X4 GetWorldMatrix();

private:
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT3 rotationYPR;
};

