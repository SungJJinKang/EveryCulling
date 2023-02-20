#include "EntityBlockViewer.h"

#include <cassert>


void culling::EntityBlockViewer::DeInitializeEntityBlockViewer()
{
	mTargetEntityBlock = nullptr;
	mEntityIndexInBlock = (std::uint64_t)-1;
}

void culling::EntityBlockViewer::ResetEntityData()
{
	SetIsObjectEnabled(true);
}

culling::EntityBlockViewer::EntityBlockViewer()
{
	DeInitializeEntityBlockViewer();
}

culling::EntityBlockViewer::EntityBlockViewer
(
	EntityBlock* const entityBlock, 
	const size_t entityIndexInBlock
)
	: mTargetEntityBlock{ entityBlock }, mEntityIndexInBlock{ entityIndexInBlock }
{
	assert(IsValid() == true);
	ResetEntityData();
}

culling::EntityBlockViewer::EntityBlockViewer(EntityBlockViewer&&) noexcept = default;
culling::EntityBlockViewer& culling::EntityBlockViewer::operator=(EntityBlockViewer&&) noexcept = default;

void culling::EntityBlockViewer::SetMeshVertexData
(
	const culling::Vec3* const vertices, 
	const std::uint64_t verticeCount,
	const std::uint32_t* const indices,
	const std::uint64_t indiceCount,
	const std::uint64_t verticeStride
)
{
	assert(IsValid() == true);
	if (IsValid() == true)
	{
		mTargetEntityBlock->mVertexDatas[mEntityIndexInBlock].mVertices = vertices;
		mTargetEntityBlock->mVertexDatas[mEntityIndexInBlock].mVerticeCount = verticeCount;
		mTargetEntityBlock->mVertexDatas[mEntityIndexInBlock].mIndices = indices;
		mTargetEntityBlock->mVertexDatas[mEntityIndexInBlock].mIndiceCount = indiceCount;
		mTargetEntityBlock->mVertexDatas[mEntityIndexInBlock].mVertexStride = verticeStride;
	}
}

