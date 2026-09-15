#include "NoodlePch.h"
#include "SpriteAnimator.h"
#include "Resource/ResourceTypes.h"
#include "Actor/Components/Sprite.h"
#include "Actor/Actor.h"
#include <fstream>

const eComponentId SpriteAnimatorInterface::COMPONENT_ID = eComponentId::COMPONENT_SPRITE_ANIMATOR;

void SpriteAnimator::Update(float32 deltaSeconds)
{
	m_FrameTime += deltaSeconds;
	const SpriteAnimationClip& currentClip = m_AnimationClipMap[m_CurrentClipKey];
	if (m_FrameTime >= (1.0f / (float32)currentClip.fps))
	{
		m_FrameTime = 0.0f;
		m_CurrentFrameIndex = m_CurrentFrameIndex >= currentClip.frames ? 0 : m_CurrentFrameIndex + 1;
	}

	Sprite* sprite = m_Owner->GetComponent<Sprite>();
	N_ASSERT(sprite != nullptr, "Actor is missing Sprite component.");
	std::shared_ptr<Material> material = sprite->GetMaterial().lock();
	if (material)
	{
		const TextureLayout& textureLayout = material->diffuseTextureLayout;
		vec4 uvRect;

		// TODO: Need to account for multi row sprite sheets
		uvRect.x = (float32)(m_CurrentFrameIndex * textureLayout.tileWidth) / (float32)material->diffuseTexture->width;
		uvRect.y = 0.0f;
		uvRect.z = ((float32)(m_CurrentFrameIndex * textureLayout.tileWidth) + (float32)textureLayout.tileWidth) / (float32)material->diffuseTexture->width;
		uvRect.w = 1.0f;

		sprite->SetUvRect(uvRect);
	}
}

bool SpriteAnimator::Init(std::ifstream& bin, EngineContext& engineContext)
{
	uint32 numClips = 0;
	bin.read(reinterpret_cast<char*>(&numClips), sizeof(numClips));

	for (uint32 i = 0; i < numClips; ++i)
	{
		SpriteAnimationClip clip;
		uint32 clipNameLength = 0;
		bin.read(reinterpret_cast<char*>(&clipNameLength), sizeof(clipNameLength));
		clip.name.resize((size_t)clipNameLength);
		bin.read(reinterpret_cast<char*>(clip.name.data()), clipNameLength);

		bin.read(reinterpret_cast<char*>(&clip.fps), sizeof(clip.fps));
		bin.read(reinterpret_cast<char*>(&clip.frames), sizeof(clip.frames));
		bin.read(reinterpret_cast<char*>(&clip.loop), sizeof(clip.loop));

		m_AnimationClipMap.insert({ clip.name, clip });

		if (i == 0)
		{
			m_CurrentClipKey = clip.name;
		}
	}

	return true;
}
