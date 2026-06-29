#include "NoodlePch.h"
#include "Camera.h"
#include <fstream>

const eComponentId CameraInterface::COMPONENT_ID = eComponentId::COMPONENT_CAMERA;

void Camera::Update(float deltaSeconds)
{
}

bool Camera::Init(std::ifstream& bin, EngineContext& engineContext)
{
	bin.read(reinterpret_cast<char*>(&m_ProjectionType), sizeof(eProjectionType));
	bin.read(reinterpret_cast<char*>(&m_NearPlane), sizeof(float32));
	bin.read(reinterpret_cast<char*>(&m_FarPlane), sizeof(float32));
	bin.read(reinterpret_cast<char*>(&m_OrthoHeight), sizeof(float32));
	bin.read(reinterpret_cast<char*>(&m_ClearColor), sizeof(vec4));
	return true;
}
