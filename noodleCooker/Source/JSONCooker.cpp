#include "JSONCooker.h"
#include "TextureCooker.h"
#include "ThirdParty/rapidjson-master/include/rapidjson/document.h"
#include "Math/NoodleMath.h"
#include "Actor/Components/ComponentIds.h"
#include "Resource/ResourceTypes.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdio>
#include <filesystem>
#include <unordered_map>

static const char* BINARY_EXTENSION = ".bin";

struct AssetMetadata
{
    AssetId assedId = 0;
    std::string sourcePath = "";
    std::string cookedPath = "";
};

static std::unordered_map <AssetId, AssetMetadata> sCookedAssets;
static void sClearCookedCache()
{
    sCookedAssets.clear();
}

static AssetId sGenerateAssetId(const std::string& path)
{
    std::filesystem::path normalized = path;
    normalized = normalized.lexically_normal();
    normalized.make_preferred();
    return std::hash<std::filesystem::path>{}(normalized.c_str());
}

static bool sLoadFileToString(const std::string& path, std::string& out)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    out = buffer.str();
    file.close();
	return true;
}

static bool sReadVec3(const rapidjson::Value& arr, vec3& out)
{
    if (!arr.IsArray() || arr.Size() != 3)
    {
        return false;
    }

    for (rapidjson::SizeType i = 0; i < 3; ++i)
    {
        if (!arr[i].IsNumber())
        {
            return false;
        }
        out.data[i] = arr[i].GetFloat();
    }
    return true;
}

static void sPrintParseError(rapidjson::ParseErrorCode errorCode)
{
    printf("Json Parse Error:\n");

    switch (errorCode)
    {
    case rapidjson::kParseErrorNone:
        printf("Error Code: None.");
        break;
    case rapidjson::kParseErrorDocumentEmpty:
        printf("The document is empty.");
        break;
    case rapidjson::kParseErrorDocumentRootNotSingular:
        printf("The document root must not follow by other values.");
        break;
    case rapidjson::kParseErrorValueInvalid:
        printf("Invalid value.");
        break;
    case rapidjson::kParseErrorObjectMissName:
        printf("Missing a name for object member.");
        break;
    case rapidjson::kParseErrorObjectMissColon:
        printf("Missing a colon after a name of object member.");
        break;
    case rapidjson::kParseErrorObjectMissCommaOrCurlyBracket:
        printf("Missing a comma or '}' after an object member.");
        break;
    case rapidjson::kParseErrorArrayMissCommaOrSquareBracket:
        printf("Missing a comma or ']' after an array element.");
        break;
    case rapidjson::kParseErrorStringUnicodeEscapeInvalidHex:
        printf("Incorrect hex digit after \\u escape in string.");
        break;
    case rapidjson::kParseErrorStringUnicodeSurrogateInvalid:
        printf("The surrogate pair in string is invalid.");
        break;
    case rapidjson::kParseErrorStringEscapeInvalid:
        printf("Invalid escape character in string.");
        break;
    case rapidjson::kParseErrorStringMissQuotationMark:
        printf("Missing a closing quotation mark in string.");
        break;
    case rapidjson::kParseErrorStringInvalidEncoding:
        printf("Invalid encoding in string.");
        break;
    case rapidjson::kParseErrorNumberTooBig:
        printf("Number too big to be stored in double.");
        break;
    case rapidjson::kParseErrorNumberMissFraction:
        printf("Miss fraction part in number.");
        break;
    case rapidjson::kParseErrorNumberMissExponent:
        printf("Miss exponent in number.");
        break;
    case rapidjson::kParseErrorTermination:
        printf("Parsing was terminated.");
        break;
    case rapidjson::kParseErrorUnspecificSyntaxError:
        printf("Unspecific syntax error.");
        break;
    default:
        break;
    }
}

static std::string sGenerateInputFilePath(const char* applicationRoot, const char* assetName, const char* exentsion)
{
    std::string assetDirectory("Assets\\");
    return applicationRoot + assetDirectory + assetName + exentsion;
}

static std::string sGenerateOutputFilePath(const char* applicationRoot, const char* assetName)
{
    std::string assetDirectory("Cooked\\");
    return applicationRoot + assetDirectory + assetName + BINARY_EXTENSION;
}

static bool sCookTransform(const rapidjson::Value& jsonValue, const rapidjson::Document& jsonDoc, std::ofstream& out)
{
    eComponentId transformId = eComponentId::COMPONENT_TRANSFORM;
    out.write(reinterpret_cast<const char*>(&transformId), sizeof(eComponentId));

    vec3 posVec, rotVec, scaleVec;
    if (!sReadVec3(jsonValue["Position"], posVec) ||
        !sReadVec3(jsonValue["Rotation"], rotVec) ||
        !sReadVec3(jsonValue["Scale"], scaleVec))
    {
        return false;
    }

    out.write(reinterpret_cast<const char*>(&posVec), sizeof(vec3));
    out.write(reinterpret_cast<const char*>(&scaleVec), sizeof(vec3));

    rotVec.x = DegToRad(rotVec.x);
    rotVec.y = DegToRad(rotVec.y);
    rotVec.z = DegToRad(rotVec.z);
    quaternion rotation = quaternion::FromEuler(rotVec.x, rotVec.y, rotVec.z);
    out.write(reinterpret_cast<const char*>(&rotation), sizeof(quaternion));
    return true;
}

static bool sConvertActorJsonToBinary(const char* applicationRoot, rapidjson::Document& jsonDoc, std::ofstream& out, AssetId assetId)
{
    out.write(reinterpret_cast<const char*>(&assetId), sizeof(AssetId));

    if (jsonDoc.HasMember("Components"))
    {
        const rapidjson::Value& components = jsonDoc["Components"];

        if (!components.IsArray())
        {
            printf("Error: Components is not an array.\n");
            return false;
        }

        uint8 numComponents = (uint8)components.Size();
        printf("Actor has %i components.\n", numComponents);
        out.write(reinterpret_cast<const char*>(&numComponents), sizeof(uint8));

        for (const rapidjson::Value& component : components.GetArray())
        {
            if (!component.HasMember("Type"))
            {
                printf("Component missing Type member.\n");
                return false;
            }

            std::string type = component["Type"].GetString();
            printf("Parsing %s component...\n", type.data());

            // Note, Transform components are typically defined in the scene json, relative to the scene's origin
            if (type == "Transform")
            {
                printf("WARNING: Parsing Transform component directly from Actor json. Make sure this is intentional.\n");
                if (!sCookTransform(component, jsonDoc, out))
                {
                    printf("Failed to parse Transform component.\n");
                    return false;
                }
                printf("Successfully parsed Transform component.\n");
            }
            else if (type == "Sprite")
            {
                eComponentId spriteId = eComponentId::COMPONENT_SPRITE;
                out.write(reinterpret_cast<const char*>(&spriteId), sizeof(eComponentId));

                if (!component.HasMember("Asset"))
                {
                    printf("Sprite Component missing Asset member.\n");
                    return false;
                }

                std::string assetName = component["Asset"].GetString();
                std::string assetOutputPath = sGenerateOutputFilePath(applicationRoot, assetName.data());

                // TODO: Need to link cooker project against engine to clean up linker errors to Path::WritePath()
                path::WritePath(assetOutputPath, out);

                AssetId assetId = sGenerateAssetId(assetName);

                if (!sCookedAssets.contains(assetId))
                {
                    std::filesystem::path fsOutputPath = assetOutputPath;
                    std::filesystem::create_directories(fsOutputPath.parent_path());
                    std::ofstream textureOut(assetOutputPath, std::ios::binary);
                    if (!textureOut.is_open())
                    {
                        printf("Failed to open binary output file %s\n", assetOutputPath.data());
                        return false;
                    }

                    std::string assetInputPath = sGenerateInputFilePath(applicationRoot, assetName.data(), ".png");
                    TextureCooker textureCooker;
                    if (!textureCooker.CookTexture(assetInputPath.c_str(), textureOut, assetId))
                    {
                        textureOut.close();
                        return false;
                    }
                    textureOut.close();

                    AssetMetadata metadata;
                    metadata.assedId = assetId;
                    metadata.sourcePath = assetInputPath;
                    metadata.cookedPath = assetOutputPath;
                    sCookedAssets.insert({ assetId, metadata });
                }
                printf("Successfully parsed Sprite component.\n");
            }
        }
    }
	return true;
}

static bool sConvertSceneJsonToBinary(const char* applicationRoot, rapidjson::Document& jsonDoc, std::ofstream& out, AssetId assetId)
{
    out.write(reinterpret_cast<const char*>(&assetId), sizeof(AssetId));

    if (jsonDoc.HasMember("Actors"))
    {
        const rapidjson::Value& actors = jsonDoc["Actors"];

        if (!actors.IsArray())
        {
            printf("Error: Actors is not an array.\n");
            return false;
        }

        uint32 numActors = (uint32)actors.Size();
        printf("Scene has %i components.\n", numActors);
        out.write(reinterpret_cast<const char*>(&numActors), sizeof(uint32));

        uint32 actorIndex = 0;
        for (const rapidjson::Value& actor : actors.GetArray())
        {
            if (!actor.HasMember("Prefab"))
            {
                printf("Actor missing Prefab member.\n");
            }

            std::string prefabName = actor["Prefab"].GetString();
            std::string actorInstanceName = prefabName + "_" + std::to_string(actorIndex);
            std::string actorOutputPath = sGenerateOutputFilePath(applicationRoot, prefabName.data());
            
            // Actor Prefab Path
            path::WritePath(actorOutputPath, out);

            // Cook Actor Prefab
            if (!ConvertJsonToBinary(eJsonType::ACTOR, applicationRoot, prefabName.data()))
            {
                return false;
            }
            printf("Successfully cooked %s Actor Prefab.\n", prefabName.data());

            // Actor Scene Transform
            if (!sCookTransform(actor, jsonDoc, out))
            {
                printf("Failed to parse Transform component for %s\n", actorInstanceName.data());
                return false;
            }
            printf("Successfully parsed Transform component for %s.\n", actorInstanceName.data());
            ++actorIndex;
        }
    }
    return true;
}

bool ConvertJsonToBinary(eJsonType type, const char* applicationRoot, const char* assetName)
{
    AssetId assetId = sGenerateAssetId(assetName);
    if (sCookedAssets.contains(assetId))
    {
        return true;
    }

    std::string inputPath = sGenerateInputFilePath(applicationRoot, assetName, ".json");
    std::string jsonStr;
    if (!sLoadFileToString(inputPath, jsonStr))
    {
        printf("Failed to load json file to string from %s\n", inputPath.data());
        return false;
    }
    printf("Successfully loaded json file string from %s\n", inputPath.data());

    rapidjson::Document jsonDoc;
    jsonDoc.Parse(jsonStr.data());

    if (jsonDoc.HasParseError())
    {
        sPrintParseError(jsonDoc.GetParseError());
        return false;
    }

    std::string outputPath = sGenerateOutputFilePath(applicationRoot, assetName);
    std::filesystem::path fsOutputPath = outputPath;
    std::filesystem::create_directories(fsOutputPath.parent_path());
    std::ofstream out(outputPath, std::ios::binary);
    if (!out.is_open())
    {
        printf("Failed to open binary output file %s\n", outputPath.data());
        return false;
    }
    printf("Successfully opened binary output file %s\n", outputPath.data());

    bool success = false;

    switch (type)
    {
    case eJsonType::ACTOR: 
    {
        success = sConvertActorJsonToBinary(applicationRoot, jsonDoc, out, assetId);
        break;
    }
    case eJsonType::SCENE:
    {
        success = sConvertSceneJsonToBinary(applicationRoot, jsonDoc, out, assetId);
        sClearCookedCache();
        break;
    }
    default:
        break;
    }

    out.close();

    AssetMetadata metadata;
    metadata.assedId = assetId;
    metadata.sourcePath = inputPath;
    metadata.cookedPath = outputPath;
    sCookedAssets.insert({ assetId, metadata });

    return success;
}
