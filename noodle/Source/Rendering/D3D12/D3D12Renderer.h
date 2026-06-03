#pragma once
#include "Core/NoodleCore.h"

#if defined(WINDOWS)
#include "Rendering/IRenderer.h"
#include "D3D12Common.h"

class D3D12Renderer : public IRenderer
{
public:
	D3D12Renderer();
	virtual void Initialize(void* hwnd) override;
	virtual void Shutdown() override;

	virtual void BeginFrame() override;
	virtual void RenderFrame() override;
	virtual void EndFrame() override;
	virtual void Present() override;

	virtual void SubmitSprite(const struct SpriteRenderCommand& cmd) override;

protected:

private:
	ComPtr<ID3D12Device> m_Device;
	ComPtr<IDXGISwapChain4> m_SwapChain;
	ComPtr<ID3D12CommandQueue> m_CommandQueue;
	ComPtr<ID3D12GraphicsCommandList> m_CommandList;
	ComPtr<ID3D12Fence> m_Fence;

	ComPtr<IDXGIFactory6> m_Factory;
	ComPtr<IDXGIAdapter1> m_Adapter;

	HANDLE m_FenceEvent;
	uint64 m_FenceValue;
	uint32 m_WindowResizeEventHandle;

	void Resize(const class WindowResizeEvent& event);
	void EnableDebugLayer();
};
#endif