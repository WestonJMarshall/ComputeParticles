#pragma once
#include "Transform.h"
#include <Windows.h>

class Camera
{
public:
	Camera();
	Camera(DirectX::XMFLOAT3 _position, float _fieldOfView, float _aspectRatio, float _nearClipDistance, float _farClipDistance, float _moveSpeed, float _lookSpeed);
	~Camera();

	Transform GetTransform();
	DirectX::XMFLOAT4X4* GetViewMatrix();
	DirectX::XMFLOAT4X4* GetProjectionMatrix();

	void UpdateProjectionMatrix(float aspectRatio, float fieldOfView, float nearClipDistance, float farClipDistance);
	void UpdateViewMatrix();
	void Update(float dt, HWND windowHandle);

private:

	Transform transform;

	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;

	POINT prevMousePosition;

	float fieldOfView;
	float aspectRatio;
	float nearClipDistance;
	float farClipDistance;
	float moveSpeed;
	float lookSpeed;
};

