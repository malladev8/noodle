#pragma once
#include "NoodleDefines.h"

class LinearAllocator
{
public:
	LinearAllocator(uint32 size);
	virtual ~LinearAllocator();
	void* Alloc(uint32 size);
	void Clear();
protected:
	uint32 m_Marker = 0;
private:
	void* m_Address = nullptr;
	uint32 m_Size = 0;
};

class StackAllocator : public LinearAllocator
{
public:
	StackAllocator(uint32 size) : LinearAllocator(size) {}
	uint32 GetMarker() const { return m_Marker; }
	void FreeToMarker(uint32 marker);
};