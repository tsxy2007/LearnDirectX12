#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dxgi1_6.h>
//#include <dxgi1_3.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "d3dx12.h"
#include <string>
#include <wrl.h>
#include <shellapi.h>
#include "d3d12.h"
#include <stdexcept>

#define GRS_UPPER(A,B) ((UINT)(((A)+((B)-1))&~(B - 1)))