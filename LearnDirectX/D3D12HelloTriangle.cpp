#include "stdafx.h"
#include "D3DCamera.h"
#include "D3D12HelloTriangle.h"
#include "UploadBuffer.h"
#include "FrameResource.h"

#define MipLevel  9

#define nWidth  256
#define nHeight  206


D3D12HelloTriangle::D3D12HelloTriangle(UINT width, UINT height, std::wstring name) :
	DXSample(width,height,name),
	m_frameIndex(0),
	m_viewport(0.f,0.f,static_cast<float>(width),static_cast<float>(height)),
	m_scissorRect(0,0,static_cast<LONG>(width),static_cast<LONG>(height)),
	m_rtvDescriptorSize(0)
{
	Camera = std::make_shared<D3DCamera>( );
}

D3D12HelloTriangle::~D3D12HelloTriangle()
{

}

void D3D12HelloTriangle::OnInit()
{
		LoadPipeline();
		BuildDescriptorHeaps();
		LoadAssets();
}

void D3D12HelloTriangle::LoadPipeline()
{

	
	UINT dxgiFactoryFlags = 0;
	HRESULT hr;
#if defined(_DEBUG)
	{
		ComPtr<ID3D12Debug1> debugController;
		hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
		if (SUCCEEDED(hr))
		{
			debugController->EnableDebugLayer();
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	}
#endif
	ComPtr<IDXGIFactory6> factory;
	ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

	if (m_useWarpDevice)
	{
		ComPtr<IDXGIAdapter4> warpAdapter;
		ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));
		ThrowIfFailed(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&m_device)
		));
	}
	else
	{
		ComPtr<IDXGIAdapter4> hardwareAdapter;
		GetHardwareAdapter(factory.Get(), &hardwareAdapter);
		ThrowIfFailed(D3D12CreateDevice(hardwareAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&m_device)
		));
	}
	//创建命令队列
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));

	//创建交换缓存区
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = FrameCount;
	swapChainDesc.Width = m_width;
	swapChainDesc.Height = m_height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;

	ComPtr<IDXGISwapChain1> swapChain;

	ThrowIfFailed(
		factory->CreateSwapChainForHwnd(
			m_commandQueue.Get(),
			Win32Application::GetHwnd(),
			&swapChainDesc,
			nullptr,
			nullptr,
			&swapChain
		)
	); 

	ThrowIfFailed(factory->MakeWindowAssociation(Win32Application::GetHwnd(), DXGI_MWA_NO_ALT_ENTER));
	
	ThrowIfFailed(swapChain.As(&m_swapChain));

	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	// 创建一个描述堆
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = FrameCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ThrowIfFailed(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));
		m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.NumDescriptors = 1;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		ThrowIfFailed(m_device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_srvHeap)));
	}

	//创建RenderTargetView
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
		for (UINT i = 0; i < FrameCount; i++)
		{
			ThrowIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i])));
			m_device->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, m_rtvDescriptorSize);
		}
	}
	
	//创建命令分配器
	m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator));

	m_device->SetName(L"Device");
}

void D3D12HelloTriangle::BuildDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = 1;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
	ThrowIfFailed(m_device->CreateDescriptorHeap(&cbvHeapDesc,
		IID_PPV_ARGS(&mCbvHeap)));
}

void D3D12HelloTriangle::LoadAssets()
{

	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
	featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

	m_device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData));
	

	CD3DX12_DESCRIPTOR_RANGE1 ranges[1];
	ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0);
	CD3DX12_ROOT_PARAMETER1 rootParameters[2];
	rootParameters[0].InitAsDescriptorTable(_countof(ranges), ranges, D3D12_SHADER_VISIBILITY_PIXEL);
	rootParameters[1].InitAsConstantBufferView(0);

	D3D12_STATIC_SAMPLER_DESC samplers[1] = {};
	samplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	samplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplers[0].MipLODBias = 0;
	samplers[0].MaxAnisotropy = 0;
	samplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplers[0].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	samplers[0].MinLOD = 0.f;
	samplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	samplers[0].ShaderRegister = 0;
	samplers[0].RegisterSpace = 0;
	samplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	// 创建一个空的root signature
	{
		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, _countof(samplers), samplers, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;
		ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
		m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));
	}

	// 创建一个pipeline state 包含编译和加载shader
	{
		ComPtr<ID3DBlob> vertexShader;
		ComPtr<ID3DBlob> pixelShader;
#if defined (_DEBUG)
		UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		UINT compileFlags = 0;
#endif

		std::wstring shaderFile = GetAssetFullPath(L"shaders.hlsl");


		vertexShader = CompileShader(shaderFile.c_str(), nullptr, "VSMain", "vs_5_0");
		pixelShader = CompileShader(shaderFile.c_str(), nullptr, "PSMain", "ps_5_0");
		//ThrowIfFailed(D3DCompileFromFile(shaderFile.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
		//ThrowIfFailed(D3DCompileFromFile(shaderFile.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));
		
		//定义vertex buff 输入布局方式
		D3D12_INPUT_ELEMENT_DESC inputElementDesc[] =
		{
			{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
			//{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,12,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

		};

		// 创建pso
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.InputLayout = { inputElementDesc ,_countof(inputElementDesc)};
		psoDesc.pRootSignature = m_rootSignature.Get();
		psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
		psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState.DepthEnable = FALSE;
		psoDesc.DepthStencilState.StencilEnable = FALSE;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count = 1;
		ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));
	}

	//  创建命令队列
	{
		ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), m_pipelineState.Get(), IID_PPV_ARGS(&m_commandList)));
		//ThrowIfFailed(m_commandList->Close());
	}
	D3D12_CPU_DESCRIPTOR_HANDLE srvHeapHandle = m_srvHeap->GetCPUDescriptorHandleForHeapStart();
	//创建Vertex Buffer
	{
		Vertex triangleVertices[] =
		{
			{ { 0.25f, 0.25f * m_aspectRatio, 0.0f }, { 1.f, 0.f } },
			{ { 0.25f, -0.25f * m_aspectRatio, 0.0f }, { 1.0f, 1.0f } },
			{ { -0.25f, -0.25f * m_aspectRatio, 0.0f }, { 0.0f, 1.0f } },
			{ { -0.25f, 0.25f * m_aspectRatio, 0.0f }, { 0.0f, 0.0f } },
		};

		const UINT vertexBufferSize = sizeof(triangleVertices);
		const UINT Count = vertexBufferSize / sizeof(Vertex);

		VertexBuffer = std::make_unique<UploadBuffer<Vertex>>(m_device.Get(), Count, false);
		//VertexBuffer->CopyDataList(0, Count, triangleVertices);
		for (int i = 0; i < Count; i++)
		{
			Vertex v = triangleVertices[i];
			VertexBuffer->CopyData(i, v);
		}

		//初始化vertexbuffer view
		m_vertexBufferView.BufferLocation = VertexBuffer->Resource()->GetGPUVirtualAddress();// m_vertexBuffer->GetGPUVirtualAddress();
		m_vertexBufferView.StrideInBytes = sizeof(Vertex);
		m_vertexBufferView.SizeInBytes = vertexBufferSize;

		DWORD iList[] =
		{
			0,1,2,
			0,2,3
		};
		const UINT IndexBufferSize = sizeof(iList);
		const UINT IndexCount = IndexBufferSize / sizeof(DWORD);


		IndexBuffer = std::make_unique<UploadBuffer<DWORD>>(m_device.Get(),IndexCount,false);
		//IndexBuffer->CopyDataList(0, IndexCount, iList);
		for (UINT i = 0; i < IndexCount; i++)
		{
			IndexBuffer->CopyData(i, iList[i]);
		}

		// 初始化顶点数据
		m_IndexBufferView.BufferLocation = IndexBuffer->Resource()->GetGPUVirtualAddress() ;// m_IndexBuffer->GetGPUVirtualAddress();
		m_IndexBufferView.SizeInBytes = IndexBufferSize;
		m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	

		


		// 创建常量缓冲视图
		//D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
		//cbvDesc.BufferLocation = m_CBVBuffer->GetGPUVirtualAddress();
		//cbvDesc.SizeInBytes = ConstantBufferSize;
		//m_device->CreateConstantBufferView(&cbvDesc, mCbvHeap->GetCPUDescriptorHandleForHeapStart());



		// 加载Texture
		TexMetadata MetaData;
		ScratchImage Image;
		LoadFromDDSFile(L"test.DDS", DDS_FLAGS_NONE, &MetaData,Image);

		{
			D3D12_RESOURCE_DESC bufferdc = CD3DX12_RESOURCE_DESC::Tex2D(MetaData.format, MetaData.width, MetaData.height);
			D3D12_RESOURCE_ALLOCATION_INFO info = m_device->GetResourceAllocationInfo(0, 1, &bufferdc);

			// 创建上传堆

			D3D12_RESOURCE_DESC Uploadbufferdc = CD3DX12_RESOURCE_DESC::Buffer(info.SizeInBytes);

			ID3D12Heap* UploadHeap;
			D3D12_HEAP_PROPERTIES UploadHeapProperties = { D3D12_HEAP_TYPE_UPLOAD };
			D3D12_HEAP_DESC UpLoadHeapDesc = { info.SizeInBytes,UploadHeapProperties };
			ThrowIfFailed(m_device->CreateHeap(&UpLoadHeapDesc, IID_PPV_ARGS(&UploadHeap)));
			ThrowIfFailed(m_device->CreatePlacedResource(
				UploadHeap,
				0,
				&Uploadbufferdc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&m_ITextureUpload)
			));


			ID3D12Heap* pUploadHeap;
			D3D12_HEAP_PROPERTIES DefaultHeapProperties = { D3D12_HEAP_TYPE_DEFAULT };
			D3D12_HEAP_DESC heapdesc = { info.SizeInBytes ,DefaultHeapProperties };
			ThrowIfFailed(m_device->CreateHeap(&heapdesc, IID_PPV_ARGS(&pUploadHeap)));
			// 创建缓冲
			ID3D12Resource* pTextureResource;
			ThrowIfFailed(m_device->CreatePlacedResource(
				pUploadHeap,
				0,
				&bufferdc,
				D3D12_RESOURCE_STATE_COPY_DEST,
				NULL,
				IID_PPV_ARGS(&pTextureResource)));


			UINT   nNumSubresources = 1u;  //我们只有一副图片，即子资源个数为1
			UINT   nTextureRowNum = 0u;
			UINT64 n64TextureRowSizes = 0u;
			UINT64 n64RequiredSize = 0u;

			D3D12_PLACED_SUBRESOURCE_FOOTPRINT FootPrint;
			m_device->GetCopyableFootprints(&bufferdc, 0, 1, 0, &FootPrint, &nTextureRowNum, &n64TextureRowSizes, &n64RequiredSize);;

			UINT8* pTexturePixel;
			CD3DX12_RANGE TextureRange(0, 0);
			ThrowIfFailed(m_ITextureUpload->Map(0, &TextureRange, reinterpret_cast<void**>(&pTexturePixel)));
			memcpy(pTexturePixel, Image.GetPixels(), Image.GetPixelsSize());
			m_ITextureUpload->Unmap(0, nullptr);

			CD3DX12_TEXTURE_COPY_LOCATION Dst(pTextureResource, 0);
			CD3DX12_TEXTURE_COPY_LOCATION Src(m_ITextureUpload.Get(), FootPrint);
			m_commandList->CopyTextureRegion(&Dst, 0, 0, 0, &Src, nullptr);
			

			//设置一个资源屏障，同步并确认复制操作完成
			//直接使用结构体然后调用的形式
			D3D12_RESOURCE_BARRIER stResBar = {};
			stResBar.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			stResBar.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			stResBar.Transition.pResource = pTextureResource;
			stResBar.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
			stResBar.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
			stResBar.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

			m_commandList->ResourceBarrier(1, &stResBar);

			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Format = bufferdc.Format;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = 1;

			m_device->CreateShaderResourceView(pTextureResource, &srvDesc, srvHeapHandle);
		}
	

		ThrowIfFailed(m_commandList->Close());
		ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
		m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
		
		

	}


	// 创建fence 异步并且等待assets 被GPU加载
	{
		ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
		m_fenceValue = 1;
		m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (m_fenceEvent == nullptr)
		{ 
			ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
		}
		WaitForPreviousFrame();
	}
}

void D3D12HelloTriangle::OnUpdate()
{
	ObjectConstants OC;

	FXMVECTOR EyePosition = { 0.f,0.f,-5.f };
	FXMVECTOR FocusPosition = { 0,0,0 };
	FXMVECTOR UpDirection = { 0.0f, 1.f, 0.0f };
	Camera->LookAt(EyePosition, FocusPosition, UpDirection);
	Camera->SetLens(20.f, .5f, 1.0f, 100.0f);
	Camera->SetPosition(1.f, 1.f, -5.f);
	Camera->UpdateViewMatrix();

	ConstantBuffer = std::make_unique<UploadBuffer<ObjectConstants>>(m_device.Get(), 1, true);
	XMStoreFloat4x4(&OC.WorldViewProj, Camera->GetViewAndProj());
	ConstantBuffer->CopyData(0, OC);
}

void D3D12HelloTriangle::OnRender()
{
	// 记录 所有命令
	PopulateCommandList();

	// 执行命令
	ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// 推送
	ThrowIfFailed(m_swapChain->Present(1, 0));
	WaitForPreviousFrame();
}

void D3D12HelloTriangle::OnDestroy()
{
	WaitForPreviousFrame();
	CloseHandle(m_fenceEvent);
}


void D3D12HelloTriangle::PopulateCommandList()
{
	ThrowIfFailed(m_commandAllocator->Reset());
	ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), m_pipelineState.Get()));

	//Set necessary state
	m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());


	ID3D12DescriptorHeap* ppHeaps[] = { m_srvHeap.Get() };
	m_commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	m_commandList->SetGraphicsRootDescriptorTable(0, m_srvHeap->GetGPUDescriptorHandleForHeapStart());
	m_commandList->SetGraphicsRootConstantBufferView(1, ConstantBuffer->Resource()->GetGPUVirtualAddress());
	//m_commandList->SetGraphicsRootConstantBufferView(1, m_CBVBuffer->GetGPUVirtualAddress());
	m_commandList->RSSetViewports(1, &m_viewport);
	m_commandList->RSSetScissorRects(1, &m_scissorRect);

	// 投射viewtarget
	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
	m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
	
	// Record commands
	const float clearColor[] = { 0.f,0.2f,.4f,1.f };
	m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	m_commandList->IASetIndexBuffer(&m_IndexBufferView);
	m_commandList->DrawIndexedInstanced(6, 1, 0, 0,0);

	// Indicate
	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	ThrowIfFailed(m_commandList->Close());
}

void D3D12HelloTriangle::WaitForPreviousFrame()
{
	const UINT64 fence = m_fenceValue;
	ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), fence));
	m_fenceValue++;
	if (m_fence->GetCompletedValue()<fence)
	{
		ThrowIfFailed(m_fence->SetEventOnCompletion(fence, m_fenceEvent));
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}
	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}

void D3D12HelloTriangle::BuildFrameResources()
{
	for (int i = 0; i < gNumFrameResources; i++)
	{
		mFrameResources.push_back(std::make_unique<struct FrameResource>(m_device.Get(),1,(UINT)mAllRitem.size()));
	}
}

void D3D12HelloTriangle::UpdateObjectCBs()
{
}

void D3D12HelloTriangle::UpdateMainPassCB()
{
}

