#pragma once
#include "Core/NoodleCore.h"

class LinearAllocator
{
public:
	LinearAllocator() {}
	virtual ~LinearAllocator();
	bool Init(size_t size);
	void* Alloc(size_t size);
	void Clear();
protected:
	size_t m_Marker = 0;
private:
	void* m_Address = nullptr;
	size_t m_Size = 0;

	LinearAllocator(const LinearAllocator& linearAllocator) {}
};

class StackAllocator : public LinearAllocator
{
public:
	StackAllocator() {}
	size_t GetMarker() const { return m_Marker; }
	void FreeToMarker(size_t marker);

private:
	StackAllocator(const StackAllocator& stackAllocator) {}
};

// Best way to use this MemoryPool is to figure out which objects will be
// constructed and destroyed extremely often and make them use this class.
// Reccommend overriding new and delete operators for that class so that
// they call into the memory pool for allocation and deallocation.
class MemoryPool
{
public:
	MemoryPool() {}
	~MemoryPool();
	bool Init(size_t chunkSize, uint32 numChunks);
	void Destroy();
	void* Alloc();
	void Free(void* memory);
	size_t GetChunkSize() const { return m_ChunkSize; }
	void SetAllowResize(bool allowResize) { m_AllowResize = allowResize; }

private:
	unsigned char** m_RawMemoryArray = nullptr;
	unsigned char* m_Head = nullptr;
	uint32 m_ChunkSize = 0; // Size of each chunk
	uint32 m_NumChunks = 0; // Number of chunks per array
	uint32 m_ArraySize = 0; // Number of elements in the memory array
	bool m_AllowResize = false;

	void Reset();
	bool GrowMemoryArray();
	unsigned char* AllocateNewMemoryBlock();
	unsigned char* GetNext(unsigned char* block);
	void SetNext(unsigned char* blockToChange, unsigned char* newNext);
	MemoryPool(const MemoryPool& memPool) {}
};