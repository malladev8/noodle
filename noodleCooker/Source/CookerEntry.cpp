#include "JSONCooker.h"
#include <cstdio>
#include <unordered_map>
#include <string>

static const std::unordered_map<std::string, eJsonType> sJsonTypeLookup
{
    {"actor", eJsonType::ACTOR},
    {"scene", eJsonType::SCENE}
};

int main(int argc, char** argv)
{
    if (argc < 4)
    {
        printf("Usage: Cooker <type> <application root> <asset name>\n");
        return -1;
    }

    // Example args
    //const char* jsonType = "scene";
    //const char* applicationRoot = "B:/ScarlettDev/RA/RedAngel/";
    //const char* assetName = "TestScene";

    const char* jsonType = argv[1];
    const char* applicationRoot = argv[2];
    const char* assetName = argv[3];

    if (!sJsonTypeLookup.contains(jsonType))
    {
        printf("Invalid json type: %s\n", jsonType);
        return -1;
    }

    if (!ConvertJsonToBinary(sJsonTypeLookup.find(jsonType)->second, applicationRoot, assetName))
    {
        printf("Cook failed.\n");
        return -1;
    }
    
    printf("Cook succeeded.\n");

    return 0;
}