#pragma once
#include "stdafx.h"
#include "UploadBuffer.h"

using namespace Microsoft::WRL;
using namespace std;

class ID3D12Device;
class ID3D12CommandAllocator;

struct FrameResource
{
public:
	FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount);
	FrameResource(FrameResource& fr) = delete;
	FrameResource& operator=(const FrameResource& fr) = delete;
	~FrameResource();

	ComPtr<ID3D12CommandAllocator> CmdListAlloc;

	unique_ptr < UploadBuffer<ObjectConstants> > ObjectCB = nullptr;
	UINT64 Fence = 0;
};