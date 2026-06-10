#pragma once
#include "Core/NoodleCore.h"
#include "Resource/ResourceTypes.h"
#include "D3D12Common.h"

struct D3D12TextureResource : public ITextureResource
{
	ComPtr<ID3D12Resource> textureResource;
	ComPtr<ID3D12Resource> uploadResource;
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle{};
	D3D12_CPU_DESCRIPTOR_HANDLE srvCpuHandle{};
};

struct D3D12Shader : public Shader
{
	ComPtr<ID3D12PipelineState> pso;
	ComPtr<ID3D12RootSignature> rootSignature;
};