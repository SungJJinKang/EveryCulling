#pragma once

#include "MaskedSWOcclusionCullingStage.h"

#include "../SWDepthBuffer.h"

namespace culling
{
	class QueryOccludeeStage : public MaskedSWOcclusionCullingStage
	{
	
	private:

		FORCE_INLINE float MinFloatFromM256F(const culling::M256F& data);
		FORCE_INLINE float MaxFloatFromM256F(const culling::M256F& data);
		FORCE_INLINE void ComputeBinBoundingBoxFromVertex
		(
			const culling::M256F& screenPixelX,
			const culling::M256F& screenPixelY,
			std::uint32_t& outBinBoundingBoxMinX,
			std::uint32_t& outBinBoundingBoxMinY,
			std::uint32_t& outBinBoundingBoxMaxX,
			std::uint32_t& outBinBoundingBoxMaxY,
			SWDepthBuffer& depthBuffer
		);
		FORCE_INLINE void ComputeBinBoundingBoxFromVertex
		(
			const float minScreenPixelX,
			const float minScreenPixelY,
			const float maxScreenPixelX,
			const float maxScreenPixelY,
			std::uint32_t& outBinBoundingBoxMinX,
			std::uint32_t& outBinBoundingBoxMinY,
			std::uint32_t& outBinBoundingBoxMaxX,
			std::uint32_t& outBinBoundingBoxMaxY,
			SWDepthBuffer& depthBuffer
		);
		FORCE_INLINE void Clipping
		(
			const culling::M256F& clipspaceVertexX,
			const culling::M256F& clipspaceVertexY,
			const culling::M256F& clipspaceVertexZ,
			const culling::M256F& clipspaceVertexW,
			std::uint32_t& triangleCullMask
		);

		void QueryOccludee(const size_t cameraIndex, culling::EntityBlock* const entityBlock);

	public:

		QueryOccludeeStage(MaskedSWOcclusionCulling* mOcclusionCulling);

		void CullBlockEntityJob(const size_t cameraIndex) override;
		const char* GetCullingModuleName() const override;
	};
}


