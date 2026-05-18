#include "JSONCooker.h"
#include "../ThirdParty/rapidjson-master/include/rapidjson/document.h"
#include "../Math/NoodleMath.h"
#include "../Actor/Components/ComponentIds.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdio>
#include <filesystem>

static const std::filesystem::path APPLICATION_ASSEST_ROOT = "../../../RedAngel/Assets/";

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
    printf("JSON Parse Error:\n");

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
            }

            std::string type = component["Type"].GetString();
            printf("Parsing %s component...\n", type.c_str());

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

static bool sConvertSceneJsonToBinary(const char* assetRoot, rapidjson::Document& jsonDoc, std::ofstream& out)
{
    if (jsonDoc.HasMember("Actors"))
    {
        const rapidjson::Value& actors = jsonDoc["Actors"];

        if (!actors.IsArray())
        {
            printf("Error: Actors is not an array.\n");
            return false;
        }

        uint8 numActors = (uint8)actors.Size();
        printf("Scene has %i components.\n", numActors);
        out.write(reinterpret_cast<const char*>(&numActors), sizeof(uint8));

        uint32 actorIndex = 0;
        for (const rapidjson::Value& actor : actors.GetArray())
        {
            if (!actors.HasMember("Template"))
            {
                printf("Actor missing Template member.\n");
            }

            std::string templateName = actor["Template"].GetString();
            std::string actorInstanceName = templateName + std::to_string(actorIndex);

            ConvertJsonToBinary(eJsonType::ACTOR, templateName.c_str(), assetRoot);
            if (!sCookTransform(actor, jsonDoc, out))
            {
                printf("Failed to parse Transform component for %s\n", actorInstanceName.c_str());
                return false;
            }
            printf("Successfully parsed Transform component for %s.\n", actorInstanceName.c_str());
            ++actorIndex;
        }
    }
    return true;
}

bool ConvertJsonToBinary(eJsonType type, const char* assetRoot, const char* assetName)
{
    std::string inputPath(assetRoot);
    std::string typeDirectory;
    switch (type)
    {
    case eJsonType::ACTOR:
    {
        typeDirectory = "Actor/";
        break;
    }
    case eJsonType::SCENE:
    {
        typeDirectory = "Scene/";
        break;
    }
    default:
        break;
    }
    inputPath += typeDirectory + assetName + ".json";

    std::string jsonStr;
    if (!sLoadFileToString(inputPath, jsonStr))
    {
        printf("Failed to load JSON file to string from %s\n", inputPath.c_str());
        return false;
    }
    printf("Successfully loaded JSON file string from %s\n", inputPath.c_str());

    rapidjson::Document jsonDoc;
    jsonDoc.Parse(jsonStr.c_str());

    if (jsonDoc.HasParseError())
    {
        sPrintParseError(jsonDoc.GetParseError());
        return false;
    }

    std::string outputPath = assetRoot + typeDirectory + assetName + ".bin";
    std::ofstream out(outputPath, std::ios::binary);
    if (!out.is_open())
    {
        printf("Failed to open binary output file %s\n", outputPath.c_str());
        return false;
    }
    printf("Successfully opened binary output file %s\n", outputPath.c_str());

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
        success = sConvertSceneJsonToBinary(assetRoot, jsonDoc, out);
        break;
    }
    default:
        break;
    }

    out.close();
    return success;
}
