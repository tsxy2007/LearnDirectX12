#include "stdafx.h"
#include "D3D12HelloTriangle.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nShowCmd)
{
	D3D12HelloTriangle sample(1280, 720, L"D3D12 Hello Triangle");
	return Win32Application::Run(&sample,hInstance,nShowCmd);
}

