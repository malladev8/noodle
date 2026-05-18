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
    if (argc < 5)
    {
        printf("Usage: Cooker <type> <asset root> <input> <output>\n");
        return -1;
    }

    const char* jsonType = argv[1];
    const char* assetName = argv[2];
    const char* assetRoot = argv[3];

    if (!ConvertJsonToBinary(sJsonTypeLookup.find(jsonType)->second, assetName, assetRoot))
    {
        printf("Cook failed.\n");
        return -1;
    }
    
    printf("Cook succeeded.\n");

    return 0;
}