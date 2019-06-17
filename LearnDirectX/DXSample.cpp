
#include "stdafx.h"
#include "DXSample.h"

using namespace Microsoft::WRL;

DXSample::DXSample(UINT width, UINT height, std::wstring name):
	m_width(width),
	m_height(height),
	m_title(name),
	m_useWarpDevice(false)
{
	WCHAR assetsPath[512];
	GetAsstsPath(assetsPath, _countof(assetsPath));
	m_assetsPath = assetsPath;
	m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
}

DXSample::~DXSample()
{
}

std::wstring DXSample::GetAssetFullPath(LPCWSTR assetName)
{
	return m_assetsPath+assetName;
}

_Use_decl_annotations_
void DXSample::GetHardwareAdapter(IDXGIFactory6 * pFactory, IDXGIAdapter4 ** ppAdapter)
{
	ComPtr<IDXGIAdapter4> adapter;
	*ppAdapter = nullptr;
	//枚举上下文
	for (UINT adapterIndex = 0;DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapterByGpuPreference(adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,IID_PPV_ARGS(&adapter));++adapterIndex)
	{
		HRESULT hr;
		DXGI_ADAPTER_DESC3 desc;
		adapter->GetDesc3(&desc);
		if (desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)
		{
			continue;
		}
		hr = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr);
		if (SUCCEEDED(hr))
		{
			break;
		}
		*ppAdapter = adapter.Detach();
	}
}

void DXSample::SetCustomWindowText(LPCWSTR text)
{
	std::wstring windowText = m_title + L":" + text;
	SetWindowText(Win32Application::GetHwnd(), windowText.c_str());
}

_Use_decl_annotations_
void DXSample::ParseCommandLineArgs(_In_reads_(argc) WCHAR* argv[], int argc)
{
	for (int i = 1;i<argc;++i)
	{
		if (_wcsnicmp(argv[i],L"-wrap",wcslen(argv[i])) == 0 ||
			_wcsnicmp(argv[i],L"/wrap",wcslen(argv[i])) == 0 )
		{
			m_useWarpDevice = true;
			m_title = m_title + L"(WARP)";
		}
	}
}
