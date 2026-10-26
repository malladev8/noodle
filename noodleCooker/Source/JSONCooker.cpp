#include "JSONCooker.h"
#include "ShaderCooker.h"
#include "TextureCooker.h"
#include "ThirdParty/rapidjson-master/include/rapidjson/document.h"
#include "Actor/Components/ComponentIds.h"
#include "Resource/SerializationTypes.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdio>
#include <filesystem>
#include <unordered_map>
#include <optional>

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

static std::unordered_map<std::string, eSpriteLayer> sSpriteLayerLookup
{
    {"Background", eSpriteLayer::BACKGROUND},
    {"World", eSpriteLayer::WORLD},
    {"Character", eSpriteLayer::CHARACTER},
    {"Foreground", eSpriteLayer::FOREGROUND},
    {"UI", eSpriteLayer::UI},
};

static std::unordered_map<std::string, eComponentId> sComponentTypeLookup
{
    {"Transform", eComponentId::COMPONENT_TRANSFORM},
    {"Sprite", eComponentId::COMPONENT_SPRITE},
    {"SpriteAnimator", eComponentId::COMPONENT_SPRITE_ANIMATOR},
    {"Movement", eComponentId::COMPONENT_MOVEMENT},
    {"Camera", eComponentId::COMPONENT_CAMERA},
    {"Collider", eComponentId::COMPONENT_COLLIDER},
};

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

static bool sReadVec2(const rapidjson::Value& arr, vec2& out)
{
    if (!arr.IsArray() || arr.Size() != 2)
    {
        return false;
    }

    for (rapidjson::SizeType i = 0; i < 2; ++i)
    {
        if (!arr[i].IsNumber())
        {
            return false;
        }
        out.data[i] = arr[i].GetFloat();
    }
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

static bool sParseTransformComponent(const rapidjson::Value& component, TransformSerializationData& outData)
{
    vec3 posVec, rotVec, scaleVec;

    if (component.HasMember("Position"))
    {
        sReadVec3(component["Position"], posVec);
        outData.position = posVec;
    }

    if (component.HasMember("Scale"))
    {
        sReadVec3(component["Scale"], scaleVec);
        outData.scale = scaleVec;
    }

    if (component.HasMember("Rotation"))
    {
        sReadVec3(component["Rotation"], rotVec);
        rotVec.x = DegToRad(rotVec.x);
        rotVec.y = DegToRad(rotVec.y);
        rotVec.z = DegToRad(rotVec.z);
        quaternion rotation = quaternion::FromEuler(rotVec.x, rotVec.y, rotVec.z);
        outData.rotation = rotation;
    }
    
    return true;
}

static bool sParseSpriteComponent(const rapidjson::Value& component, SpriteSerializationData& outData)
{
    // Material
    const char* material = "Material";
    if (component.HasMember(material))
    {
        std::string materialName = component[material].GetString();
        outData.material = materialName;
    }

    // Atlas
    const char* atlas = "Atlas";
    bool usingAtlas = component.HasMember(atlas);
    outData.usingAtlas = usingAtlas || outData.usingAtlas;

    if (usingAtlas)
    {
        std::string atlasName = component[atlas].GetString();
        outData.atlas = atlasName;

        const char* regionMemberName = "Region";
        if (component.HasMember(regionMemberName))
        {
            std::string region = component[regionMemberName].GetString();
            outData.region = region;
        }
    }

    // Pixels per unit
    const char* ppuName = "PixelsPerUnit";
    if (component.HasMember(ppuName))
    {
        uint32 ppu = component[ppuName].GetUint();
        outData.pixlesPerUnit = ppu;
    }

    // Color
    const char* colorName = "Color";
    if (component.HasMember(colorName))
    {
        vec4 color;
        sReadVec4(component[colorName], color);
        outData.color = color;
    }

    // Layering
    const char* layerMemberName = "Layer";
    const char* orderMemberName = "Order";

    if (component.HasMember(layerMemberName))
    {
        std::string layerName = component[layerMemberName].GetString();
        eSpriteLayer spriteLayer = eSpriteLayer::BACKGROUND;
        if (!sSpriteLayerLookup.contains(layerName))
        {
            printf("Invalid sprite layer: %s.\n", layerName.data());
        }
        else
        {
            spriteLayer = sSpriteLayerLookup[layerName];
        }
        outData.layer = spriteLayer;
    }
    
    if (component.HasMember(orderMemberName))
    {
        int32 spriteOrder = component[orderMemberName].GetInt();
        outData.order = spriteOrder;
    }
   
    return true;
}

static bool sParseSpriteAnimatorComponent(const rapidjson::Value& component, SpriteAnimatorSerializationData& outData)
{
    const char* animationClipsName = "AnimationClips";
    if (!sValidateJsonValueMember(component, animationClipsName))
    {
        return false;
    }

    if (!component[animationClipsName].IsArray())
    {
        printf("Error: Animation Clips is not an array.\n");
        return false;
    }

    uint32 numClips = component[animationClipsName].Size();
    outData.numClips += numClips;

    printf("Parsing %i animation clips.\n", numClips);

    for (const rapidjson::Value& animationClip : component[animationClipsName].GetArray())
    {
        SpriteAnimationClip animClipData;
        const char* clipName = "Name";
        if (!sValidateJsonValueMember(animationClip, clipName))
        {
            return false;
        }
        std::string clipNameStr = animationClip[clipName].GetString();
        uint32 clipNameLength = (uint32)clipNameStr.length();
        animClipData.name = clipNameStr;

        const char* fpsName = "FPS";
        if (!sValidateJsonValueMember(animationClip, fpsName))
        {
            return false;
        }
        uint32 fps = animationClip[fpsName].GetUint();
        animClipData.fps = fps;

        const char* framesName = "Frames";
        if (!sValidateJsonValueMember(animationClip, framesName))
        {
            return false;
        }
        uint32 frames = animationClip[framesName].GetUint();
        animClipData.frames = frames;

        const char* loopName = "Loop";
        if (!sValidateJsonValueMember(animationClip, loopName))
        {
            return false;
        }
        bool loop = animationClip[loopName].GetBool();
        animClipData.loop = loop;

        outData.animationClips.push_back(animClipData);
    }
    return true;
}

static bool sParseCameraComponent(const rapidjson::Value& component, CameraSerializationData& outData)
{
    const char* projection = "Projection";
    if (component.HasMember(projection))
    {
        std::string projectionName = component[projection].GetString();
        if (_strcmpi(projectionName.c_str(), "Orthographic") == 0)
        {
            eProjectionType projectionType = eProjectionType::ORTHOGRAPHIC;
            outData.projectionType = projectionType;

            const char* near = "NearPlane";
            if (component.HasMember(near))
            {
                float32 nearPlane = component[near].GetFloat();
                outData.nearPlane = nearPlane;
            }

            const char* far = "FarPlane";
            if (component.HasMember(far))
            {
                float32 farPlane = component[far].GetFloat();
                outData.farPlane = farPlane;
            }

            const char* ortho = "OrthoHeight";
            if (component.HasMember(ortho))
            {
                float32 orthoHeight = component[ortho].GetFloat();
                outData.orthoHeight = orthoHeight;
            }

            const char* clear = "ClearColor";
            if (component.HasMember(clear))
            {
                vec4 clearColor;
                sReadVec4(component[clear], clearColor);
                outData.clearColor = clearColor;
            }
        }
        else
        {
            printf("%s projection not yet supported.\n", projectionName.c_str());
        }
    }
    
    return true;
}

static bool sParseMovementComponent(const rapidjson::Value& component, MovementSerializationData& outData)
{
    const char* speedName = "Speed";
    if (component.HasMember(speedName))
    {
        float32 speed = component[speedName].GetFloat();
        outData.speed = speed;
    }
    return true;
}

static bool sParseColliderComponent(const rapidjson::Value& component, ColliderSerializationData& outData)
{
    const char* colliderShapeName = "Shape";
    if (component.HasMember(colliderShapeName))
    {
        std::string shapeStr = component[colliderShapeName].GetString();
        if (_strcmpi(shapeStr.c_str(),"AABB") == 0)
        {
            outData.colliderType = eColliderType::AABB;
        }
        else if (_strcmpi(shapeStr.c_str(), "Circle") == 0)
        {
            outData.colliderType = eColliderType::CIRCLE;
        }
    }

    const char* offsetName = "Offset";
    if (component.HasMember(offsetName))
    {
        sReadVec3(component[offsetName], outData.offset);
    }

    const char* halfExtentsName = "HalfExtents";
    if (component.HasMember(halfExtentsName))
    {
        sReadVec3(component[halfExtentsName], outData.halfExtents);
    }

    const char* radiusName = "Radius";
    if (component.HasMember(radiusName))
    {
        outData.radius = component[radiusName].GetFloat();
    }
    return true;
}

static bool sParseComponents(const rapidjson::Value& components, std::vector<std::unique_ptr<ComponentSerializationData>>& componentData, uint8& numComponents)
{
    if (!components.IsArray())
    {
        printf("Error: Components is not an array.\n");
        return false;
    }

    for (const rapidjson::Value& component : components.GetArray())
    {
        const char* typeStr = "Type";
        if (!sValidateJsonValueMember(component, typeStr))
        {
            return false;
        }

        std::string type = component["Type"].GetString();
        printf("Parsing %s component...\n", type.data());

        eComponentId componentType = sComponentTypeLookup[type];

        switch (componentType)
        {
        default:
        {
            printf("Unsupported Component Type: %s\n", type.data());
            break;
        }
        case eComponentId::COMPONENT_TRANSFORM:
        {
            TransformSerializationData* transformData = nullptr;
            if (componentData[(uint32)componentType].get() == nullptr)
            {
                transformData = N_NEW TransformSerializationData();
                componentData[(uint32)componentType].reset(transformData);
                ++numComponents;
            }
            else
            {
                transformData = static_cast<TransformSerializationData*>(componentData[(uint32)componentType].get());
            }
            transformData->componentId = componentType;
            sParseTransformComponent(component, *transformData);
            break;
        }
        case eComponentId::COMPONENT_SPRITE:
        {
            SpriteSerializationData* spriteData = nullptr;
            if (componentData[(uint32)componentType].get() == nullptr)
            {
                spriteData = N_NEW SpriteSerializationData();
                componentData[(uint32)componentType].reset(spriteData);
                ++numComponents;
            }
            else
            {
                spriteData = static_cast<SpriteSerializationData*>(componentData[(uint32)componentType].get());
            }
            spriteData->componentId = componentType;
            sParseSpriteComponent(component, *spriteData);
            break;
        }
        case eComponentId::COMPONENT_SPRITE_ANIMATOR:
        {
            SpriteAnimatorSerializationData* spriteAnimatorData = nullptr;
            if (componentData[(uint32)componentType].get() == nullptr)
            {
                spriteAnimatorData = N_NEW SpriteAnimatorSerializationData();
                componentData[(uint32)componentType].reset(spriteAnimatorData);
                ++numComponents;
            }
            else
            {
                spriteAnimatorData = static_cast<SpriteAnimatorSerializationData*>(componentData[(uint32)componentType].get());
            }
            spriteAnimatorData->componentId = componentType;
            sParseSpriteAnimatorComponent(component, *spriteAnimatorData);
            break;
        }
        case eComponentId::COMPONENT_CAMERA:
        {
            CameraSerializationData* cameraData = nullptr;
            if (componentData[(uint32)componentType].get() == nullptr)
            {
                cameraData = N_NEW CameraSerializationData();
                componentData[(uint32)componentType].reset(cameraData);
                ++numComponents;
            }
            else
            {
                cameraData = static_cast<CameraSerializationData*>(componentData[(uint32)componentType].get());
            }
            cameraData->componentId = componentType;
            sParseCameraComponent(component, *cameraData);
            break;
        }
        case eComponentId::COMPONENT_MOVEMENT:
        {
            MovementSerializationData* movementData = nullptr;
            if (componentData[(uint32)componentType].get() == nullptr)
            {
                movementData = N_NEW MovementSerializationData();
                componentData[(uint32)componentType].reset(movementData);
                ++numComponents;
            }
            else
            {
                movementData = static_cast<MovementSerializationData*>(componentData[(uint32)componentType].get());
            }
            movementData->componentId = componentType;
            sParseMovementComponent(component, *movementData);
            break;
        }
        case eComponentId::COMPONENT_COLLIDER:
        {
            ColliderSerializationData* colliderData = nullptr;
            if (componentData[(uint32)componentType].get() == nullptr)
            {
                colliderData = N_NEW ColliderSerializationData();
                componentData[(uint32)componentType].reset(colliderData);
                ++numComponents;
            }
            else
            {
                colliderData = static_cast<ColliderSerializationData*>(componentData[(uint32)componentType].get());
            }
            colliderData->componentId = componentType;
            sParseColliderComponent(component, *colliderData);
            break;
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
        printf("Scene has %i actors.\n", numActors);
        out.write(reinterpret_cast<const char*>(&numActors), sizeof(uint32));

        for (const rapidjson::Value& actor : actors.GetArray())
        {
            // ID
            if (!actor.HasMember("Id"))
            {
                printf("Actor missing Id member.\n");
            }
            uint32 actorId = actor["Id"].GetUint();
            out.write(reinterpret_cast<const char*>(&actorId), sizeof(actorId));
            printf("Parsing Actor ID: %i.\n", actorId);

            // Parent ID
            uint32 parentId = UINT_MAX;
            bool hasParent = false;
            if (actor.HasMember("Parent"))
            {
                parentId = actor["Parent"].GetUint();
                hasParent = true;
            }
            out.write(reinterpret_cast<const char*>(&hasParent), sizeof(hasParent));
            if (hasParent)
            {
                out.write(reinterpret_cast<const char*>(&parentId), sizeof(parentId));
            }

            // Controller
            std::string controllerType;
            bool hasController = false;
            if (actor.HasMember("Controller"))
            {
                controllerType = actor["Controller"].GetString();
                hasController = true;
            }
            out.write(reinterpret_cast<const char*>(&hasController), sizeof(hasController));
            if (hasController)
            {
                printf("Actor has Controller type: %s\n", controllerType.c_str());

                uint8 controllerTypeLength = (uint8)controllerType.length();
                out.write(reinterpret_cast<const char*>(&controllerTypeLength), sizeof(uint8));
                out.write(controllerType.data(), controllerTypeLength);
            }

            // Prepare to parse components
            std::vector<std::unique_ptr<ComponentSerializationData>> componentData;
            componentData.resize((uint32)eComponentId::COUNT);
            uint8 numComponents = 0;

            // Parse Actor prefab data into cooked struct
            if (actor.HasMember("Prefab"))
            {
                std::string prefabName = actor["Prefab"].GetString();
                printf("Parsing Actor Prefab: %s.\n", prefabName.data());

                // Parse the actor prefab json
                std::string inputPath = sGenerateInputFilePath(applicationRoot, prefabName.data(), ".json");
                std::string jsonStr;
                if (!sLoadFileToString(inputPath, jsonStr))
                {
                    printf("Failed to load json file to string from %s\n", inputPath.data());
                    return false;
                }
                printf("Successfully loaded json file string from %s\n", inputPath.data());

                rapidjson::Document actorJsonDoc;
                actorJsonDoc.Parse(jsonStr.data());

                if (actorJsonDoc.HasParseError())
                {
                    sPrintParseError(actorJsonDoc.GetParseError());
                    return false;
                }

                // Parse prefab components
                if (actorJsonDoc.HasMember("Components"))
                {
                    const rapidjson::Value& components = actorJsonDoc["Components"];
                    sParseComponents(components, componentData, numComponents);
                }
            }

            // Overwirte prefab component data with scene instance data
            if (actor.HasMember("Components"))
            {
                const rapidjson::Value& components = actor["Components"];
                sParseComponents(components, componentData, numComponents);
            }

            // Serialize components
            out.write(reinterpret_cast<const char*>(&numComponents), sizeof(numComponents));
            printf("Serializing %i Components.\n", numComponents);

            for (std::unique_ptr<ComponentSerializationData>& data : componentData)
            {
                if (data.get() != nullptr)
                {
                    eComponentId componentId = data->componentId;
                    out.write(reinterpret_cast<const char*>(&componentId), sizeof(componentId));

                    printf("Serializing Component ID: %i.\n", (uint32)componentId);

                    switch (componentId)
                    {
                    case eComponentId::COMPONENT_TRANSFORM:
                    {
                        out.write(reinterpret_cast<const char*>(data.get()), sizeof(TransformSerializationData));
                        printf("Successfully serialized Component: %i.\n", (uint32)componentId);
                        break;
                    }
                    case eComponentId::COMPONENT_SPRITE:
                    {
                        SpriteSerializationData* spriteData = static_cast<SpriteSerializationData*>(data.get());

                        // Cook material
                        std::string materialOutputPath = sGenerateOutputFilePath(applicationRoot, spriteData->material.data());
                        path::WritePath(materialOutputPath, out);
                        if (!ConvertJsonToBinary(eJsonType::MATERIAL, applicationRoot, spriteData->material.data()))
                        {
                            printf("Failed to serialize Material for Sprite Component: %s.\n", spriteData->material.data());
                            return false;
                        }
                        printf("Successfully serialized Material for Sprite Component: %s.\n", spriteData->material.data());

                        // Atlas
                        out.write(reinterpret_cast<const char*>(&spriteData->usingAtlas), sizeof(spriteData->usingAtlas));
                        printf("Serializing Atlas: %s.\n", spriteData->usingAtlas ? "True" : "False");
                        if (spriteData->usingAtlas)
                        {
                            // Cook Atlas
                            std::string atlasOutputPath = sGenerateOutputFilePath(applicationRoot, spriteData->atlas.data());
                            path::WritePath(atlasOutputPath, out);
                            if (!ConvertJsonToBinary(eJsonType::ATLAS, applicationRoot, spriteData->atlas.data()))
                            {
                                printf("Failed to serialize Atlas for Sprite Component: %s.\n", spriteData->atlas.data());
                                return false;
                            }
                            printf("Successfully serialized Atlas for Sprite Component: %s.\n", spriteData->atlas.data());

                            // Region
                            path::WritePath(spriteData->region, out);
                        }

                        out.write(reinterpret_cast<const char*>(&spriteData->pixlesPerUnit), sizeof(spriteData->pixlesPerUnit));
                        out.write(reinterpret_cast<const char*>(&spriteData->color), sizeof(spriteData->color));
                        out.write(reinterpret_cast<const char*>(&spriteData->layer), sizeof(spriteData->layer));
                        out.write(reinterpret_cast<const char*>(&spriteData->order), sizeof(spriteData->order));
                        printf("Successfully serialized Component: %i.\n", (uint32)componentId);
                        break;
                    }
                    case eComponentId::COMPONENT_SPRITE_ANIMATOR:
                    {
                        SpriteAnimatorSerializationData* spriteAnimatorData = static_cast<SpriteAnimatorSerializationData*>(data.get());
                        out.write(reinterpret_cast<const char*>(&spriteAnimatorData->numClips), sizeof(spriteAnimatorData->numClips));
                        for (const SpriteAnimationClip& animClip : spriteAnimatorData->animationClips)
                        {
                            path::WritePath(animClip.name, out);
                            out.write(reinterpret_cast<const char*>(&animClip.fps), sizeof(animClip.fps));
                            out.write(reinterpret_cast<const char*>(&animClip.frames), sizeof(animClip.frames));
                            out.write(reinterpret_cast<const char*>(&animClip.loop), sizeof(animClip.loop));
                        }
                        printf("Successfully serialized Component: %i.\n", (uint32)componentId);
                        break;
                    }
                    case eComponentId::COMPONENT_CAMERA:
                    {
                        out.write(reinterpret_cast<const char*>(data.get()), sizeof(CameraSerializationData));
                        printf("Successfully serialized Component: %i.\n", (uint32)componentId);
                        break;
                    }
                    case eComponentId::COMPONENT_MOVEMENT:
                    {
                        out.write(reinterpret_cast<const char*>(data.get()), sizeof(MovementSerializationData));
                        printf("Successfully serialized Component: %i.\n", (uint32)componentId);
                        break;
                    }
                    case eComponentId::COMPONENT_COLLIDER:
                    {
                        out.write(reinterpret_cast<const char*>(data.get()), sizeof(ColliderSerializationData));
                        printf("Successfully serialized Component: %i.\n", (uint32)componentId);
                        break;
                    }
                    default:
                    {
                        printf("Cannot serialiaze unsupported component type: %i\n", (uint32)componentId);
                        break;
                    }
                    }
                }
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

    // Sprite Sheet / Texture Layout
    const char* spriteSheetName = "SpriteSheet";
    if (!sValidateJsonValueMember(jsonDoc, spriteSheetName))
    {
        return false;
    }
    const char* numRowsName = "NumRows";
    if (!sValidateJsonValueMember(jsonDoc[spriteSheetName], numRowsName))
    {
        return false;
    }
    const char* numColumnsName = "NumColumns";
    if (!sValidateJsonValueMember(jsonDoc[spriteSheetName], numColumnsName))
    {
        return false;
    }
    uint32 numRows = jsonDoc[spriteSheetName][numRowsName].GetUint();
    uint32 numColumns = jsonDoc[spriteSheetName][numColumnsName].GetUint();
    out.write(reinterpret_cast<const char*>(&numRows), sizeof(numRows));
    out.write(reinterpret_cast<const char*>(&numColumns), sizeof(numColumns));

    printf("Successfully Serialized Material Json.\n");
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

bool static sConvertAtlasJsonToBinary(const char* applicationRoot, rapidjson::Document& jsonDoc, std::ofstream& out)
{
    const char* framesMemberName = "frames";
    if (!sValidateJsonValueMember(jsonDoc, framesMemberName))
    {
        return false;
    }

    const rapidjson::Value& frames = jsonDoc[framesMemberName];

    if (!frames.IsArray())
    {
        printf("Error: Atlas frames is not an array.\n");
        return false;
    }

    uint32 numFrames = (uint32)frames.Size();
    printf("Atlas has %i frames.\n", numFrames);
    out.write(reinterpret_cast<const char*>(&numFrames), sizeof(numFrames));
    for (const rapidjson::Value& frame : frames.GetArray())
    {
        // Write frame key
        const char* frameMemberName = "filename";
        if (!sValidateJsonValueMember(frame, frameMemberName))
        {
            return false;
        }
        std::string frameName = frame[frameMemberName].GetString();
        path::WritePath(frameName, out);

        // Write frame data - x, y, width, height
        const char* frameDataMemberName = "frame";
        if (!sValidateJsonValueMember(frame, frameDataMemberName))
        {
            return false;
        }

        const char* xName = "x";
        const char* yName = "y";
        const char* wName = "w";
        const char* hName = "h";

        if (!sValidateJsonValueMember(frame[frameDataMemberName], xName) ||
            !sValidateJsonValueMember(frame[frameDataMemberName], yName) ||
            !sValidateJsonValueMember(frame[frameDataMemberName], wName) ||
            !sValidateJsonValueMember(frame[frameDataMemberName], hName))
        {
            return false;
        }

        uint32 x = frame[frameDataMemberName][xName].GetUint();
        uint32 y = frame[frameDataMemberName][yName].GetUint();
        uint32 w = frame[frameDataMemberName][wName].GetUint();
        uint32 h = frame[frameDataMemberName][hName].GetUint();
        out.write(reinterpret_cast<const char*>(&x), sizeof(x));
        out.write(reinterpret_cast<const char*>(&y), sizeof(y));
        out.write(reinterpret_cast<const char*>(&w), sizeof(w));
        out.write(reinterpret_cast<const char*>(&h), sizeof(h));
    }
    printf("Successfully parsed Atlas.\n");
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
    case eJsonType::ATLAS:
    {
        success = sConvertAtlasJsonToBinary(applicationRoot, jsonDoc, out);
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
