#include "JSONCooker.h"
#include "ShaderCooker.h"
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

static bool sReadVec4(const rapidjson::Value& arr, vec4& out)
{
    if (!arr.IsArray() || arr.Size() != 4)
    {
        return false;
    }

    for (rapidjson::SizeType i = 0; i < 4; ++i)
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

static bool sValidateJsonValueMember(const rapidjson::Value& value, const char*& memberName)
{
    if (!value.HasMember(memberName))
    {
        printf("%s is missing %s member.", value.GetString(), memberName);
        return false;
    }
    return true;
}

static std::string sGenerateInputFilePath(const char* applicationRoot, const char* assetName, const char* exentsion)
{
    std::string assetDirectory("Assets/");
    return applicationRoot + assetDirectory + assetName + exentsion;
}

static std::string sGenerateOutputFilePath(const char* applicationRoot, const char* assetName)
{
    std::string assetDirectory("Cooked/");
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

        for (const rapidjson::Value& actor : actors.GetArray())
        {
            if (!actor.HasMember("Prefab"))
            {
                printf("Actor missing Prefab member.\n");
            }

            std::string prefabName = actor["Prefab"].GetString();
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
                printf("Failed to parse Transform component for %s\n", prefabName.c_str());
                return false;
            }
            printf("Successfully parsed Transform component for %s.\n", prefabName.c_str());
        }
    }
    return true;
}

static bool sConvertActorJsonToBinary(const char* applicationRoot, rapidjson::Document& jsonDoc, std::ofstream& out)
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
            const char* typeStr = "Type";
            if (!sValidateJsonValueMember(component, typeStr))
            {
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

                const char* material = "Material";
                if (!sValidateJsonValueMember(component, material))
                {
                    return false;
                }

                std::string materialName = component[material].GetString();
                std::string materialOutputPath = sGenerateOutputFilePath(applicationRoot, materialName.data());

                // Write material path
                path::WritePath(materialOutputPath, out);

                // Cook Material
                if (!ConvertJsonToBinary(eJsonType::MATERIAL, applicationRoot, materialName.data()))
                {
                    return false;
                }

                const char* ppuName = "PixelsPerUnit";
                if (!sValidateJsonValueMember(component, ppuName))
                {
                    return false;
                }
                uint32 ppu = component[ppuName].GetUint();
                out.write(reinterpret_cast<const char*>(&ppu), sizeof(ppu));

                printf("Successfully cooked %s Material.\n", materialName.data());
                printf("Successfully parsed Sprite component.\n");
            }
            else if (type == "Camera")
            {
                eComponentId cameraId = eComponentId::COMPONENT_CAMERA;
                out.write(reinterpret_cast<const char*>(&cameraId), sizeof(cameraId));

                const char* projection = "Projection";
                if (!sValidateJsonValueMember(component, projection))
                {
                    return false;
                }

                std::string projectionName = component[projection].GetString();
                if (_strcmpi(projectionName.c_str(), "Orthographic") == 0)
                {
                    eProjectionType projectionType = eProjectionType::ORTHOGRAPHIC;
                    out.write(reinterpret_cast<const char*>(&projectionType), sizeof(projectionType));

                    const char* near = "NearPlane";
                    if (!sValidateJsonValueMember(component, near))
                    {
                        return false;
                    }
                    float32 nearPlane = component[near].GetFloat();
                    out.write(reinterpret_cast<const char*>(&nearPlane), sizeof(float32));

                    const char* far = "FarPlane";
                    if (!sValidateJsonValueMember(component, far))
                    {
                        return false;
                    }
                    float32 farPlane = component[far].GetFloat();
                    out.write(reinterpret_cast<const char*>(&farPlane), sizeof(float32));

                    const char* ortho = "OrthoHeight";
                    if (!sValidateJsonValueMember(component, ortho))
                    {
                        return false;
                    }
                    float32 orthoHeight = component[ortho].GetFloat();
                    out.write(reinterpret_cast<const char*>(&orthoHeight), sizeof(float32));

                    const char* clear = "ClearColor";
                    if (!sValidateJsonValueMember(component, clear))
                    {
                        return false;
                    }
                    vec4 clearColor;
                    sReadVec4(component[clear], clearColor);
                    out.write(reinterpret_cast<const char*>(&clearColor), sizeof(vec4));
                }
                else
                {
                    printf("%s projection not yet supported.\n", projectionName.c_str());
                    return false;
                }
                printf("Successfully parsed Camera component.\n");
            }
        }
    }
	return true;
}

bool static sConvertMaterialJsonToBinary(const char* applicationRoot, rapidjson::Document& jsonDoc, std::ofstream& out)
{
    // Cook Shaders
    const char* shader = "Shader";
    if (!jsonDoc.HasMember(shader))
    {
        printf("Material missing Shader member.\n");
        return false;
    }

    std::string shaderName = jsonDoc[shader].GetString();
    std::string shaderOutputPath = sGenerateOutputFilePath(applicationRoot, shaderName.data());

    path::WritePath(shaderOutputPath, out);

    if (!ConvertJsonToBinary(eJsonType::SHADER, applicationRoot, shaderName.data()))
    {
        return false;
    }
    printf("Successfully cooked %s Shader.\n", shaderName.data());

    // Cook Textures
    const char* diffuseTexture = "DiffuseTexture";
    if (!jsonDoc.HasMember(diffuseTexture))
    {
        printf("Material missing DiffuseTexture member.\n");
        return false;
    }
    
    std::string textureName = jsonDoc[diffuseTexture].GetString();
    std::string textureOutputPath = sGenerateOutputFilePath(applicationRoot, textureName.data());
    
    path::WritePath(textureOutputPath, out);
    
    AssetId textureAssetId = sGenerateAssetId(textureName);
    
    if (!sCookedAssets.contains(textureAssetId))
    {
        std::filesystem::path fsOutputPath = textureOutputPath;
        std::filesystem::create_directories(fsOutputPath.parent_path());
        std::ofstream textureOut(textureOutputPath, std::ios::binary);
        if (!textureOut.is_open())
        {
            printf("Failed to open binary output file %s\n", textureOutputPath.data());
            return false;
        }
    
        std::string textureInputPath = sGenerateInputFilePath(applicationRoot, textureName.data(), ".png");
        TextureCooker textureCooker;
        if (!textureCooker.CookTexture(textureInputPath.c_str(), textureOut, textureAssetId))
        {
            textureOut.close();
            return false;
        }
        textureOut.close();
    
        AssetMetadata metadata;
        metadata.assedId = textureAssetId;
        metadata.sourcePath = textureInputPath;
        metadata.cookedPath = textureOutputPath;
        sCookedAssets.insert({ textureAssetId, metadata });
    }

    return true;
}

bool static sCookShader(const std::string& shaderType, const char* applicationRoot, rapidjson::Document& jsonDoc, std::string& outOutputFilePath)
{
    ShaderCooker shaderCooker;

    if (!jsonDoc.HasMember(shaderType.c_str()))
    {
        printf("Shader missing %s member.\n", shaderType.c_str());
        return false;
    }
    const rapidjson::Value& shader = jsonDoc[shaderType.c_str()];

    if (!shader.HasMember("Source"))
    {
        printf("%s missing Source member.\n", shaderType.c_str());
        return false;
    }
    const rapidjson::Value& vsSource = shader["Source"];

    if (!shader.HasMember("Entry"))
    {
        printf("%s missing Entry member.\n", shaderType.c_str());
        return false;
    }
    const rapidjson::Value& vsEntry = shader["Entry"];

    if (!shader.HasMember("Profile"))
    {
        printf("VS missing Profile member.\n");
        return false;
    }
    const rapidjson::Value& vsProfile = shader["Profile"];

    std::string sourceName = vsSource.GetString();
    std::string inputPath = sGenerateInputFilePath(applicationRoot, sourceName.data(), ".hlsl");
    outOutputFilePath = sGenerateOutputFilePath(applicationRoot, sourceName.data());
    std::filesystem::path fsOutputPath = outOutputFilePath;
    std::filesystem::create_directories(fsOutputPath.parent_path());
    shaderCooker.CookShaderByteCode(inputPath, vsEntry.GetString(), vsProfile.GetString(), outOutputFilePath);

    return true;
}

bool static sConvertShaderJsonToBinary(const char* applicationRoot, rapidjson::Document& jsonDoc, std::ofstream& out)
{
    // Cook Shader Byte Code
    std::string vsOutputPath;
    if (!sCookShader("VS", applicationRoot, jsonDoc, vsOutputPath))
    {
        printf("Failed to Cook VS.\n");
        return false;
    }

    std::string psOutputPath;
    if (!sCookShader("PS", applicationRoot, jsonDoc, psOutputPath))
    {
        printf("Failed to Cook PS.\n");
        return false;
    }

    // Gather shader data
    std::ifstream vsFile(vsOutputPath, std::ios::binary | std::ios::ate);
    if (!vsFile.is_open())
    {
        printf("Failed to open VS byte code %s.\n", vsOutputPath.c_str());
        return false;
    }
    uint32 vsSize = static_cast<uint32>(vsFile.tellg());
    std::vector<uint8> vsData(vsSize);
    vsFile.seekg(0);
    vsFile.read(reinterpret_cast<char*>(vsData.data()), vsSize);
    vsFile.close();
    std::remove(vsOutputPath.c_str());

    std::ifstream psFile(psOutputPath, std::ios::binary | std::ios::ate);
    if (!psFile.is_open())
    {
        printf("Failed to open PS byte code %s.\n", psOutputPath.c_str());
        return false;
    }
    uint32 psSize = static_cast<uint32>(psFile.tellg());
    std::vector<uint8> psData(psSize);
    psFile.seekg(0);
    psFile.read(reinterpret_cast<char*>(psData.data()), psSize);
    psFile.close();
    std::remove(psOutputPath.c_str());

    // Write Shader Data to bin
    out.write(reinterpret_cast<const char*>(&vsSize), sizeof(vsSize));
    out.write(reinterpret_cast<const char*>(&psSize), sizeof(psSize));
    out.write(reinterpret_cast<const char*>(vsData.data()), vsSize);
    out.write(reinterpret_cast<const char*>(psData.data()), psSize);
   
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

    if (type < eJsonType::COUNT)
    {
        out.write(reinterpret_cast<const char*>(&assetId), sizeof(AssetId));
    }

    switch (type)
    {
    case eJsonType::SCENE:
    {
        success = sConvertSceneJsonToBinary(applicationRoot, jsonDoc, out);
        sClearCookedCache();
        break;
    }
    case eJsonType::ACTOR:
    {
        success = sConvertActorJsonToBinary(applicationRoot, jsonDoc, out);
        break;
    }
    case eJsonType::MATERIAL:
    {
        success = sConvertMaterialJsonToBinary(applicationRoot, jsonDoc, out);
        break;
    }
    case eJsonType::SHADER:
    {
        success = sConvertShaderJsonToBinary(applicationRoot, jsonDoc, out);
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
