#pragma once
#include "Core/NoodleCore.h"

class LinearAllocator
{
public:
	LinearAllocator(size_t size);
	virtual ~LinearAllocator();
	void* Alloc(size_t size);
	void Clear();
protected:
	size_t m_Marker = 0;
private:
	void* m_Address = nullptr;
	size_t m_Size = 0;
};

class StackAllocator : public LinearAllocator
{
public:
	StackAllocator(size_t size) : LinearAllocator(size) {}
	size_t GetMarker() const { return m_Marker; }
	void FreeToMarker(size_t marker);
};