#pragma once
#include "Core/NoodleCore.h"

#if defined(WINDOWS)
#include "Rendering/IRenderer.h"

class D3D12Renderer : public IRenderer
{
public:
	virtual void Initialize(void* hwnd) override;
	virtual void Shutdown() override;

	virtual void BeginFrame() override;
	virtual void RenderFrame() override;
	virtual void EndFrame() override;
	virtual void Present() override;

	virtual void SubmitSprite(const struct SpriteRenderCommand& cmd) override;

protected:

private:
	uint32 m_WindowResizeEventHandle = 0;

	void Resize(const class WindowResizeEvent& event);
};
#endif