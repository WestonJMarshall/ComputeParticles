#include "Camera.h"

Camera::Camera()
{
	fieldOfView = 90;
	aspectRatio = 16.0f / 9.0f;;
	nearClipDistance = 0.05f;
	farClipDistance = 1000.0f;
	moveSpeed = 1;
	lookSpeed = 1;

	prevMousePosition = { 0, 0 };
	DirectX::XMStoreFloat4x4(&viewMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&projectionMatrix, DirectX::XMMatrixIdentity());
}

Camera::Camera(DirectX::XMFLOAT3 _position, float _fieldOfView, float _aspectRatio, float _nearClipDistance, float _farClipDistance, float _moveSpeed, float _lookSpeed)
{
	transform.SetPosition(_position.x, _position.y, _position.z);
	fieldOfView = _fieldOfView;
	aspectRatio = _aspectRatio;
	nearClipDistance = _nearClipDistance;
	farClipDistance = _farClipDistance;
	moveSpeed = _moveSpeed;
	lookSpeed = _lookSpeed;

	prevMousePosition = { 0, 0 };
	DirectX::XMStoreFloat4x4(&viewMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&projectionMatrix, DirectX::XMMatrixIdentity());

	UpdateProjectionMatrix(aspectRatio, fieldOfView, nearClipDistance, farClipDistance);
	UpdateViewMatrix();
}

Camera::~Camera()
{
}

Transform Camera::GetTransform()
{
	return transform;
}

DirectX::XMFLOAT4X4* Camera::GetViewMatrix()
{
	return &viewMatrix;
}

DirectX::XMFLOAT4X4* Camera::GetProjectionMatrix()
{
	return &projectionMatrix;
}

void Camera::UpdateProjectionMatrix(float aspectRatio, float fieldOfView, float nearClipDistance, float farClipDistance)
{
	DirectX::XMStoreFloat4x4(&projectionMatrix, DirectX::XMMatrixPerspectiveFovLH(fieldOfView, aspectRatio, nearClipDistance, farClipDistance));
}

void Camera::UpdateViewMatrix()
{
	DirectX::XMFLOAT3 currentRotation = transform.GetRotationYPR();
	DirectX::XMVECTOR currentDirection = DirectX::XMVector3Rotate(DirectX::XMVectorSet(0, 0, 1, 0), DirectX::XMQuaternionRotationRollPitchYawFromVector(DirectX::XMLoadFloat3(&currentRotation)));
	DirectX::XMStoreFloat4x4(&viewMatrix, DirectX::XMMatrixLookToLH(DirectX::XMLoadFloat3(transform.GetPosition()), currentDirection, DirectX::XMVectorSet(0, 1, 0, 0)));
}

void Camera::Update(float dt, HWND windowHandle)
{
	POINT mousePos = {};
	GetCursorPos(&mousePos);
	ScreenToClient(windowHandle, &mousePos);
	
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) 
	{ 
		float xDif = (float)prevMousePosition.x - mousePos.x;
		float yDif = (float)prevMousePosition.y - mousePos.y;
	
		transform.RotateYPR(-yDif * dt * lookSpeed, -xDif * dt * lookSpeed, 0);
	}
	
	float frameSpeed = moveSpeed;
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000) { frameSpeed *= 5.0f; }
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000) { frameSpeed *= 0.5f; }
	
	if (GetAsyncKeyState('W') & 0x8000) { transform.TranslateRelative(0, 0, frameSpeed * dt); }
	if (GetAsyncKeyState('S') & 0x8000) { transform.TranslateRelative(0, 0, -frameSpeed * dt); }
	if (GetAsyncKeyState('A') & 0x8000) { transform.TranslateRelative(-frameSpeed * dt, 0, 0); }
	if (GetAsyncKeyState('D') & 0x8000) { transform.TranslateRelative(frameSpeed * dt, 0, 0); }
	if (GetAsyncKeyState('Q') & 0x8000) { transform.TranslateAbsolute(0, -frameSpeed * dt, 0); }
	if (GetAsyncKeyState('E') & 0x8000) { transform.TranslateAbsolute(0, frameSpeed * dt, 0); }
	
	prevMousePosition = mousePos;
	transform.GetWorldMatrix();
	UpdateViewMatrix();
}

