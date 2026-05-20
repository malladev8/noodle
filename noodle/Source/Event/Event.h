#pragma once
#include "../Core/NoodleCore.h"

class Event
{
public:
	Event(float timeStamp = 0.0f)
		: m_TimeStamp(timeStamp) {}

	virtual ~Event() {}

	float GetTimeStamp() const { return m_TimeStamp; }

protected:
private:
	const float m_TimeStamp;
};
