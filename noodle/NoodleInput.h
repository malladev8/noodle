#pragma once
#include "Core/NoodleCore.h"

class NoodleInput
{
public:
	NoodleInput() = default;
	~NoodleInput() = default;

	void Update();
	bool InputPressed(uint32 key);
	bool InputReleased(uint32 key);
protected:
private:
};