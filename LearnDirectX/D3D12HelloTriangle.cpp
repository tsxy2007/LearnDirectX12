#include "stdafx.h"
#include "D3D12HelloTriangle.h"
#include "UploadBuffer.h"
#include "FrameResource.h"
#include "GeometryGenrator.h"
#include "D3DCamera.h"

D3D12HelloTriangle::D3D12HelloTriangle(UINT width, UINT height, std::wstring name) :
	DXSample(width,height,name),
	m_frameIndex(0),
	m_viewport(0.f,0.f,static_cast<float>(width),static_cast<float>(height)),
	m_scissorRect(0,0,static_cast<LONG>(width),static_cast<LONG>(height)),
	m_rtvDescriptorSize(0)
{
	mCamera = std::make_shared<D3DCamera>( );
}

D3D12HelloTriangle::~D3D12HelloTriangle()
{

}

void D3D12HelloTriangle::OnInit()
{
		LoadPipeline();
		BuildDescriptorHeaps();
		BuildConstantBuffers();
		BuildRootSignature();
		BuildShadersAndInputLayout();
		BuildPSO();
		BuildBoxGeometry();
		BuildCamera();
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

	CreateSwapChain(factory);

	ThrowIfFailed(factory->MakeWindowAssociation(Win32Application::GetHwnd(), DXGI_MWA_NO_ALT_ENTER));

	// 创建一个描述堆
	{
		CreateRtvAndDsvDescriptorHeaps();
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

	//  创建命令队列
	{
		ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), m_pipelineState.Get(), IID_PPV_ARGS(&m_commandList)));
	}
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

void D3D12HelloTriangle::CreateRtvAndDsvDescriptorHeaps()
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

void D3D12HelloTriangle::CreateSwapChain(ComPtr<IDXGIFactory6>& factory)
{
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
	ThrowIfFailed(swapChain.As(&m_swapChain));
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12HelloTriangle::CurrentBackBufferView() const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
}

void D3D12HelloTriangle::LoadAssets()
{	
	//执行命令
	{
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
		FlushCommandQueue();
	}
}

void D3D12HelloTriangle::OnUpdate(const GameTimer& gt)
{
	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
	float x = mRandius * sinf(mPhi) * cosf(mTheta);
	float z = mRandius * sinf(mPhi) * sinf(mTheta);
	float y = mRandius * cosf(mPhi);

	// Build the view matrix.
	XMMATRIX WorldMatrix = XMMatrixIdentity();
	ObjectConstants OC;
	XMStoreFloat4x4(&OC.WorldViewProj, WorldMatrix * mCamera->GetViewAndProj());
	mConstantBuffer->CopyData(0, OC);
}

void D3D12HelloTriangle::OnRender(const GameTimer& gt)
{
	// 记录 所有命令
	PopulateCommandList();

	// 执行命令
	ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// 推送
	ThrowIfFailed(m_swapChain->Present(1, 0));
	FlushCommandQueue();
}

void D3D12HelloTriangle::OnDestroy()
{
	FlushCommandQueue();
	CloseHandle(m_fenceEvent);
}

void D3D12HelloTriangle::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void D3D12HelloTriangle::OnMouseUp(WPARAM btnState, int x, int y)
{
	mDeltaX = 0.f;
	mDeltaY = 0.f;
	mDeltaZ = 0.f;
}

void D3D12HelloTriangle::OnMouseMove(WPARAM btnState, int x, int y)
{
	mDeltaX = 0.f;
	mDeltaY = 0.f;
	if ((btnState & MK_RBUTTON) != 0)
	{
		mDeltaX = 0.01f * (x - mLastMousePos.x);
		mDeltaY = 0.01f * (y - mLastMousePos.y);
		mCamera->RotationBy(mDeltaY, mDeltaX, 0);
	}
	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void D3D12HelloTriangle::OnKeyDown(UINT8 key)
{
	float CameraSpeed = 0.01f;
	switch (key)
	{
	case 'w':
	case 'W':
	{
		mCamera->MoveBy(mCamera->GetForwardVector()*CameraSpeed);
	}
	break;
	case 's':
	case 'S':
	{
		mCamera->MoveBy(mCamera->GetBackVector()*CameraSpeed);
	}
	break;
	case 'a':
	case 'A':
	{
		mCamera->MoveBy(mCamera->GetLeftVector()*CameraSpeed);
	}
	break;
	case 'd':
	case 'D':
	{
		mCamera->MoveBy(mCamera->GetRightVector()*CameraSpeed);
	}
	break;
	default:
		break;
	}
}

void D3D12HelloTriangle::OnKeyUp(UINT8 key)
{
	switch (key)
	{
	case 'w':
	case 'W':
	case 's':
	case 'S':
		mDeltaZ = 0.f;
		break;
	case 'a':
	case 'A':
	case 'd':
	case 'D':
		mDeltaX = 0.f;
		break;

	default:
		break;
	}
}


void D3D12HelloTriangle::PopulateCommandList()
{
	ThrowIfFailed(m_commandAllocator->Reset());
	ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), m_pipelineState.Get()));

	//Set necessary state
	m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());


	ID3D12DescriptorHeap* ppHeaps[] = { m_srvHeap.Get() };
	m_commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	//m_commandList->SetGraphicsRootDescriptorTable(0, m_srvHeap->GetGPUDescriptorHandleForHeapStart());
	m_commandList->SetGraphicsRootConstantBufferView(1, mConstantBuffer->Resource()->GetGPUVirtualAddress());
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
	//m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	//m_commandList->IASetIndexBuffer(&m_IndexBufferView);
	//m_commandList->DrawIndexedInstanced(6, 1, 0, 0,0);

	m_commandList->IASetVertexBuffers(0, 1, &mBoxGeo->VertexBufferView());
	m_commandList->IASetIndexBuffer(&mBoxGeo->IndexBufferView());
	m_commandList->DrawIndexedInstanced(mBoxGeo->DrawArgs["box"].IndexCount, 1, 0, 0, 0);

	// Indicate
	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	ThrowIfFailed(m_commandList->Close());
}

void D3D12HelloTriangle::FlushCommandQueue()
{
	m_fenceValue++;
	ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), m_fenceValue));
	if (m_fence->GetCompletedValue() < m_fenceValue)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent));
		WaitForSingleObject(m_fenceEvent, INFINITE);
		CloseHandle(eventHandle);
	}
}

void D3D12HelloTriangle::BuildFrameResources()
{
	for (int i = 0; i < gNumFrameResources; i++)
	{
		mFrameResources.push_back(std::make_unique<struct FrameResource>(m_device.Get(),1,(UINT)mAllRitem.size()));
	}
}

void D3D12HelloTriangle::BuildBoxGeometry()
{
	std::array<Vertex, 8> vertices =
	{
		Vertex({XMFLOAT3(-.25f,-.25f * m_aspectRatio,0.f),XMFLOAT4(Colors::White)}),
		Vertex({XMFLOAT3(-.25f,+.25f * m_aspectRatio,0.f),XMFLOAT4(Colors::Black)}),
		Vertex({XMFLOAT3(+.25f,+.25f * m_aspectRatio,0.f),XMFLOAT4(Colors::Red)}),
		Vertex({XMFLOAT3(+.25f,-.25f * m_aspectRatio,0.f),XMFLOAT4(Colors::Green)}),

		Vertex({XMFLOAT3(-.25f,-.25f * m_aspectRatio,1.f),XMFLOAT4(Colors::Blue)}),
		Vertex({XMFLOAT3(-.25f,+.25f * m_aspectRatio,1.f),XMFLOAT4(Colors::Yellow)}),
		Vertex({XMFLOAT3(+.25f,+.25f * m_aspectRatio,1.f),XMFLOAT4(Colors::Cyan)}),
		Vertex({XMFLOAT3(+.25f,-.25f * m_aspectRatio,1.f),XMFLOAT4(Colors::Magenta)}),
	};
	std::array<std::uint16_t, 36> indices =
	{
		0,1,2,
		0,2,3,

		4, 6, 5,
		4, 7, 6,

		4, 5, 1,
		4, 1, 0,

		3, 2, 6,
		3, 6, 7,

		1, 5, 6,
		1, 6, 2,

		4, 0, 3,
		4, 3, 7
	};
	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	mBoxGeo = std::make_unique<MeshGeometry>();
	mBoxGeo->Name = "boxGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &mBoxGeo->VertexBufferCPU));
	CopyMemory(mBoxGeo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &mBoxGeo->IndexBufferCPU));
	CopyMemory(mBoxGeo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	mBoxGeo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(m_device.Get(),
		m_commandList.Get(), vertices.data(), vbByteSize,
		mBoxGeo->VertexBufferUploader);

	mBoxGeo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(m_device.Get(),
		m_commandList.Get(), indices.data(), ibByteSize,
		mBoxGeo->IndexBufferUploader);

	mBoxGeo->VertexByteStride = sizeof(Vertex);
	mBoxGeo->VertexBufferByteSize = vbByteSize;
	mBoxGeo->IndexFormat = DXGI_FORMAT_R16_UINT;
	mBoxGeo->IndexBufferByteSize = ibByteSize;
	
	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	mBoxGeo->DrawArgs["box"] = submesh;
}

void D3D12HelloTriangle::BuildCamera()
{
	FXMVECTOR EyePosition = { 0.f, 0.f, -2.f };
	FXMVECTOR FocusPosition = { 0, 0, -1.0f };
	FXMVECTOR UpDirection = { 0.0f, 1.f, 0.0f };
	mCamera->SetPosition(EyePosition);
	mCamera->SetProjectionValues(90.f, m_width / m_height, 0.1f, 1000.0f);
	mCamera->SetLookAtPos(XMFLOAT3(0.f, 0.f, 0.f));
}

void D3D12HelloTriangle::BuildConstantBuffers()
{
	mConstantBuffer = std::make_unique<UploadBuffer<ObjectConstants>>(m_device.Get(), 1, true);
	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));

	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = mConstantBuffer->Resource()->GetGPUVirtualAddress();
	int boxCBufIndex = 0;
	cbAddress += boxCBufIndex * objCBByteSize;

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	m_device->CreateConstantBufferView(&cbvDesc, mCbvHeap->GetCPUDescriptorHandleForHeapStart());
}

void D3D12HelloTriangle::BuildRootSignature()
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

}

void D3D12HelloTriangle::BuildShadersAndInputLayout()
{
	// 创建一个pipeline state 包含编译和加载shader
	std::wstring shaderFile = GetAssetFullPath(L"shaders.hlsl");

	mVertexShader = d3dUtil::CompileShader(shaderFile.c_str(), nullptr, "VSMain", "vs_5_0");
	mPixelShader = d3dUtil::CompileShader(shaderFile.c_str(), nullptr, "PSMain", "ps_5_0");

	//定义vertex buff 输入布局方式
	mInputLayout =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,12,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		//{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

}

void D3D12HelloTriangle::BuildPSO()
{
	{
		// 创建pso
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.InputLayout = { mInputLayout.data() ,(UINT)(mInputLayout.size()) };
		psoDesc.pRootSignature = m_rootSignature.Get();
		psoDesc.VS = CD3DX12_SHADER_BYTECODE(mVertexShader.Get());
		psoDesc.PS = CD3DX12_SHADER_BYTECODE(mPixelShader.Get());
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
}

void D3D12HelloTriangle::BuildShapeGeometry()
{
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData box = geoGen.CreateBox(1.5f, 0.5f, 1.5f, 3);
	GeometryGenerator::MeshData sphere = geoGen.CreateSphere(20.f, 30.f, 60);
}

void D3D12HelloTriangle::UpdateObjectCBs()
{
	auto currentObjectCB = mCurrentFrameResource->ObjectCB.get();
	for (auto& e : mAllRitem)
	{
		if (e->NumFrameDirty > 0)
		{
			XMMATRIX world = XMLoadFloat4x4(&e->World);
			ObjectConstants objConstants;
			XMStoreFloat4x4(&objConstants.WorldViewProj, XMMatrixTranspose(world));
			currentObjectCB->CopyData(e->ObjCBIndex, objConstants);
			e->NumFrameDirty--;
		}
	}
}

void D3D12HelloTriangle::UpdateMainPassCB()
{ 

}

