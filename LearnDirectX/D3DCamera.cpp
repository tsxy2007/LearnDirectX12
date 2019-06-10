#include "D3DCamera.h"
using namespace DirectX;

D3DCamera::D3DCamera()
{
}

D3DCamera::~D3DCamera()
{
}

void D3DCamera::LookAt(FXMVECTOR pos, FXMVECTOR target,FXMVECTOR worldUp)
{
	XMVECTOR L = XMVector3Normalize(XMVectorSubtract(target, pos));
	XMVECTOR R = XMVector3Normalize(XMVector3Cross(worldUp, L));
	XMVECTOR U = XMVector3Cross(L, R);

	XMStoreFloat3(&mCameraPosition, pos);
	XMStoreFloat3(&mCameraDirection, L);
	XMStoreFloat3(&mCameraRight, R);
	XMStoreFloat3(&mCameraUp, U);
	XMMATRIX view = DirectX::XMMatrixLookAtLH(pos, target, worldUp);
	XMStoreFloat4x4(&mViewMatrix, 
		view);
}

void D3DCamera::SetLens(float FovAngleY, float AspectRatio, float NearZ, float FarZ)
{
	XMMATRIX mMatrix = XMMatrixPerspectiveFovLH(FovAngleY, AspectRatio, NearZ, FarZ);
	XMStoreFloat4x4(&mPerspectiveMatrix, mMatrix);
}

void D3DCamera::UpdateViewMatrix()
{
	XMVECTOR R = XMLoadFloat3(&mCameraRight);
	XMVECTOR U = XMLoadFloat3(&mCameraUp);
	XMVECTOR L = XMLoadFloat3(&mCameraDirection);
	XMVECTOR P = XMLoadFloat3(&mCameraPosition);

	// Keep camera's axes orthogonal to each other and of unit length.
	L = XMVector3Normalize(L);
	U = XMVector3Normalize(XMVector3Cross(L,R));

	// U, L already ortho-normal, so no need to normalize cross product.
	R = XMVector3Cross(U, L);

	// Fill in the view matrix entries.
	float x = -XMVectorGetX(XMVector3Dot(P, R));
	float y = -XMVectorGetX(XMVector3Dot(P, U));
	float z = -XMVectorGetX(XMVector3Dot(P, L));

	XMStoreFloat3(&mCameraRight, R);
	XMStoreFloat3(&mCameraUp, U);
	XMStoreFloat3(&mCameraDirection, L);

	mViewMatrix(0, 0) = mCameraRight.x;
	mViewMatrix(1, 0) = mCameraRight.y;
	mViewMatrix(2, 0) = mCameraRight.z;
	mViewMatrix(3, 0) = x;

	mViewMatrix(0, 1) = mCameraUp.x;
	mViewMatrix(1, 1) = mCameraUp.y;
	mViewMatrix(2, 1) = mCameraUp.z;
	mViewMatrix(3, 1) = y;

	mViewMatrix(0, 2) = mCameraDirection.x;
	mViewMatrix(1, 2) = mCameraDirection.y;
	mViewMatrix(2, 2) = mCameraDirection.z;
	mViewMatrix(3, 2) = z;

	mViewMatrix(0, 3) = 0.0f;
	mViewMatrix(1, 3) = 0.0f;
	mViewMatrix(2, 3) = 0.0f;
	mViewMatrix(3, 3) = 1.0f;
}

void D3DCamera::SetPosition(float X, float Y, float Z)
{
	mCameraPosition.x = X;
	mCameraPosition.y = Y;
	mCameraPosition.z = Z;
}

DirectX::XMMATRIX D3DCamera::GetView() const
{
	return XMLoadFloat4x4(&mViewMatrix);
}

DirectX::XMMATRIX D3DCamera::GetProj() const
{
	return XMLoadFloat4x4(&mPerspectiveMatrix);
}

DirectX::XMMATRIX D3DCamera::GetViewAndProj() const
{
	DirectX::XMFLOAT4X4 Matrix;
	XMStoreFloat4x4( &Matrix, GetView()* GetProj());
	return XMLoadFloat4x4(&Matrix);
}