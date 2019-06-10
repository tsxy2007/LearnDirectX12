#pragma once
#include "stdafx.h"

using namespace Microsoft::WRL;
class d3dUtil
{
public:

	static UINT CalcConstantBufferByteSize(UINT byteSize)
	{
		return (byteSize + 255) & ~255;
	}

	static Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* cmList,
		const void* initData,
		UINT64 byteSize,
		UINT64 Alignment,
		Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer);

	static ComPtr<ID3D12Resource> CreateBufferPlacedResource(
		ID3D12Device* InDevice,
		UINT64 SizeInBytes,
		D3D12_HEAP_TYPE InHeapType
	);

	static DirectX::XMFLOAT4X4 Identity4x4()
	{
		static DirectX::XMFLOAT4X4 I(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

		return I;
	};

};