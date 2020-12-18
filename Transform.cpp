#include "Transform.h"

using namespace DirectX;

Transform::Transform()
{
	XMStoreFloat4x4(&worldMatrix, DirectX::XMMatrixIdentity());
	dirty = false;
	position = DirectX::XMFLOAT3(0, 0, 0);
	scale = DirectX::XMFLOAT3(1, 1, 1);
	rotationYPR = DirectX::XMFLOAT3(0, 0, 0);
}

void Transform::SetPosition(float x, float y, float z)
{
	//Will set dirty to true only if position has changed (written to have no if statement)
	dirty = ((x - position.x) || (y - position.y) || (z - position.z)) || dirty;

	position = DirectX::XMFLOAT3(x, y, z);
}

void Transform::SetRotationYPR(float yaw, float pitch, float roll)
{
	dirty = ((yaw - rotationYPR.x) || (pitch - rotationYPR.y) || (roll - rotationYPR.z)) || dirty;

	rotationYPR = DirectX::XMFLOAT3(yaw, pitch, roll);
}

void Transform::SetScale(float x, float y, float z)
{
	dirty = ((x - scale.x) || (y - scale.y) || (z - scale.z)) || dirty;

	scale = DirectX::XMFLOAT3(x, y, z);
}

void Transform::TranslateAbsolute(float x, float y, float z)
{
	dirty = (x || y || z) || dirty;

	position.x += x;
	position.y += y;
	position.z += z;
}

void Transform::TranslateRelative(float x, float y, float z)
{
	dirty = (x || y || z) || dirty;

	DirectX::XMVECTOR currentDirection = DirectX::XMVector3Rotate(DirectX::XMVectorSet(x, y, z, 0), DirectX::XMQuaternionRotationRollPitchYawFromVector(DirectX::XMLoadFloat3(&rotationYPR)));
	XMStoreFloat3(&position, XMLoadFloat3(&position) + currentDirection);
}

void Transform::RotateYPR(float x, float y, float z)
{
	dirty = (x || y || z) || dirty;

	rotationYPR.x += x;
	rotationYPR.y += y;
	rotationYPR.z += z;
}

void Transform::Scale(float x, float y, float z)
{
	dirty = (x || y || z) || dirty;

	scale.x += x;
	scale.y += y;
	scale.z += z;
}

DirectX::XMFLOAT3* Transform::GetPosition()
{
	return &position;
}

DirectX::XMFLOAT3 Transform::GetRotationYPR()
{
	return rotationYPR;
}

DirectX::XMFLOAT3 Transform::GetScale()
{
	return scale;
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
	if (dirty)
	{
		dirty = false;

		DirectX::XMMATRIX tra = DirectX::XMMatrixTranslationFromVector(XMLoadFloat3(&position));
		DirectX::XMMATRIX rot = DirectX::XMMatrixRotationRollPitchYawFromVector(DirectX::XMLoadFloat3(&rotationYPR));
		DirectX::XMMATRIX scl = DirectX::XMMatrixScalingFromVector(XMLoadFloat3(&scale));
		DirectX::XMMATRIX world = scl * rot * tra;
		XMStoreFloat4x4(&worldMatrix, world);
	}
	return worldMatrix;
}
