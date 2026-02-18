#include "NoodlePch.h"
#include "NoodleAllocator.h"

#include <cstdlib>

StackAllocator::StackAllocator(uint32 size)
	:m_StackSize(size)
{
	m_StackAddress = std::malloc(m_StackSize);
}

StackAllocator::~StackAllocator()
{
	Destroy();
}

void StackAllocator::Destroy()
{
	std::free(m_StackAddress);
	m_StackAddress = nullptr;
	m_StackSize = 0;
	m_Marker = 0;
}

void* StackAllocator::Alloc(uint32 size)
{
	NASSERT(size + m_Marker <= m_StackSize, "Insufficient stack space remaining.");
	char* chunk = static_cast<char*>(m_StackAddress) + m_Marker;
	m_Marker += size;
	return static_cast<void*>(chunk);
}

void StackAllocator::Free(uint32 size)
{
	NASSERT(size <= m_Marker, "Free call will underflow Stack Allocator");
	m_Marker -= size;
}

void StackAllocator::Clear()
{
	m_Marker = 0;
}
