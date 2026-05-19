#pragma once
#include "Core/NoodleCore.h"
#include <chrono>

class Clock
{
public:
	Clock();
	void Reset();
	void Tick();
	float GetDeltaSeconds() const { return m_DeltaSeconds; }
	float GetTotalSeconds() const { return m_TotalSeconds; }

protected:
private:
	std::chrono::high_resolution_clock::time_point m_LastTime;
	float m_DeltaSeconds;
	float m_TotalSeconds;
};
