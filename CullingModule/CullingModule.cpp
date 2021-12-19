#include "CullingModule.h"

void culling::CullingModule::SetViewProjectionMatrix(const size_t cameraIndex, const Mat4x4& viewProjectionMatrix)
{
	assert(cameraIndex >= 0 && cameraIndex < MAX_CAMERA_COUNT);

	mCameraViewProjectionMatrixs[cameraIndex] = viewProjectionMatrix;
}

void culling::CullingModule::SetCameraWorldPosition(const size_t cameraIndex, const Vec3& cameraWorldPosition)
{
	assert(cameraIndex >= 0 && cameraIndex < MAX_CAMERA_COUNT);

	mCameraWorldPosition[cameraIndex] = cameraWorldPosition;
}

void culling::CullingModule::SetCameraCount(const size_t cameraCount)
{
	mCameraCount = cameraCount;
}
