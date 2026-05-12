#include "../Core/NoodleCore.h"
#include "JSONCooker.h"
#include <cstdio>

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        printf("Usage: Cooker <input> <output>\n");
        return -1;
    }

    const char* input = argv[1];
    const char* output = argv[2];

    if (!ConvertActorJsonToBinary(input, output))
    {
        printf("Cook failed.\n");
        return -1;
    }

    printf("Cook succeeded.\n");

    return 0;
}