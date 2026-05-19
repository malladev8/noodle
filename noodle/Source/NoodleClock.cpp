#include "NoodlePch.h"
#include "NoodleClock.h"

Clock::Clock() : m_DeltaSeconds(0.0f), m_TotalSeconds(0.0f)
{
}

void Clock::Reset()
{
	m_DeltaSeconds = 0.0f;
	m_TotalSeconds = 0.0f;
	m_LastTime = std::chrono::high_resolution_clock::now();
}

void Clock::Tick()
{
	std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> deltaTime = currentTime - m_LastTime;
	m_DeltaSeconds = deltaTime.count();
	m_TotalSeconds += m_DeltaSeconds;
	m_LastTime = currentTime;
}
