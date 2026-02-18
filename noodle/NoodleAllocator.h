#pragma once
#include "NoodleDefines.h"

class StackAllocator
{
public:
	StackAllocator(uint32 size);
	~StackAllocator();
	void Destroy();
	void* Alloc(uint32 size);
	void Free(uint32 size);
	void Clear();
protected:
private:
	void* m_StackAddress = nullptr;
	uint32 m_Marker = 0;
	uint32 m_StackSize = 0;
};