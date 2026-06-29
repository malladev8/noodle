#pragma once
#include"Core/NoodleCore.h"

class WindowResizeEvent;

struct WindowDesc
{
	uint32 width = 0;
	uint32 height = 0;
	std::string title = "";
};

class Window
{
public:
	bool Initialize(const WindowDesc& desc, struct EngineContext& engineContext);
	void Shutdown(struct EngineContext& engineContext);

	uint32 GetWidth() const { return m_Width; }
	uint32 GetHeight() const { return m_Height; }
	float32 GetAspectRatio() const;

	void OnWindowResize(const WindowResizeEvent& event);

protected:
private:
	std::string m_Title = "";
	uint32 m_Width = 0;
	uint32 m_Height = 0;
	uint32 m_WindowResizeEventHandle = 0;
};