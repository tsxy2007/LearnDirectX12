#include "D3DUtil.h"
#include "DXSampleHelper.h"
Microsoft::WRL::ComPtr<ID3D12Resource> d3dUtil::CreateDefaultBuffer(ID3D12Device* device, 
	ID3D12GraphicsCommandList* cmList,
	const void* initData, 
	UINT64 byteSize, 
	UINT64 Alignment,
	Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer)
{
	ComPtr<ID3D12Resource> DefaultResource;
	ComPtr<ID3D12Resource> UploadResource;



	D3D12_RESOURCE_DESC UploadResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(byteSize);

	ID3D12Heap* UploadHeap;
	D3D12_HEAP_PROPERTIES UploadHeapProperties = { D3D12_HEAP_TYPE_UPLOAD };
	D3D12_HEAP_DESC UpLoadHeapDesc = { byteSize,UploadHeapProperties,Alignment };
	ThrowIfFailed(device->CreateHeap(&UpLoadHeapDesc, IID_PPV_ARGS(&UploadHeap)));
	ThrowIfFailed(device->CreatePlacedResource(
		UploadHeap,
		0,
		&UploadResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&UploadResource)
	));

	D3D12_RESOURCE_DESC DefaultResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_UNKNOWN,byteSize,1);


	ID3D12Heap* pUploadHeap;
	D3D12_HEAP_PROPERTIES DefaultHeapProperties = { D3D12_HEAP_TYPE_DEFAULT };
	D3D12_HEAP_DESC heapdesc = { byteSize ,DefaultHeapProperties,Alignment };
	ThrowIfFailed(device->CreateHeap(&heapdesc, IID_PPV_ARGS(&pUploadHeap)));

	ThrowIfFailed(device->CreatePlacedResource(
		pUploadHeap,
		0,
		&DefaultResourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		NULL,
		IID_PPV_ARGS(&DefaultResource)));



	return DefaultResource;
}
