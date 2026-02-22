#include "NoodlePch.h"
#include "NoodleAllocator.h"

#include <cstdlib>

LinearAllocator::LinearAllocator(size_t size)
	:m_Size(size)
{
	m_Address = std::malloc(m_Size);
}

LinearAllocator::~LinearAllocator()
{
	if (m_Address)
	{
		std::free(m_Address);
		m_Address = nullptr;
		m_Size = 0;
		m_Marker = 0;
	}
}

void* LinearAllocator::Alloc(size_t size)
{
	NASSERT(size + m_Marker <= m_Size, "Insufficient space remaining.");
	char* chunk = static_cast<char*>(m_Address) + m_Marker;
	m_Marker += size;
	return static_cast<void*>(chunk);
}

void LinearAllocator::Clear()
{
	m_Marker = 0;
}

void StackAllocator::FreeToMarker(size_t marker)
{
	NASSERT(marker <= m_Marker, "New marker exceeds current marker.");
	m_Marker = marker;
}
