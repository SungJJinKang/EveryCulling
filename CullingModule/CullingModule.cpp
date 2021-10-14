#include "CullingModule.h"

void culling::CullingModule::SetViewProjectionMatrix(const UINT32 cameraIndex, const Mat4x4& viewProjectionMatrix)
{
	assert(cameraIndex >= 0 && cameraIndex < MAX_CAMERA_COUNT);

	mCameraViewProjectionMatrixs[cameraIndex] = viewProjectionMatrix;
}
