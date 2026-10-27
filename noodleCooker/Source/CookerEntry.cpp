#include "JSONCooker.h"
#include <cstdio>
#include <unordered_map>
#include <string>

static const std::unordered_map<std::string, eJsonType> sJsonTypeLookup
{
    {"scene", eJsonType::SCENE},
    {"shader", eJsonType::SHADER},
};

int main(int argc, char** argv)
{
    if (argc < 5)
    {
        printf("Usage: Cooker <type> <asset root> <cooked root> <asset name>\n");
        return -1;
    }

    // Example args
    //const char* jsonType = "scene";
    //const char* applicationRoot = "B:/ScarlettDev/RA/RedAngel/";
    //const char* assetName = "Scene/TestScene";

    const char* jsonType = argv[1];
    const char* assetRoot = argv[2];
    const char* cookedRoot = argv[3];
    const char* assetName = argv[4];


    if (!sJsonTypeLookup.contains(jsonType))
    {
        printf("Invalid json type: %s\n", jsonType);
        return -1;
    }

    if (!ConvertJsonToBinary(sJsonTypeLookup.find(jsonType)->second, assetRoot, cookedRoot, assetName))
    {
        printf("Cook failed.\n");
        return -1;
    }
    
    printf("Cook succeeded.\n");

    return 0;
}