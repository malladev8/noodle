#pragma once
#include "Core/NoodleCore.h"
#include "Event.h"

class WindowResizeEvent : public Event
{
public:
	WindowResizeEvent(uint32 width, uint32 height, float timeStamp = 0.0f)
		: Event(timeStamp), m_Width(width), m_Height(height) {}

	uint32 GetWidth() const { return m_Width; }
	uint32 GetHeight() const { return m_Height; }

protected:
private:
	uint32 m_Width;
	uint32 m_Height;
};