#include "ViewFrustumCulling.h"

#include <cassert>

#include "../../DataType/Math/Common.h"
#include "../../EveryCulling.h"


void culling::ViewFrustumCulling::SetViewProjectionMatrix(const unsigned int cameraIndex, const Matrix4X4& viewProjectionMatrix)
{
	culling::CullingModule::SetViewProjectionMatrix(cameraIndex, viewProjectionMatrix);

	assert(cameraIndex >= 0 && cameraIndex < MAX_CAMERA_COUNT);

	ExtractSIMDPlanesFromViewProjectionMatrix(viewProjectionMatrix, mSIMDFrustumPlanes[cameraIndex].mFrustumPlanes, true);
}

