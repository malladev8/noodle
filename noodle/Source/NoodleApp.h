#pragma once
#include "Core/NoodleCore.h"
#include "NoodleAllocator.h"

class NoodleApp
{
public:
	NoodleApp();
	virtual ~NoodleApp();

	void Init();
	void Run();

protected:
private:
};