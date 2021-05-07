#include "ViewFrustumCulling.h"

#include <cassert>

#include "../../DataType/Math/Math.h"
#include "../../FrotbiteCullingSystem.h"


void culling::ViewFrustumCulling::UpdateFrustumPlane(unsigned int frustumPlaneIndex, const Matrix4X4& viewProjectionMatrix)
{
	assert(frustumPlaneIndex >= 0 && frustumPlaneIndex < MAX_CAMERA_COUNT);

	ExtractSIMDPlanesFromViewProjectionMatrix(viewProjectionMatrix, this->mSIMDFrustumPlanes[frustumPlaneIndex].mFrustumPlanes, true);
}

