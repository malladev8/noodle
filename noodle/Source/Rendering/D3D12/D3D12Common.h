#pragma once
#include "Core/NoodleCore.h"

#if defined(WINDOWS)
#include <dxgi1_6.h>
#include <d3dcompiler.h>

#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include "ThirdParty/DirectX-Headers/include/dirextx/d3dx12.h"
#endif
