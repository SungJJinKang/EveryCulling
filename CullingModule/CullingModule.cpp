#include "CullingModule.h"

void culling::CullingModule::SetViewProjectionMatrix(const unsigned int cameraIndex, const Matrix4X4& viewProjectionMatrix)
{
	assert(cameraIndex >= 0 && cameraIndex < MAX_CAMERA_COUNT);

	mCameraViewProjectionMatrixs[cameraIndex] = viewProjectionMatrix;
}
