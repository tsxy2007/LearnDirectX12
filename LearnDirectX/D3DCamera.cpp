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
	XMStoreFloat3(&mCameraWorldUp, worldUp);
	XMStoreFloat3(&mCameraFront, target);
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
	XMVECTOR vCameraPos = XMLoadFloat3(&mCameraPosition);
	XMVECTOR vCameraFront = XMLoadFloat3(&mCameraFront);
	XMVECTOR vCameraWorldUp = XMLoadFloat3(&mCameraWorldUp);

	XMMATRIX view = DirectX::XMMatrixLookAtLH(vCameraPos, vCameraPos+ vCameraFront, vCameraWorldUp);
	XMStoreFloat4x4(&mViewMatrix,
		view);

}

void D3DCamera::SetPosition(float X, float Y, float Z)
{
	mCameraPosition.x = X;
	mCameraPosition.y = Y;
	mCameraPosition.z = Z;
}

void D3DCamera::SetPosition(DirectX::FXMVECTOR pos)
{
	XMStoreFloat3(&mCameraPosition, pos);
}

void D3DCamera::MoveBy(float X, float Y, float Z)
{
	mCameraPosition.x += X;
	mCameraPosition.y += Y;
	mCameraPosition.z += Z;
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