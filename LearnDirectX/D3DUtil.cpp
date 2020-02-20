#include "D3DUtil.h"
#include "DXSampleHelper.h"
#include <comdef.h>
Microsoft::WRL::ComPtr<ID3D12Resource> d3dUtil::CreateDefaultBuffer(ID3D12Device* device, 
	ID3D12GraphicsCommandList* cmList,
	const void* initData, 
	UINT64 byteSize, 
	Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer)
{
	ComPtr<ID3D12Resource> DefaultResource;

	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(DefaultResource.GetAddressOf())
	));

	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(uploadBuffer.GetAddressOf())
	));

	D3D12_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pData = initData;
	subResourceData.RowPitch = byteSize;
	subResourceData.SlicePitch = subResourceData.RowPitch;
	
	cmList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(DefaultResource.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));

	UpdateSubresources<1>(cmList, DefaultResource.Get(), uploadBuffer.Get(), 0, 0, 1, &subResourceData);

	cmList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(DefaultResource.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));


	return DefaultResource;
}

ComPtr<ID3D12Resource> d3dUtil::CreateBufferPlacedResource(ID3D12Device* InDevice,UINT64 SizeInBytes,D3D12_HEAP_TYPE InHeapType)
{
	ComPtr<ID3D12Resource> Resource;
	D3D12_HEAP_PROPERTIES Heap_Properties = { InHeapType };
	D3D12_HEAP_DESC HeapDesc = { SizeInBytes ,Heap_Properties};
	D3D12_RESOURCE_DESC RD = CD3DX12_RESOURCE_DESC::Buffer(SizeInBytes);
	ComPtr<ID3D12Heap> Heap;
	InDevice->CreateHeap(&HeapDesc, IID_PPV_ARGS(&Heap));
	InDevice->CreatePlacedResource(Heap.Get(),0, &RD, D3D12_RESOURCE_STATE_COPY_DEST,nullptr, IID_PPV_ARGS(&Resource));

	//InDevice->CreateCommittedResource(
	//	&Heap_Properties,
	//	D3D12_HEAP_FLAG_NONE, 
	//	&RD, 
	//	D3D12_RESOURCE_STATE_COPY_DEST, 
	//	nullptr, 
	//	IID_PPV_ARGS(&Resource)
	//);

	return Resource;
}

Microsoft::WRL::ComPtr<ID3DBlob> d3dUtil::CompileShader(
	const std::wstring& filename, 
	const D3D_SHADER_MACRO* defines, 
	const std::string & entrypoint, 
	const std::string& target)
{
	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	HRESULT hr = S_OK;
	ComPtr<ID3DBlob> byteCode = nullptr;
	ComPtr<ID3DBlob> errors;

	hr = D3DCompileFromFile(
		filename.c_str(),
		defines,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entrypoint.c_str(),
		target.c_str(),
		compileFlags,
		0,
		&byteCode,
		&errors);

	if (errors != nullptr)
	{
		OutputDebugStringA((char*)errors->GetBufferPointer());
	}
	ThrowIfFailed(hr);
	return byteCode;
}

DxException::DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber)
	:ErrorCode(hr)
	,FunctionName(functionName)
	,FileName(filename)
	,LineNumber(lineNumber)
{

}

std::wstring DxException::ToString() const
{
	_com_error err(ErrorCode);
	std::wstring msg = err.ErrorMessage();

	return FunctionName + L" failed in " + FileName + L"; line " + std::to_wstring(LineNumber) + L"; error: " + msg;
}
