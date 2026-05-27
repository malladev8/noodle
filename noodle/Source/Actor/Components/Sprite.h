#pragma once
#include "../../Core/NoodleCore.h"
#include "SpriteInterface.h"
#include "../../Resource/ResourceTypes.h"

class Sprite : public SpriteInterface
{
	friend class ActorFactory;

public:
protected:
private:
	std::shared_ptr<Texture> m_Texture;
	virtual bool Init(std::ifstream& bin);
};
