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
	const size_t entityIndexInBlock
)
	: mTargetEntityBlock{ entityBlock }, mEntityIndexInBlock{ entityIndexInBlock }, bmIsActive{ true }
{}


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

