#pragma once
#include "Core/NoodleCore.h"

class IRenderer
{
public:
	virtual ~IRenderer() = default;

	virtual void Initialize(void* hwnd, NoodleWindowDesc& windowDesc) = 0;
	virtual void Shutdown() = 0;

	virtual void BeginFrame() = 0;
	virtual void RenderFrame() = 0;
	virtual void EndFrame() = 0;
	virtual void Present() = 0;

	virtual void SubmitSprite(const struct SpriteRenderCommand& cmd) = 0;

protected:
private:
};