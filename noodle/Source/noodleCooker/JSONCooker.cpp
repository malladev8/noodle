#include "JSONCooker.h"
#include "../ThirdParty/rapidjson-master/include/rapidjson/document.h"
#include "../Math/NoodleMath.h"
#include "../Actor/Components/ComponentIds.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdio>
#include <filesystem>
#include <unordered_set>

static std::unordered_set<std::string> sCookedActors;

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

static std::string sGetTypeDirectory(eJsonType type)
{
    switch (type)
    {
    case eJsonType::ACTOR:
    {
        return "Actor/";
    }
    case eJsonType::SCENE:
    {
        return "Scene/";
    }
    default:
        break;
    }
    return "";
}

static std::string sGenerateInputFilePath(const char* applicationRoot, const char* assetName, eJsonType type)
{
    std::string assetDirectory("Assets/");
    return applicationRoot + assetDirectory + sGetTypeDirectory(type) + assetName + ".json";
}

static std::string sGenerateOutputFilePath(const char* applicationRoot, const char* assetName, eJsonType type)
{
    std::string assetDirectory("Cooked/");
    return applicationRoot + assetDirectory + sGetTypeDirectory(type) + assetName + ".bin";
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

    mat4x4 translation = BuildTranslation(posVec.x, posVec.y, posVec.z);
    mat4x4 rotation = BuildRotation(DegToRad(rotVec.x), DegToRad(rotVec.y), DegToRad(rotVec.z));
    mat4x4 scale = BuildScale(scaleVec.x, scaleVec.y, scaleVec.z);
    mat4x4 transform = translation * rotation * scale;

    out.write(reinterpret_cast<const char*>(&transform), sizeof(mat4x4));
    return true;
}

static bool sConvertActorJsonToBinary(rapidjson::Document& jsonDoc, std::ofstream& out)
{
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
        }
    }
	return true;
}

static bool sConvertSceneJsonToBinary(const char* applicationRoot, rapidjson::Document& jsonDoc, std::ofstream& out)
{
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
            std::string actorOutputPath = sGenerateOutputFilePath(applicationRoot, prefabName.data(), eJsonType::ACTOR);
            
            // Actor Prefab Path
            uint8 actorPathLength = (uint8)actorOutputPath.length();
            out.write(reinterpret_cast<const char*>(&actorPathLength), sizeof(uint8));
            out.write(actorOutputPath.data(), actorPathLength);

            // Cook Actor Prefab
            // Only need to cook actor prefab once per scene
            if (!sCookedActors.contains(prefabName))
            {
                if (!ConvertJsonToBinary(eJsonType::ACTOR, applicationRoot, prefabName.data()))
                {
                    return false;
                }
                printf("Successfully cooked %s Actor Prefab.\n", prefabName.data());
                sCookedActors.insert(prefabName);
            }

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
    std::string inputPath = sGenerateInputFilePath(applicationRoot, assetName, type);
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

    std::string outputPath = sGenerateOutputFilePath(applicationRoot, assetName, type);
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
        success = sConvertActorJsonToBinary(jsonDoc, out);
        break;
    }
    case eJsonType::SCENE:
    {
        success = sConvertSceneJsonToBinary(applicationRoot, jsonDoc, out);
        sCookedActors.clear();
        break;
    }
    default:
        break;
    }

    out.close();
    return success;
}
