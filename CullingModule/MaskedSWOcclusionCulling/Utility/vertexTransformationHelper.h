#pragma once

#include "../../../EveryCullingCore.h"
#include "../SWDepthBuffer.h"

namespace culling
{
	namespace vertexTransformationHelper
	{
		FORCE_INLINE extern void TransformThreeVerticesToClipSpace
		(
			culling::M256F* outClipVertexX,
			culling::M256F* outClipVertexY,
			culling::M256F* outClipVertexZ,
			culling::M256F* outClipVertexW,
			const float* const toClipspaceMatrix
		)
		{
			assert(toClipspaceMatrix != nullptr);
			for (size_t i = 0; i < 3; ++i)
			{
				const culling::M256F tmpX = culling::M256F_MUL_AND_ADD(outClipVertexX[i], _mm256_set1_ps(toClipspaceMatrix[0]), culling::M256F_MUL_AND_ADD(outClipVertexY[i], _mm256_set1_ps(toClipspaceMatrix[4]), culling::M256F_MUL_AND_ADD(outClipVertexZ[i], _mm256_set1_ps(toClipspaceMatrix[8]), _mm256_set1_ps(toClipspaceMatrix[12]))));
				const culling::M256F tmpY = culling::M256F_MUL_AND_ADD(outClipVertexX[i], _mm256_set1_ps(toClipspaceMatrix[1]), culling::M256F_MUL_AND_ADD(outClipVertexY[i], _mm256_set1_ps(toClipspaceMatrix[5]), culling::M256F_MUL_AND_ADD(outClipVertexZ[i], _mm256_set1_ps(toClipspaceMatrix[9]), _mm256_set1_ps(toClipspaceMatrix[13]))));
				const culling::M256F tmpZ = culling::M256F_MUL_AND_ADD(outClipVertexX[i], _mm256_set1_ps(toClipspaceMatrix[2]), culling::M256F_MUL_AND_ADD(outClipVertexY[i], _mm256_set1_ps(toClipspaceMatrix[6]), culling::M256F_MUL_AND_ADD(outClipVertexZ[i], _mm256_set1_ps(toClipspaceMatrix[10]), _mm256_set1_ps(toClipspaceMatrix[14]))));
				const culling::M256F tmpW = culling::M256F_MUL_AND_ADD(outClipVertexX[i], _mm256_set1_ps(toClipspaceMatrix[3]), culling::M256F_MUL_AND_ADD(outClipVertexY[i], _mm256_set1_ps(toClipspaceMatrix[7]), culling::M256F_MUL_AND_ADD(outClipVertexZ[i], _mm256_set1_ps(toClipspaceMatrix[11]), _mm256_set1_ps(toClipspaceMatrix[15]))));
				
				outClipVertexX[i] = tmpX;
				outClipVertexY[i] = tmpY;
				outClipVertexZ[i] = tmpZ;
				outClipVertexW[i] = tmpW;

			}
		}

		FORCE_INLINE extern void TransformVertexToClipSpace
		(
			culling::M256F& outClipVertexX,
			culling::M256F& outClipVertexY,
			culling::M256F& outClipVertexZ,
			culling::M256F& outClipVertexW,
			const float* const toClipspaceMatrix
		)
		{
			assert(toClipspaceMatrix != nullptr);
			const culling::M256F tmpX = culling::M256F_MUL_AND_ADD(outClipVertexX, _mm256_set1_ps(toClipspaceMatrix[0]), culling::M256F_MUL_AND_ADD(outClipVertexY, _mm256_set1_ps(toClipspaceMatrix[4]), culling::M256F_MUL_AND_ADD(outClipVertexZ, _mm256_set1_ps(toClipspaceMatrix[8]), _mm256_set1_ps(toClipspaceMatrix[12]))));
			const culling::M256F tmpY = culling::M256F_MUL_AND_ADD(outClipVertexX, _mm256_set1_ps(toClipspaceMatrix[1]), culling::M256F_MUL_AND_ADD(outClipVertexY, _mm256_set1_ps(toClipspaceMatrix[5]), culling::M256F_MUL_AND_ADD(outClipVertexZ, _mm256_set1_ps(toClipspaceMatrix[9]), _mm256_set1_ps(toClipspaceMatrix[13]))));
			const culling::M256F tmpZ = culling::M256F_MUL_AND_ADD(outClipVertexX, _mm256_set1_ps(toClipspaceMatrix[2]), culling::M256F_MUL_AND_ADD(outClipVertexY, _mm256_set1_ps(toClipspaceMatrix[6]), culling::M256F_MUL_AND_ADD(outClipVertexZ, _mm256_set1_ps(toClipspaceMatrix[10]), _mm256_set1_ps(toClipspaceMatrix[14]))));
			const culling::M256F tmpW = culling::M256F_MUL_AND_ADD(outClipVertexX, _mm256_set1_ps(toClipspaceMatrix[3]), culling::M256F_MUL_AND_ADD(outClipVertexY, _mm256_set1_ps(toClipspaceMatrix[7]), culling::M256F_MUL_AND_ADD(outClipVertexZ, _mm256_set1_ps(toClipspaceMatrix[11]), _mm256_set1_ps(toClipspaceMatrix[15]))));

			outClipVertexX = tmpX;
			outClipVertexY = tmpY;
			outClipVertexZ = tmpZ;
			outClipVertexW = tmpW;
		}

		FORCE_INLINE extern void ConvertClipSpaceThreeVerticesToNDCSpace
		(
			culling::M256F* outClipVertexX,
			culling::M256F* outClipVertexY,
			culling::M256F* outClipVertexZ,
			const culling::M256F* oneDividedByW
		)
		{
			for (size_t i = 0; i < 3; i++)
			{
				//Why Do This??
				//compute 1/w in advance 

				outClipVertexX[i] = culling::M256F_MUL(outClipVertexX[i], oneDividedByW[i]);
				outClipVertexY[i] = culling::M256F_MUL(outClipVertexY[i], oneDividedByW[i]);
				outClipVertexZ[i] = culling::M256F_MUL(outClipVertexZ[i], oneDividedByW[i]);

				//This code is useless
				//outClipVertexW[i] = culling::M256F_MUL(outClipVertexW[i], outClipVertexW[i]);
			}
		}

		FORCE_INLINE extern void ConvertClipSpaceVertexToNDCSpace
		(
			culling::M256F& outClipVertexX,
			culling::M256F& outClipVertexY,
			culling::M256F& outClipVertexZ,
			const culling::M256F& oneDividedByW
		)
		{
			outClipVertexX = culling::M256F_MUL(outClipVertexX, oneDividedByW);
			outClipVertexY = culling::M256F_MUL(outClipVertexY, oneDividedByW);
			outClipVertexZ = culling::M256F_MUL(outClipVertexZ, oneDividedByW);
		}


		FORCE_INLINE extern void ConvertNDCSpaceThreeVerticesToScreenPixelSpace
		(
			const culling::M256F* ndcSpaceVertexX,
			const culling::M256F* ndcSpaceVertexY,
			culling::M256F* outScreenPixelSpaceX,
			culling::M256F* outScreenPixelSpaceY,
			culling::SWDepthBuffer& depthBuffer
		)
		{
			for (size_t i = 0; i < 3; i++)
			{
				//Convert NDC Space Coordinates To Screen Space Coordinates 
#if NDC_RANGE == MINUS_ONE_TO_POSITIVE_ONE

				outScreenPixelSpaceX[i] = culling::M256F_MUL(culling::M256F_ADD(ndcSpaceVertexX[i], _mm256_set1_ps(1.0f)), depthBuffer.mResolution.mReplicatedScreenHalfWidth);
				outScreenPixelSpaceX[i] = _mm256_ceil_ps(outScreenPixelSpaceX[i]);

				outScreenPixelSpaceY[i] = culling::M256F_MUL(culling::M256F_ADD(ndcSpaceVertexY[i], _mm256_set1_ps(1.0f)), depthBuffer.mResolution.mReplicatedScreenHalfHeight);
				outScreenPixelSpaceY[i] = _mm256_floor_ps(outScreenPixelSpaceY[i]);

#elif NDC_RANGE == ZERO_TO_POSITIVE_ONE

				outScreenPixelSpaceX[i] = culling::M256F_MUL(ndcSpaceVertexX[i], mDepthBuffer.mResolution.mReplicatedScreenWidth);
				outScreenPixelSpaceX[i] = _mm256_ceil_ps(outScreenPixelSpaceX[i]);

				outScreenPixelSpaceY[i] = culling::M256F_MUL(ndcSpaceVertexY[i], mDepthBuffer.mResolution.mReplicatedScreenHeight);
				outScreenPixelSpaceY[i] = _mm256_floor_ps(outScreenPixelSpaceY[i]);

#else 
				assert(0); //NEVER HAPPEN
#endif
			}
		}

		FORCE_INLINE extern void ConvertClipSpaceThreeVerticesToScreenPixelSpace
		(
			const culling::M256F* clipSpaceVertexX,
			const culling::M256F* clipSpaceVertexY,
			const culling::M256F* clipSpaceVertexReverseW,
			culling::M256F* outScreenPixelSpaceX,
			culling::M256F* outScreenPixelSpaceY,
			culling::SWDepthBuffer& depthBuffer
		)
		{
			for (size_t i = 0; i < 3; i++)
			{
				//Convert NDC Space Coordinates To Screen Space Coordinates 
#if NDC_RANGE == MINUS_ONE_TO_POSITIVE_ONE

				const culling::M256F ndcSpaceVertexX = culling::M256F_MUL(clipSpaceVertexX[i], clipSpaceVertexReverseW[i]);
				outScreenPixelSpaceX[i] = culling::M256F_MUL(culling::M256F_MUL(culling::M256F_ADD(ndcSpaceVertexX, _mm256_set1_ps(1.0f)), _mm256_set1_ps(0.5f)), depthBuffer.mResolution.mReplicatedScreenWidth);
				outScreenPixelSpaceX[i] = _mm256_ceil_ps(outScreenPixelSpaceX[i]);

				const culling::M256F ndcSpaceVertexY = culling::M256F_MUL(clipSpaceVertexY[i], clipSpaceVertexReverseW[i]);
				outScreenPixelSpaceY[i] = culling::M256F_MUL(culling::M256F_MUL(culling::M256F_ADD(ndcSpaceVertexY, _mm256_set1_ps(1.0f)), _mm256_set1_ps(0.5f)), depthBuffer.mResolution.mReplicatedScreenHeight);
				outScreenPixelSpaceY[i] = _mm256_floor_ps(outScreenPixelSpaceY[i]);

#elif NDC_RANGE == ZERO_TO_POSITIVE_ONE

				outScreenPixelSpaceX[i] = culling::M256F_MUL(ndcSpaceVertexX[i], mDepthBuffer.mResolution.mReplicatedScreenWidth);
				outScreenPixelSpaceX[i] = _mm256_ceil_ps(outScreenPixelSpaceX[i]);

				outScreenPixelSpaceY[i] = culling::M256F_MUL(ndcSpaceVertexY[i], mDepthBuffer.mResolution.mReplicatedScreenHeight);
				outScreenPixelSpaceY[i] = _mm256_floor_ps(outScreenPixelSpaceY[i]);

#else 
				assert(0); //NEVER HAPPEN
#endif
			}
		}

		FORCE_INLINE extern void ConvertNDCSpaceVertexToScreenPixelSpace
		(
			const culling::M256F& ndcSpaceVertexX,
			const culling::M256F& ndcSpaceVertexY,
			culling::M256F& outScreenPixelSpaceX,
			culling::M256F& outScreenPixelSpaceY,
			culling::SWDepthBuffer& depthBuffer
		)
		{
			//Convert NDC Space Coordinates To Screen Space Coordinates 
#if NDC_RANGE == MINUS_ONE_TO_POSITIVE_ONE
			outScreenPixelSpaceX = culling::M256F_MUL(culling::M256F_ADD(ndcSpaceVertexX, _mm256_set1_ps(1.0f)), depthBuffer.mResolution.mReplicatedScreenHalfWidth);
			outScreenPixelSpaceY = culling::M256F_MUL(culling::M256F_ADD(ndcSpaceVertexY, _mm256_set1_ps(1.0f)), depthBuffer.mResolution.mReplicatedScreenHalfHeight);
#elif NDC_RANGE == ZERO_TO_POSITIVE_ONE
			outScreenPixelSpaceX = culling::M256F_MUL(ndcSpaceVertexX, mDepthBuffer.mResolution.mReplicatedScreenWidth);
			outScreenPixelSpaceY = culling::M256F_MUL(ndcSpaceVertexY, mDepthBuffer.mResolution.mReplicatedScreenHeight);
#else 
			assert(0); //NEVER HAPPEN
#endif

		}
	}

}


