#include "../Core/NoodleCore.h"
#include "JSONCooker.h"
#include <cstdio>
#include <unordered_map>

static const std::unordered_map<const char*, eJsonType> sJsonTypeLookup
{
    {"actor", eJsonType::ACTOR},
    {"scene", eJsonType::SCENE}
};

int main(int argc, char** argv)
{
    if (argc < 4)
    {
        printf("Usage: Cooker <input> <output>\n");
        return -1;
    }

    const char* jsonType = argv[1];
    const char* input = argv[2];
    const char* output = argv[3];

    if (!ConvertJsonToBinary(input, output, sJsonTypeLookup.find(jsonType)->second))
    {
        printf("Cook failed.\n");
        return -1;
    }
    
    printf("Cook succeeded.\n");

    return 0;
}