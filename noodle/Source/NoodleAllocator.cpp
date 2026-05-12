#include "NoodlePch.h"
#include "NoodleAllocator.h"

#include <cstdlib>

LinearAllocator::~LinearAllocator()
{
	std::free(m_Address);
	m_Address = nullptr;
	m_Size = 0;
	m_Marker = 0;
}

bool LinearAllocator::Init(size_t size)
{
	m_Size = size;
	m_Address = std::malloc(m_Size);
	return m_Address;
}

void* LinearAllocator::Alloc(size_t size)
{
	N_ASSERT(m_Size > 0, "Allocator is uninitialized. Call Init() before using.");
	N_ASSERT(size + m_Marker <= m_Size, "Insufficient space remaining.");
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
	N_ASSERT(marker <= m_Marker, "New marker exceeds current marker.");
	m_Marker = marker;
}

const static size_t CHUNK_HEADER_SIZE = (sizeof(unsigned char*));

MemoryPool::~MemoryPool()
{
	Destroy();
}

bool MemoryPool::Init(size_t chunkSize, uint32 numChunks)
{
	m_ChunkSize = chunkSize;
	m_NumChunks = numChunks;
	return GrowMemoryArray();
}

void MemoryPool::Destroy()
{
	unsigned char* curr = m_Head;
	while (curr != nullptr)
	{
		unsigned char* next = GetNext(curr);
		std::free(curr);
		curr = next;
	}

	if (m_RawMemoryArray)
	{
		std::free(m_RawMemoryArray);
	}
	Reset();
}

void* MemoryPool::Alloc()
{
	// If we’re out of memory chunks, grow the pool. This is very expensive.
	if (!m_Head)
	{
		if (!m_AllowResize)
		{
			return nullptr;
		}

		if (!GrowMemoryArray())
		{
			return nullptr;
		}
	}
	unsigned char* ret = m_Head;
	m_Head = GetNext(m_Head);

	// Return a pointer to the data section only
	return (ret + CHUNK_HEADER_SIZE);
}

void MemoryPool::Free(void* memory)
{
	if (memory != nullptr)
	{
		unsigned char* block = ((unsigned char*)memory) - CHUNK_HEADER_SIZE;
		SetNext(block, m_Head);
		m_Head = block;
	}
}

void MemoryPool::Reset()
{
	m_RawMemoryArray = nullptr;
	m_Head = nullptr;
	m_ChunkSize = 0;
	m_NumChunks = 0;
	m_ArraySize = 0;
	m_AllowResize = false;
}

bool MemoryPool::GrowMemoryArray()
{
	size_t allocationSize = sizeof(unsigned char*) * (m_ArraySize + 1);
	unsigned char** newMemArrary = (unsigned char**)std::malloc(allocationSize);

	if (!newMemArrary)
	{
		return false;
	}

	for (uint32 i = 0; i < m_ArraySize; ++i)
	{
		newMemArrary[i] = m_RawMemoryArray[i];
	}

	// Indexing m_ArraySize here is safe because we haven’t incremented it yet to reflect the new size
	newMemArrary[m_ArraySize] = AllocateNewMemoryBlock();

	// Attach the block to the end of the current memory list
	if (m_Head)
	{
		unsigned char* curr = m_Head;
		unsigned char* next = GetNext(m_Head);
		while (next)
		{
			curr = next;
			next = GetNext(curr);
		}
		SetNext(curr, newMemArrary[m_ArraySize]);
	}
	else
	{
		m_Head = newMemArrary[m_ArraySize];
	}

	if (m_RawMemoryArray)
	{
		std::free(m_RawMemoryArray);
	}
	m_RawMemoryArray = newMemArrary;
	++m_ArraySize;

	return true;
}

unsigned char* MemoryPool::AllocateNewMemoryBlock()
{
	size_t blockSize = m_ChunkSize + CHUNK_HEADER_SIZE;
	size_t trueSize = blockSize * m_NumChunks;
	unsigned char* newMem = (unsigned char*)std::malloc(trueSize);

	if (!newMem)
	{
		return nullptr;
	}

	unsigned char* end = newMem + trueSize;
	unsigned char* curr = newMem;
	while (curr < end)
	{
		unsigned char* next = curr + blockSize;
		unsigned char** chunkHeader = (unsigned char**)curr;
		chunkHeader[0] = (next < end ? next : nullptr);
		curr += blockSize;
	}

	return newMem;
}

unsigned char* MemoryPool::GetNext(unsigned char* block)
{
	return ((unsigned char**)block)[0];
}

void MemoryPool::SetNext(unsigned char* blockToChange, unsigned char* newNext)
{
	((unsigned char**)blockToChange)[0] = newNext;
}
