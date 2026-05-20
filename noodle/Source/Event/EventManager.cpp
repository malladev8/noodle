#include "../NoodlePch.h"
#include "EventManager.h"
#include "Event.h"

static std::unique_ptr<EventManager> sGlobalEventManager = nullptr;

EventManager::EventManager(const char* name, bool setAsGlobal)
	: m_Name(name), m_NextEventId(0)
{
	if (setAsGlobal)
	{
		N_ASSERT(sGlobalEventManager == nullptr,  "Global Event Manager already exists.");
		sGlobalEventManager = std::unique_ptr<EventManager>(this);
	}
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

const EventManager* EventManager::GetGlobalEventManager()
{
	return sGlobalEventManager.get();
}
