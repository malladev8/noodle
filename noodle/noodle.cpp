#include "NoodlePch.h"
#include "NoodleAllocator.h"

#include<iostream>

int main(int argc, char* argv[])
{
	std::cout << "Hello World";

	StackAllocator stackAlloc(32);

	return 0;
}