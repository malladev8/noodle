#include "JSONCooker.h"
#include "../ThirdParty/rapidjson-master/include/rapidjson/document.h"
#include "../Math/NoodleMath.h"
#include "../Actor/Components/ComponentIds.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdio>

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

bool ConvertActorJsonToBinary(const char* jsonPath, const char* binPath)
{
    std::string jsonStr;
    if (!sLoadFileToString(jsonPath, jsonStr))
    {
        printf("Failed to load JSON file to string at %s\n", jsonPath);
        return false;
    }
    printf("Successfully loaded JSON file string at %s\n", jsonPath);

    rapidjson::Document jsonDoc;
    jsonDoc.Parse(jsonStr.c_str());

    if (jsonDoc.HasParseError())
    {
        sPrintParseError(jsonDoc.GetParseError());
        return false;
    }

    std::ofstream out(binPath, std::ios::binary);
    if (!out.is_open())
    {
        printf("Failed to open binary output file for %s\n", binPath);
        return false;
    }
    printf("Successfully opened binary output file for %s\n", binPath);

    if (jsonDoc.HasMember("Components"))
    {
        const rapidjson::Value& components = jsonDoc["Components"];

        if (!components.IsArray())
        {
            printf("Error: Components is not an array.\n");
            out.close();
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

            if (type == "Transform")
            {
                eComponentId transformId = eComponentId::COMPONENT_TRANSFORM;
                out.write(reinterpret_cast<const char*>(&transformId), sizeof(eComponentId));

                vec3 posVec, rotVec, scaleVec;
                if (!sReadVec3(component["Position"], posVec) ||
                    !sReadVec3(component["Rotation"], rotVec) ||
                    !sReadVec3(component["Scale"], scaleVec))
                {
                    out.close();
                    return false;
                }

                mat4x4 translation = BuildTranslation(posVec.x, posVec.y, posVec.z);
                mat4x4 rotation = BuildRotation(DegToRad(rotVec.x), DegToRad(rotVec.y), DegToRad(rotVec.z));
                mat4x4 scale = BuildScale(scaleVec.x, scaleVec.y, scaleVec.z);
                mat4x4 trans = translation * rotation * scale;

                out.write(reinterpret_cast<const char*>(&trans), sizeof(mat4x4));
                printf("Successfully parsed Transform component.\n");
            }
        }
    }

    out.close();
	return true;
}
