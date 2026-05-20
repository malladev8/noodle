#pragma once
#include "../Core/NoodleCore.h"
#include <functional>
#include <unordered_map>
#include <typeindex>
#include <vector>
#include <queue>

// EventCallback is a delegate.
// This function returns void and takes an Event& (matching the template type) as a pointer.
template<typename TEvent>
using EventCallback = std::function<void(const TEvent&)>;

using EventId = uint32;

class EventManager
{
public:
	EventManager(const char* name, bool setAsGlobal = false);
	virtual ~EventManager() = default;

	template<typename TEvent>
	EventId Subscribe(EventCallback<TEvent> callback)
	{
		auto wrapper = [callback](const void* event)
			{
				callback(*static_cast<const TEvent*>(event));
			};

		m_Subscribers[typeid(TEvent)].push_back(
			{
				m_NextEventId,
				wrapper
			});

		return m_NextEventId++;
	}

	void Unsubscribe(EventId eventId);

	template<typename TEvent>
	void QueueEvent(const TEvent& event)
	{
		m_EventQueue.push(std::make_unique<QueuedEvent<TEvent>>(event));
	}

	void Update(float deltaSeconds);

	// Returns the Global Event Manager
	static const EventManager* GetGlobalEventManager();

protected:
	using InternalFn = std::function<void(const void*)>;

	class IQueuedEvent
	{
	public:
		virtual ~IQueuedEvent() = default;
		virtual void Dispatch(class EventManager& eventManager) = 0;
	protected:
	private:
	};

	template<typename TEvent>
	class QueuedEvent : public IQueuedEvent
	{
	public:
		QueuedEvent(const TEvent& event)
			: m_Event(event) {
		}

		void Dispatch(class EventManager& eventManager) override { eventManager.Broadcast(m_Event); }

	protected:
	private:
		TEvent m_Event;
	};

	struct Subscriber
	{
		EventId Id;
		InternalFn Callback;
	};

	template<typename TEvent>
	void Broadcast(const TEvent& event)
	{
		auto it = m_Subscribers.find(typeid(TEvent));
		if (it != m_Subscribers.end())
		{
			for (const Subscriber& subscriber : it->second)
			{
				subscriber.Callback(&event);
			}
		}
	}

private:
	std::unordered_map<std::type_index, std::vector<Subscriber>> m_Subscribers;
	std::queue<std::unique_ptr<IQueuedEvent>> m_EventQueue;
	const char* m_Name;

	EventId m_NextEventId;
};