#pragma once

#include "DXSample.h"
#include "UploadBuffer.h"

using namespace DirectX;

using Microsoft::WRL::ComPtr;



// 创建常量缓存区
struct ObjectConstants
{
	XMFLOAT4X4 WorldViewProj = d3dUtil::Identity4x4();
};
class D3DCamera;
class D3D12HelloTriangle : public DXSample
{
public:
	D3D12HelloTriangle(UINT width,UINT height,std::wstring name);
	~D3D12HelloTriangle();
	virtual void OnInit();
	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnDestroy();


	 template<typename ... Args>
	void Print(int inLogLevel, const char* inFormat, Args ... args);

	bool (*func)(...);
private:
	static const UINT FrameCount = 2;
	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT2 uv;
	};

	CD3DX12_VIEWPORT m_viewport;
	CD3DX12_RECT m_scissorRect;
	ComPtr<IDXGISwapChain3> m_swapChain;
	ComPtr<ID3D12Device> m_device;
	ComPtr<ID3D12Resource1> m_renderTargets[FrameCount];
	ComPtr<ID3D12Resource>	m_ITextureUpload;
	
	ComPtr<ID3D12CommandAllocator>m_commandAllocator;
	ComPtr< ID3D12CommandQueue>m_commandQueue;
	ComPtr< ID3D12RootSignature> m_rootSignature;
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	ComPtr<ID3D12DescriptorHeap> m_srvHeap;
	ComPtr<ID3D12PipelineState>m_pipelineState;
	ComPtr<ID3D12GraphicsCommandList2>m_commandList;
	UINT m_rtvDescriptorSize;

	ComPtr<ID3D12Resource1> m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

	ComPtr<ID3D12Resource> m_IndexBuffer;
	D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;

	ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;

	std::unique_ptr<UploadBuffer<ObjectConstants>> ConstantBuffer = nullptr;

	ComPtr<ID3D12Resource> m_CBVBuffer;
	//D3D12_CONSTANT_BUFFER_VIEW_DESC

	UINT m_frameIndex;
	HANDLE m_fenceEvent;
	ComPtr<ID3D12Fence1>m_fence;
	UINT64 m_fenceValue;

	std::shared_ptr<D3DCamera> Camera;
	void LoadPipeline();

	void BuildDescriptorHeaps();
	void LoadAssets();
	void PopulateCommandList();
	void WaitForPreviousFrame();
};

