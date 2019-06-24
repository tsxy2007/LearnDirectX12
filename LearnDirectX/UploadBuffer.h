#pragma once
#include "D3DUtil.h"
#include "DXSampleHelper.h"
#include "d3dx12.h"
using namespace DirectX;

const int gNumFrameResources = 3;

// 创建常量缓存区
struct ObjectConstants
{
	XMFLOAT4X4 WorldViewProj = d3dUtil::Identity4x4();
};


struct RenderItem
{
	RenderItem() = default;
	XMFLOAT4X4 World = d3dUtil::Identity4x4();
	int NumFrameDirty = gNumFrameResources;
	UINT ObjCBIndex = -1;
	MeshGeometry* Geo = nullptr;
	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	UINT BaseVertexLocation = 0;
};

template<typename T>
class UploadBuffer
{
public:
	UploadBuffer(ID3D12Device* device,UINT elementCount,bool isConstantBuffer):
		mIsConstantBuffer(isConstantBuffer)
	{
		mElementByteSize = sizeof(T);
		if (isConstantBuffer)
		{
			mElementByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(T));
		}
		ThrowIfFailed(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(mElementByteSize * elementCount),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&mUploadBuffer)
		));
		
		ThrowIfFailed(mUploadBuffer->Map(0, &IndexReadRange, reinterpret_cast<void**>(&mMappedData)));
	}
	UploadBuffer(const UploadBuffer& rhs) = delete;
	UploadBuffer& operator=(const UploadBuffer& rhs) = delete;
	~UploadBuffer()
	{
		if (mUploadBuffer != nullptr)
		{
			mUploadBuffer->Unmap(0, nullptr);
		}
		mMappedData = nullptr;
	}
	ID3D12Resource* Resource() const { return mUploadBuffer.Get(); }
	void CopyData(int elementIndex, const T& data)
	{
		memcpy(&mMappedData[elementIndex * mElementByteSize], &data, sizeof(T));
	}

	void CopyDataList(int elementIndex, int elementCount, const T* Data)
	{
		memcpy(mMappedData,&Data,elementCount);
	}
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> mUploadBuffer;
	BYTE* mMappedData = nullptr;

	UINT mElementByteSize = 0;
	bool mIsConstantBuffer = false;
	D3D12_RANGE IndexReadRange = { 0, 0 };
};