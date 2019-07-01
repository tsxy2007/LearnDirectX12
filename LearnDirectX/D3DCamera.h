#pragma once
#include "stdafx.h"

class D3DCamera
{
public:
	D3DCamera();
	~D3DCamera();

	void LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp);

	void SetLens(float FovAngleY,float AspectRatio,float NearZ,float FarZ);

	void UpdateViewMatrix();

	void SetPosition(float X, float Y, float Z);

	void SetPosition(DirectX::FXMVECTOR pos);

	void MoveBy(float X = 0.f, float Y = 0.f, float Z = 0.f);

	DirectX::XMMATRIX GetView() const;

	DirectX::XMMATRIX GetProj() const;

	DirectX::XMMATRIX GetViewAndProj() const;

private:
	DirectX::XMFLOAT3 mCameraPosition = { 0.f,0.f,0.f };
	DirectX::XMFLOAT3 mCameraDirection = { 0.f,0.f,1.f };
	DirectX::XMFLOAT3 mCameraUp = { 0.f,1.f,0.f };
	DirectX::XMFLOAT3 mCameraRight = { 1.f,0.f,0.f };

	DirectX::XMFLOAT4X4 mViewMatrix = d3dUtil::Identity4x4();
	DirectX::XMFLOAT4X4 mPerspectiveMatrix = d3dUtil::Identity4x4();
};