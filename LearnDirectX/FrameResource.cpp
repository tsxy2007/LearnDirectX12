#include "FrameResource.h"

FrameResource::FrameResource(ID3D12Device * device, UINT passCount, UINT objectCount)
{
	ThrowIfFailed(device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(CmdListAlloc.GetAddressOf())
	));
	ObjectCB = make_unique<UploadBuffer<ObjectConstants>>(device,objectCount,true);
}

FrameResource::~FrameResource()
{
}
