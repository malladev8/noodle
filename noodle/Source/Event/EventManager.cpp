#include "NoodlePch.h"
#include "EventManager.h"
#include "Event.h"

EventManager::EventManager(const char* name)
	: m_Name(name), m_NextEventId(0)
{
}

void EventManager::Unsubscribe(EventId eventId)
{
	for (auto& [type, subscribers] : m_Subscribers)
	{
		subscribers.erase(
			std::remove_if(subscribers.begin(), subscribers.end(),
				[eventId](const Subscriber& subscriber)
				{
					return subscriber.Id == eventId;
				}),
			subscribers.end());
	}
}

void EventManager::Update(float deltaSeconds)
{
	while (!m_EventQueue.empty())
	{
		m_EventQueue.front()->Dispatch(*this);
		m_EventQueue.pop();
	}
}