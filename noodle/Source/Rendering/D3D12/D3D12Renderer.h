#pragma once
#include "Core/NoodleCore.h"

#if defined(WINDOWS)
#include "Rendering/IRenderer.h"
#include "D3D12ResourceTypes.h"

constexpr uint32 BufferCount = 2;
class D3D12SpriteRenderer;

enum class eRootParamIndex : uint32
{
	FRAME_CB = 0,
	OBJECT_CB,
	SRV,
	COUNT
};

class D3D12Renderer : public IRenderer
{
public:
	D3D12Renderer();
	virtual void Initialize(void* hwnd, class Window& windowDesc) override;
	virtual void Shutdown() override;

	virtual void BeginFrame(const struct CameraData& camData) override;
	virtual void RenderFrame() override;
	virtual void EndFrame() override;
	virtual void Present() override;

	virtual void SubmitSprite(const struct SpriteRenderCommand& cmd) override;

	virtual void CreateTextureResources(struct Texture& texture, const std::vector<uint8>& pixels) override;
	virtual void CreateShaderResources(std::shared_ptr<struct Shader>& outShader, const struct CookedShaderData& shaderData) override;

	ID3D12Device* GetDevice() const { return m_Device.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() const { return m_CommandList.Get(); }
	ID3D12DescriptorHeap* GetSrvHeap() const { return m_SrvHeap.Get(); }
	ID3D12Resource* GetFrameConstantBuffer() const { return m_FrameConstantBuffer.Get(); }

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
	ComPtr<ID3D12Resource> m_FrameConstantBuffer;
	uint8* m_MappedFrameCB;
	D3D12_GPU_VIRTUAL_ADDRESS m_FrameCBGpuAddress;

	ComPtr<ID3D12CommandAllocator>m_UploadAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_UploadCommandList;
	ComPtr<ID3D12Fence>m_UploadFence;

	std::unique_ptr<D3D12SpriteRenderer> m_SpriteRenderer;

	HANDLE m_FenceEvent;
	HANDLE m_UploadFenceEvent;
	uint64 m_FenceValue[BufferCount];
	uint64 m_CurrentFenceValue;
	uint64 m_UploadFenceValue;
	D3D12_CPU_DESCRIPTOR_HANDLE m_RtvHandles[BufferCount];
	uint32 m_RtvDescriptorSize;
	uint32 m_CurrentFrameIndex;
	uint32 m_WindowResizeEventHandle;

	uint32 m_SrvDescriptorSize;
	uint32 m_CurrentSrvIndex;

	void Resize(const class WindowResizeEvent& event);
	void EnableDebugLayer();
	void WaitForGpu();
	void WaitForGpuUpload();
	void CreateRenderTargetViews();
	void UpdateViewport(uint32 width, uint32 height);
	DXGI_FORMAT GetDxgiFormat(eTextureFormat format);
};
#endif