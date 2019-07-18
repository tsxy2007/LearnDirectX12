#include "D3DCamera.h"
using namespace DirectX;

D3DCamera::D3DCamera()
{
	this->mPosFloat3 = { 0.f,0.f,0.f };
	this->mPosVector = XMLoadFloat3(&this->mPosFloat3);
	this->mRotFloat3 = { 0.f,0.f,0.f };
	this->mRotVector = XMLoadFloat3(&this->mRotFloat3);
	this->mScaleFloat3 = { 1.f,1.f,1.f };
	this->mScaleVector = XMLoadFloat3(&this->mScaleFloat3);
	this->UpdateViewMatrix();
}

D3DCamera::~D3DCamera()
{
}

void D3DCamera::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	float fovRadians = (fovDegrees / 360.0f) * XM_2PI;
	this->mProjectionMatrix = XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
}

const XMMATRIX & D3DCamera::GetViewMatrix() const
{
	// TODO: 在此处插入 return 语句
	return this->mViewMatrix;
}

const XMMATRIX & D3DCamera::GetProjectionMatrix() const
{
	// TODO: 在此处插入 return 语句
	return this->mProjectionMatrix;
}

const XMMATRIX & D3DCamera::GetViewAndProj() const
{
	// TODO: 在此处插入 return 语句
	return this->mViewMatrix * this->mProjectionMatrix;
}

const XMVECTOR & D3DCamera::GetPostionVector() const
{
	// TODO: 在此处插入 return 语句
	return this->mPosVector;
}

const XMFLOAT3 & D3DCamera::GetPostionFloat3() const
{
	// TODO: 在此处插入 return 语句
	return this->mPosFloat3;
}

const XMVECTOR & D3DCamera::GetRotationVector() const
{
	// TODO: 在此处插入 return 语句
	return this->mRotVector;
}

const XMFLOAT3 & D3DCamera::GetRotationFloat3() const
{
	// TODO: 在此处插入 return 语句
	return this->mRotFloat3;
}

const XMVECTOR & D3DCamera::GetForwardVector()
{
	// TODO: 在此处插入 return 语句
	return vec_forward;
}

const XMVECTOR & D3DCamera::GetBackVector()
{
	// TODO: 在此处插入 return 语句
	return vec_back;
}

const XMVECTOR & D3DCamera::GetRightVector()
{
	// TODO: 在此处插入 return 语句
	return vec_right;
}

const XMVECTOR & D3DCamera::GetLeftVector()
{
	// TODO: 在此处插入 return 语句
	return vec_left;
}

void D3DCamera::SetPosition(const XMVECTOR & pos)
{
	this->mPosVector = pos;
	XMStoreFloat3(&this->mPosFloat3, pos);
	UpdateViewMatrix();
}

void D3DCamera::SetPosition(float x, float y, float z)
{
	this->mPosFloat3.x = x;
	this->mPosFloat3.y = y;
	this->mPosFloat3.z = z;
	this->mPosVector = XMLoadFloat3(&this->mPosFloat3);
	UpdateViewMatrix();
}

void D3DCamera::MoveBy(const XMVECTOR & pos)
{
	this->mPosVector += pos;
	XMStoreFloat3(&this->mPosFloat3, pos);
	UpdateViewMatrix();
}

void D3DCamera::MoveBy(float x, float y, float z)
{
	this->mPosFloat3.x += x;
	this->mPosFloat3.y += y;
	this->mPosFloat3.z += z;
	this->mPosVector = XMLoadFloat3(&this->mPosFloat3);
	UpdateViewMatrix();
}

void D3DCamera::SetRotation(const XMVECTOR & rot)
{
	this->mRotVector = rot;
	XMStoreFloat3(&this->mRotFloat3, rot);
	UpdateViewMatrix();
}

void D3DCamera::SetRotation(float x, float y, float z)
{
	this->mRotFloat3.x = x;
	this->mRotFloat3.y = y;
	this->mRotFloat3.z = z;
	this->mRotVector = XMLoadFloat3(&this->mRotFloat3);
	UpdateViewMatrix();
}

void D3DCamera::RotationBy(const XMVECTOR & rot)
{
	this->mRotVector += rot;
	XMStoreFloat3(&this->mRotFloat3, rot);
	UpdateViewMatrix();
}

void D3DCamera::RotationBy(float x, float y, float z)
{
	this->mRotFloat3.x += x;
	this->mRotFloat3.y += y;
	this->mRotFloat3.z += z;
	this->mRotVector = XMLoadFloat3(&this->mRotFloat3);
	UpdateViewMatrix();
}

void D3DCamera::SetLookAtPos(XMFLOAT3 LookAtPos)
{
	if (LookAtPos.x == this->mPosFloat3.x && LookAtPos.y == this->mPosFloat3.y && LookAtPos.z == this->mPosFloat3.z)
	{
		return;
	}
	LookAtPos.x = this->mPosFloat3.x - LookAtPos.x;
	LookAtPos.y = this->mPosFloat3.y - LookAtPos.y;
	LookAtPos.z = this->mPosFloat3.z - LookAtPos.z;
	float pitch = 0.f;
	if (LookAtPos.y != 0.f)
	{
		const float distance = sqrt(LookAtPos.x * LookAtPos.x + LookAtPos.z  *LookAtPos.z);
		pitch = atan(LookAtPos.y / distance);
	}

	float yaw = 0.f;
	if (LookAtPos.x != 0.f)
	{
		yaw = atan(LookAtPos.x / LookAtPos.z);
	}
	if (LookAtPos.z > 0)
	{
		yaw += XM_PI;
	}
	this->SetRotation(pitch, yaw, 0.f);
}

void D3DCamera::UpdateViewMatrix()
{
	XMMATRIX RotMatrix = DirectX::XMMatrixRotationRollPitchYawFromVector(mRotVector);
	XMMATRIX PosMatrix = XMMatrixTranslationFromVector(mPosVector);
	XMMATRIX ScaleMatrix = XMMatrixScalingFromVector(mScaleVector);
	XMMATRIX TransformMatrix = ScaleMatrix * RotMatrix* PosMatrix;
	//XMMatrixScalingFromVector()
	XMVECTOR CameraTarget = DirectX::XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, TransformMatrix);
	XMVECTOR CameraUp = DirectX::XMVector3TransformCoord(this->DEFAULT_UP_VECTOR,RotMatrix);
	//CameraTarget += this->mPosVector;

	this->mViewMatrix = DirectX::XMMatrixLookAtLH(this->mPosVector, CameraTarget, CameraUp);

	XMMATRIX Matrix = DirectX::XMMatrixRotationRollPitchYaw(mRotFloat3.x, mRotFloat3.y, 0.f);
	this->vec_forward = DirectX::XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, Matrix);
	this->vec_back = DirectX::XMVector3TransformCoord(this->DEFAULT_BACK_VECTOR, Matrix);
	this->vec_left = DirectX::XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR, Matrix);
	this->vec_right = DirectX::XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR, Matrix);
}

