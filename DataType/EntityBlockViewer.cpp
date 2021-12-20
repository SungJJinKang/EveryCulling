#include "EntityBlockViewer.h"

#include <cassert>
#include <cstring>


culling::EntityBlockViewer::EntityBlockViewer()
	: bmIsActive(false), mTargetEntityBlock(nullptr), mEntityIndexInBlock(0)
{
}

culling::EntityBlockViewer::EntityBlockViewer
(
	EntityBlock* const entityBlock, 
	const std::uint32_t entityIndexInBlock
)
	: mTargetEntityBlock{ entityBlock }, mEntityIndexInBlock{ entityIndexInBlock }, bmIsActive{ true }
{}

void culling::EntityBlockViewer::SetEntityPosition(const float* worldPosition)
{
	if (GetIsActive() == true)
	{
		assert(bmIsActive == true);
		std::memcpy((mTargetEntityBlock->mPositionAndBoundingSpheres + mEntityIndexInBlock), worldPosition, sizeof(Vec3));
	}

}

void culling::EntityBlockViewer::SetSphereBoundRadius(float sphereRadius)
{
	if (GetIsActive() == true)
	{
		assert(bmIsActive == true);
		assert(sphereRadius >= 0.0f);

		// WHY NEGATIVE??
		// Think Sphere is on(!!) frustum plane. But it still should be drawd
		// Distance from plane to EntityPoint is negative.
		// If Distance from plane to EntityPoint is larget than negative radius, it should be drawed
		mTargetEntityBlock->mPositionAndBoundingSpheres[mEntityIndexInBlock].SetBoundingSphereRadius(sphereRadius);
	}

}

void culling::EntityBlockViewer::SetMeshVertexData
(
	const culling::Vec3* const vertices, 
	const size_t verticeCount,
	const std::uint32_t* const indices,
	const size_t indiceCount,
	const size_t verticeStride
)
{
	if (GetIsActive() == true)
	{
		mTargetEntityBlock->mVertexDatas[mEntityIndexInBlock].mVertices = vertices;
		mTargetEntityBlock->mVertexDatas[mEntityIndexInBlock].mVerticeCount = verticeCount;
		mTargetEntityBlock->mVertexDatas[mEntityIndexInBlock].mIndices = indices;
		mTargetEntityBlock->mVertexDatas[mEntityIndexInBlock].mIndiceCount = indiceCount;
		mTargetEntityBlock->mVertexDatas[mEntityIndexInBlock].mVertexStride = verticeStride;
	}
}

void culling::EntityBlockViewer::SetModelMatrix(const float* const modelMatrix)
{
	if (GetIsActive() == true)
	{
		mTargetEntityBlock->mModelMatrixes[mEntityIndexInBlock] = modelMatrix;
	}
}

