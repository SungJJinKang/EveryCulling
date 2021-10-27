#include "CullingModule.h"

void culling::CullingModule::SetViewProjectionMatrix(const std::uint32_t cameraIndex, const Mat4x4& viewProjectionMatrix)
{
	assert(cameraIndex >= 0 && cameraIndex < MAX_CAMERA_COUNT);

	mCameraViewProjectionMatrixs[cameraIndex] = viewProjectionMatrix;
}
