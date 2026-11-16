#include "NoodlePch.h"
#include "SpriteAnimator.h"

#include "Actor/Actor.h"
#include "Actor/Components/Sprite.h"
#include "Resource/ResourceTypes.h"

#include <fstream>

const eComponentId SpriteAnimator::COMPONENT_ID = eComponentId::COMPONENT_SPRITE_ANIMATOR;

void SpriteAnimator::Update(float32 deltaSeconds)
{
	if (m_IsFinished)
	{
		return;
	}

	m_FrameTime += deltaSeconds;
	const SpriteAnimationClip& currentClip = m_AnimationClipMap[m_CurrentClipKey];
	N_ASSERT(currentClip.fps > 0, "Animation Clip FPS cannot be 0.");
	const float32 frameDuration = 1.0f / currentClip.fps;

	while (m_FrameTime >= frameDuration)
	{
		m_FrameTime -= frameDuration;
		AdvanceFrame(currentClip);
	}
	
	Sprite* sprite = m_Owner->GetComponent<Sprite>();
	N_ASSERT(sprite != nullptr, "Actor is missing Sprite component.");
	std::shared_ptr<Material> material = sprite->GetMaterial().lock();
	if (material)
	{
		const TextureLayout& textureLayout = material->diffuseTextureLayout;
		vec4 uvRect;

		uint32 columnIndex = m_CurrentFrameIndex % textureLayout.numColumns;
		uint32 rowIndex = m_CurrentFrameIndex / textureLayout.numColumns;

		// TODO: Need to account for multi row sprite sheets
		uvRect.xMin = (float32)(columnIndex * textureLayout.tileWidth) / (float32)material->diffuseTexture->width;
		uvRect.yMin = (float32)(rowIndex * textureLayout.tileHeight) / (float32)material->diffuseTexture->height;
		uvRect.xMax = ((float32)(columnIndex * textureLayout.tileWidth) + (float32)textureLayout.tileWidth) / (float32)material->diffuseTexture->width;
		uvRect.yMax = ((float32)(rowIndex * textureLayout.tileHeight) + (float32)textureLayout.tileHeight) / (float32)material->diffuseTexture->height;

		sprite->SetUvRect(uvRect);
	}
}

bool SpriteAnimator::SetClipKey(const std::string& clipName)
{
	if (m_CurrentClipKey == clipName)
	{
		return false;
	}
	PlayClip(clipName);
	return true;
}

void SpriteAnimator::PlayClip(const std::string& clipName)
{
	m_CurrentClipKey = clipName;
	m_FrameTime = 0.0f;
	m_CurrentFrameIndex = m_AnimationClipMap[clipName].startFrame;
	m_IsFinished = false;
}

void SpriteAnimator::Stop()
{
	m_FrameTime = 0.0f;
	m_IsFinished = true;
}

bool SpriteAnimator::Init(std::ifstream& bin, EngineContext& engineContext)
{
	uint32 numClips = 0;
	std::string defaultClip = path::ReadFilePath(bin);
	bin.read(reinterpret_cast<char*>(&numClips), sizeof(numClips));

	for (uint32 i = 0; i < numClips; ++i)
	{
		SpriteAnimationClip clip;
		clip.name = path::ReadFilePath(bin);

		bin.read(reinterpret_cast<char*>(&clip.fps), sizeof(clip.fps));
		bin.read(reinterpret_cast<char*>(&clip.startFrame), sizeof(clip.startFrame));
		bin.read(reinterpret_cast<char*>(&clip.endFrame), sizeof(clip.endFrame));
		bin.read(reinterpret_cast<char*>(&clip.loop), sizeof(clip.loop));

		m_AnimationClipMap.insert({ clip.name, clip });
	}
	N_ASSERT(m_AnimationClipMap.contains(defaultClip), "Default Sprite Animation Clip is invalid.");
	PlayClip(defaultClip);

	return true;
}

void SpriteAnimator::AdvanceFrame(const SpriteAnimationClip& clip)
{
	if (m_CurrentFrameIndex + 1 < clip.endFrame)
	{
		++m_CurrentFrameIndex;
	}
	else if (clip.loop)
	{
		m_CurrentFrameIndex = clip.startFrame;
	}
	else
	{
		m_CurrentFrameIndex = clip.endFrame - 1;
		m_IsFinished = true;
	}
}
