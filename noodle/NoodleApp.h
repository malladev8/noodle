#pragma once
#include "Core/NoodleCore.h"
#include "NoodleAllocator.h"

const size_t APP_SIZE = 1_GB;

class NoodleApp
{
public:
	NoodleApp();
	virtual ~NoodleApp();

	void Run();

protected:
private:
	LinearAllocator ApplicationAllocator;
};