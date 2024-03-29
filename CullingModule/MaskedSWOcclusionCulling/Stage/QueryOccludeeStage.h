#pragma once

#include "MaskedSWOcclusionCullingStage.h"

#include "../SWDepthBuffer.h"

namespace culling
{
	class QueryOccludeeStage : public MaskedSWOcclusionCullingStage
	{
	
	private:

		EVERYCULLING_FORCE_INLINE float MinFloatFromM256F(const culling::EVERYCULLING_M256F& data);
		EVERYCULLING_FORCE_INLINE float MaxFloatFromM256F(const culling::EVERYCULLING_M256F& data);
		EVERYCULLING_FORCE_INLINE void ComputeBinBoundingBoxFromVertex
		(
			const culling::EVERYCULLING_M256F& screenPixelX,
			const culling::EVERYCULLING_M256F& screenPixelY,
			std::uint32_t& outBinBoundingBoxMinX,
			std::uint32_t& outBinBoundingBoxMinY,
			std::uint32_t& outBinBoundingBoxMaxX,
			std::uint32_t& outBinBoundingBoxMaxY,
			SWDepthBuffer& depthBuffer
		);
		EVERYCULLING_FORCE_INLINE void ComputeBinBoundingBoxFromVertex
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
		EVERYCULLING_FORCE_INLINE void Clipping
		(
			const culling::EVERYCULLING_M256F& clipspaceVertexX,
			const culling::EVERYCULLING_M256F& clipspaceVertexY,
			const culling::EVERYCULLING_M256F& clipspaceVertexZ,
			const culling::EVERYCULLING_M256F& clipspaceVertexW,
			std::uint32_t& triangleCullMask
		);

		void QueryOccludee(const size_t cameraIndex, culling::EntityBlock* const entityBlock);

	public:

		QueryOccludeeStage(MaskedSWOcclusionCulling* mOcclusionCulling);

		void CullBlockEntityJob(const size_t cameraIndex, const unsigned long long currentTickCount) override;
		const char* GetCullingModuleName() const override;
	};
}


