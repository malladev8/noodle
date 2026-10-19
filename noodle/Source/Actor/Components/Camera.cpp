#include "NoodlePch.h"
#include "Camera.h"
#include "Actor/Actor.h"
#include "Actor/Components/Transform.h"
#include "Resource/SerializationTypes.h"
#include <fstream>

const eComponentId Camera::COMPONENT_ID = eComponentId::COMPONENT_CAMERA;

// Standard Orthographic Projcetion
static mat4x4 sGetOrthographicLH(float32 width, float32 height, float32 nearPlane, float32 farPlane)
{
	mat4x4 m = mat4x4::Identity();

	m.m[0][0] = 2.0f / width;
	m.m[1][1] = 2.0f / height;
	m.m[2][2] = 1.0f / (farPlane - nearPlane);
	m.m[2][3] = -nearPlane / (farPlane - nearPlane);
	m.m[3][3] = 1.0f;

	return m;
}

void Camera::Update(float deltaSeconds)
{
}

CameraData Camera::BuildCameraData(float32 aspectRatio) const
{
	CameraData camData;

    const Transform& camTransform = *m_Owner->GetComponent<Transform>();
    camData.view = camTransform.GetWorldMatrix().InverseTransform();

    float32 orthoWidth = m_OrthoHeight * aspectRatio;
    camData.projection = sGetOrthographicLH(orthoWidth, m_OrthoHeight, m_NearPlane, m_FarPlane);

    camData.clearColor = m_ClearColor;

    return camData;
}

bool Camera::Init(std::ifstream& bin, EngineContext& engineContext)
{
	CameraSerializationData cameraData;
	bin.read(reinterpret_cast<char*>(&cameraData), sizeof(CameraSerializationData));
	m_ProjectionType = cameraData.projectionType;
	m_NearPlane = cameraData.nearPlane;
	m_FarPlane = cameraData.farPlane;
	m_OrthoHeight = cameraData.orthoHeight;
	m_ClearColor = cameraData.clearColor;
	return true;
}
