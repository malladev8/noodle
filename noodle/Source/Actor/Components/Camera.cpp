#include "NoodlePch.h"
#include "Camera.h"
#include "Actor/Actor.h"
#include "Actor/Components/Transform.h"
#include <fstream>

const eComponentId CameraInterface::COMPONENT_ID = eComponentId::COMPONENT_CAMERA;

// Standard Orthographic Projcetion
static mat4x4 sGetOrthographicLH(float32 width, float32 height, float32 nearPlane, float32 farPlane)
{
	mat4x4 m = mat4x4::Identity();

	m.m[0][0] = 2.0f / width;
	m.m[1][1] = 2.0f / height;
	m.m[2][2] = 1.0f / (farPlane - nearPlane);
	m.m[3][2] = -nearPlane / (farPlane - nearPlane);

	//m.m[0][0] = 1.0f / (farPlane - nearPlane); 
	//m.m[1][1] = 2.0f / width; 
	//m.m[2][2] = 2.0f / height;
	//m.m[3][0] = -nearPlane / (farPlane - nearPlane);

	return m;
}

void Camera::Update(float deltaSeconds)
{
}

CameraData Camera::BuildCameraData(float32 aspectRatio) const
{
	CameraData camData;
	camData.view = m_Owner->GetComponent<Transform>()->GetWorldMatrix().InverseTransform();

	float32 orthoWidth = m_OrthoHeight * aspectRatio;
	camData.projection = sGetOrthographicLH(orthoWidth, m_OrthoHeight, m_NearPlane, m_FarPlane);

	camData.clearColor = m_ClearColor;

	return camData;
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
