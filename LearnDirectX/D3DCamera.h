#pragma once
#include "stdafx.h"
using namespace DirectX;
class D3DCamera
{
public:
	D3DCamera();
	~D3DCamera();

	void SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);

	const XMMATRIX& GetViewMatrix()const;
	const XMMATRIX& GetProjectionMatrix()const;
	const XMMATRIX& GetViewAndProj()const;


	const XMVECTOR& GetPostionVector()const;
	const XMFLOAT3& GetPostionFloat3()const;
	const XMVECTOR& GetRotationVector()const;
	const XMFLOAT3& GetRotationFloat3()const;

	const XMVECTOR& GetForwardVector();
	const XMVECTOR& GetBackVector();
	const XMVECTOR& GetRightVector();
	const XMVECTOR& GetLeftVector();

	void SetPosition(const XMVECTOR& pos);
	void SetPosition(float x, float y, float z);
	void MoveBy(const XMVECTOR& pos);
	void MoveBy(float x, float y, float z);

	void SetRotation(const XMVECTOR& pos);
	void SetRotation(float x, float y, float z);
	void RotationBy(const XMVECTOR& pos);
	void RotationBy(float x, float y, float z);

	void SetLookAtPos(XMFLOAT3 LookAtPos);

private:
	void UpdateViewMatrix();
	DirectX::XMVECTOR mPosVector;
	DirectX::XMFLOAT3 mPosFloat3;
	DirectX::XMVECTOR mRotVector;
	DirectX::XMFLOAT3 mRotFloat3;
	DirectX::XMMATRIX mViewMatrix;
	DirectX::XMMATRIX mProjectionMatrix;

	const XMVECTOR DEFAULT_FORWARD_VECTOR = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	const XMVECTOR DEFAULT_UP_VECTOR = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	const XMVECTOR DEFAULT_RIGHT_VECTOR = XMVectorSet(1.f, 0.f, 0.f, 0.f);
	const XMVECTOR DEFAULT_BACK_VECTOR = XMVectorSet(0.f, 0.f, -1.f, 0.f);
	const XMVECTOR DEFAULT_LEFT_VECTOR = XMVectorSet(-1.f, 0.f, 0.f, 0.f);
	const XMVECTOR DEFAULT_DOWN_VECTOR = XMVectorSet(0.f, 0.f, -1.f, 0.f);

	XMVECTOR vec_forward;
	XMVECTOR vec_left;
	XMVECTOR vec_right;
	XMVECTOR vec_back;
};