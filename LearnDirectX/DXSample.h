#pragma once

#include "DXSampleHelper.h"
#include "Win32Application.h"

class DXSample
{
public:
	DXSample(UINT width,UINT height,std::wstring name);
	virtual ~DXSample();

	virtual void OnInit() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnRender() = 0;
	virtual void OnDestroy() = 0;

	virtual void OnKeyDown(UINT8  key){}
	virtual void OnKeyUp(UINT8 key){}

	const WCHAR* GetTitle() const { return m_title.c_str(); }

	void ParseCommandLineArgs(_In_reads_(argc) WCHAR* argv[], int argc);

	UINT GetWidth() { return m_width; }
	UINT GetHeight() { return m_height; }
protected:
	std::wstring GetAssetFullPath(LPCWSTR assetName);
	void GetHardwareAdapter(_In_ IDXGIFactory6* pFactory, _Outptr_result_maybenull_ IDXGIAdapter4** ppAdapter);
	void SetCustomWindowText(LPCWSTR text);
private:
	std::wstring m_title;
	std::wstring m_assetsPath;
protected:
	UINT m_width;
	UINT m_height;
	float m_aspectRatio;
	bool m_useWarpDevice;
};
