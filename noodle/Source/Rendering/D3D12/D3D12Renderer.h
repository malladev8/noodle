#pragma once
#include "Core/NoodleCore.h"

#if defined(WINDOWS)
#include "Rendering/IRenderer.h"
#include "D3D12ResourceTypes.h"

constexpr uint32 BufferCount = 2;

class D3D12Renderer : public IRenderer
{
public:
	D3D12Renderer();
	virtual void Initialize(void* hwnd, NoodleWindowDesc& windowDesc) override;
	virtual void Shutdown() override;

	virtual void BeginFrame() override;
	virtual void RenderFrame() override;
	virtual void EndFrame() override;
	virtual void Present() override;

	virtual void SubmitSprite(const struct SpriteRenderCommand& cmd) override;

	virtual void CreateTextureResources(struct Texture& texture, const std::vector<uint8>& pixels) override;
	virtual void CreateShaderResources(std::shared_ptr<struct Shader>& outShader, const struct CookedShaderData& shaderData) override;

protected:

private:
	D3D12_VIEWPORT m_Viewport;
	D3D12_RECT m_ScissorRect;

	ComPtr<ID3D12Device> m_Device;
	ComPtr<IDXGISwapChain3> m_SwapChain;
	ComPtr<ID3D12CommandQueue> m_CommandQueue;
	ComPtr<ID3D12GraphicsCommandList> m_CommandList;
	ComPtr<ID3D12Fence> m_Fence;
	ComPtr<IDXGIFactory6> m_Factory;
	ComPtr<IDXGIAdapter1> m_Adapter;
	ComPtr<ID3D12DescriptorHeap> m_RtvHeap;
	ComPtr<ID3D12Resource> m_RenderTargets[BufferCount];
	ComPtr<ID3D12CommandAllocator> m_CommandAllocators[BufferCount];

	ComPtr<ID3D12DescriptorHeap> m_SrvHeap;

	HANDLE m_FenceEvent;
	uint64 m_FenceValue[BufferCount];
	uint64 m_CurrentFenceValue;
	D3D12_CPU_DESCRIPTOR_HANDLE m_RtvHandles[BufferCount];
	uint32 m_RtvDescriptorSize;
	uint32 m_CurrentFrameIndex;
	uint32 m_WindowResizeEventHandle;

	uint32 m_SrvDescriptorSize;
	uint32 m_CurrentSrvIndex = 0;

	void Resize(const class WindowResizeEvent& event);
	void EnableDebugLayer();
	void WaitForGpu();
	void CreateRenderTargetViews();
	void UpdateViewport(uint32 width, uint32 height);
	DXGI_FORMAT GetDxgiFormat(eTextureFormat format);
};
#endif