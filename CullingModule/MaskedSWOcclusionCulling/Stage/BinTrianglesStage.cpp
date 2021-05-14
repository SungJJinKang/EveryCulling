#include "BinTrianglesStage.h"
#include "../MaskedSWOcclusionCulling.h"

#include "../SWDepthBuffer.h"

void culling::BinTrianglesStage::ConverClipSpaceToNDCSpace
(
	M256F* outClipVertexX, M256F* outClipVertexY, M256F* outClipVertexZ, const M256F* oneDividedByW, unsigned int& triangleCullMask
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

void culling::BinTrianglesStage::ConvertNDCSpaceToScreenPixelSpace
(
	const M256F* ndcSpaceVertexX, const M256F* ndcSpaceVertexY,
	M256F* outScreenPixelSpaceX, M256F* outScreenPixelSpaceY, unsigned int& triangleCullMask
)
{
	for (size_t i = 0; i < 3; i++)
	{
		//Convert NDC Space Coordinates To Screen Space Coordinates 
#if NDC_RANGE == MINUS_ONE_TO_POSITIVE_ONE
		const M256F tmpScreenSpaceX = culling::M256F_MUL_AND_ADD(ndcSpaceVertexX[i], this->mMaskedOcclusionCulling.mDepthBuffer.mResolution.mReplicatedScreenHalfWidth, this->mMaskedOcclusionCulling.mDepthBuffer.mResolution.mReplicatedScreenHalfWidth);
		const M256F tmpScreenSpaceY = culling::M256F_MUL_AND_ADD(ndcSpaceVertexY[i], this->mMaskedOcclusionCulling.mDepthBuffer.mResolution.mReplicatedScreenHalfHeight, this->mMaskedOcclusionCulling.mDepthBuffer.mResolution.mReplicatedScreenHalfHeight);
#elif NDC_RANGE == ZERO_TO_POSITIVE_ONE
		const M256F tmpScreenSpaceX = culling::M256F_MUL(ndcSpaceVertexX[i], this->mDepthBuffer.mResolution.mReplicatedScreenWidth);
		const M256F tmpScreenSpaceY = culling::M256F_MUL(ndcSpaceVertexY[i], this->mDepthBuffer.mResolution.mReplicatedScreenHeight);
#else 
		assert(0); //NEVER HAPPEN
#endif

		//Snap Screen Space Coordinates To Integer Coordinate In ScreenBuffer(or DepthBuffer)
		
		//A grid square, including its (x, y) window coordinates, z (depth), and associated data which may be added by fragment shaders, is called a fragment. A
		//fragment is located by its lower left corner, which lies on integer grid coordinates.
		//Rasterization operations also refer to a fragment¡¯s center, which is offset by ( 1/2, 1/2 )
		//from its lower left corner(and so lies on half - integer coordinates).

		//outScreenPixelSpaceX[i] = _mm256_floor_ps(tmpScreenSpaceX);
		//outScreenPixelSpaceY[i] = _mm256_floor_ps(tmpScreenSpaceY);


	}

}

void culling::BinTrianglesStage::TransformVertexsToClipSpace
(
	M256F* outClipVertexX, M256F* outClipVertexY, M256F* outClipVertexZ, M256F* outClipVertexW, 
	const float* toClipspaceMatrix, unsigned int& triangleCullMask
)
{
	if (toClipspaceMatrix != nullptr)
	{
		//Actually triangleCullMask should be considered.
		//But Just Compute it all.
		//TODO : Consider Trimask
		for (size_t i = 0; i < 3; ++i)
		{
			const M256F tmpX = culling::M256F_MUL_AND_ADD(outClipVertexX[i], _mm256_set1_ps(toClipspaceMatrix[0]), culling::M256F_MUL_AND_ADD(outClipVertexY[i], _mm256_set1_ps(toClipspaceMatrix[4]), culling::M256F_MUL_AND_ADD(outClipVertexW[i], _mm256_set1_ps(toClipspaceMatrix[8]), _mm256_set1_ps(toClipspaceMatrix[12]))));
			const M256F tmpY = culling::M256F_MUL_AND_ADD(outClipVertexX[i], _mm256_set1_ps(toClipspaceMatrix[1]), culling::M256F_MUL_AND_ADD(outClipVertexY[i], _mm256_set1_ps(toClipspaceMatrix[5]), culling::M256F_MUL_AND_ADD(outClipVertexW[i], _mm256_set1_ps(toClipspaceMatrix[9]), _mm256_set1_ps(toClipspaceMatrix[13]))));
			const M256F tmpZ = culling::M256F_MUL_AND_ADD(outClipVertexX[i], _mm256_set1_ps(toClipspaceMatrix[2]), culling::M256F_MUL_AND_ADD(outClipVertexY[i], _mm256_set1_ps(toClipspaceMatrix[6]), culling::M256F_MUL_AND_ADD(outClipVertexW[i], _mm256_set1_ps(toClipspaceMatrix[10]), _mm256_set1_ps(toClipspaceMatrix[14]))));
			const M256F tmpW = culling::M256F_MUL_AND_ADD(outClipVertexX[i], _mm256_set1_ps(toClipspaceMatrix[3]), culling::M256F_MUL_AND_ADD(outClipVertexY[i], _mm256_set1_ps(toClipspaceMatrix[7]), culling::M256F_MUL_AND_ADD(outClipVertexW[i], _mm256_set1_ps(toClipspaceMatrix[11]), _mm256_set1_ps(toClipspaceMatrix[15]))));

			outClipVertexX[i] = tmpX;
			outClipVertexY[i] = tmpY;
			outClipVertexZ[i] = tmpZ;
			outClipVertexW[i] = tmpW;

		}
	}
}

void culling::BinTrianglesStage::CullBackfaces(const M256F* screenPixelX, const M256F* screenPixelY, unsigned int& triangleCullMask)
{
	//I don't know How this Works.........
		//https://stackoverflow.com/questions/67357115/i-found-back-face-culling-code-but-i-cant-know-how-this-works
	const M256F triArea1 = culling::M256F_MUL(culling::M256F_SUB(screenPixelX[1], screenPixelX[0]), culling::M256F_SUB(screenPixelY[2], screenPixelY[0]));
	const M256F triArea2 = culling::M256F_MUL(culling::M256F_SUB(screenPixelX[0], screenPixelX[2]), culling::M256F_SUB(screenPixelY[0], screenPixelY[1]));
	const M256F triArea = culling::M256F_SUB(triArea1, triArea2);

	//_CMP_GT_OQ vs _CMP_GT_OQ : https://stackoverflow.com/questions/16988199/how-to-choose-avx-compare-predicate-variants
	const M256F ccwMask = _mm256_cmp_ps(triArea, _mm256_set1_ps(0.0f), _CMP_GT_OQ);

	//Set each bit of mask dst based on the most significant bit of the corresponding packed single-precision (32-bit) floating-point element in a.
	//https://software.intel.com/sites/landingpage/IntrinsicsGuide/#techs=SSE,SSE2,SSE3,SSSE3,SSE4_1,SSE4_2,AVX&expand=2156,4979,4979,1731,4929,951,4979,3869&text=movemask
	//if second triangle is front facing, low second bit of triangleCullMask is 1
	triangleCullMask = static_cast<unsigned int>(_mm256_movemask_ps(ccwMask));
}

void culling::BinTrianglesStage::ComputeBinBoundingBox(const M256F* screenPixelX, const M256F* screenPixelY, M256I& outBinBoundingBoxMinX, M256I& outBinBoundingBoxMinY, M256I& outBinBoundingBoxMaxX, M256I& outBinBoundingBoxMaxY)
{
	M256I minScreenPixelX, minScreenPixelY, maxScreenPixelX, maxScreenPixelY;

	//A grid square, including its (x, y) window coordinates, z (depth), and associated data which may be added by fragment shaders, is called a fragment. A
	//fragment is located by its lower left corner, which lies on integer grid coordinates.
	//Rasterization operations also refer to a fragment¡¯s center, which is offset by ( 1/2, 1/2 )
	//from its lower left corner(and so lies on half - integer coordinates).

	static const M256I WIDTH_MASK = _mm256_set1_epi32(~(TILE_WIDTH - 1));
	static const M256I HEIGHT_MASK = _mm256_set1_epi32(~(TILE_HEIGHT - 1));

	minScreenPixelX = _mm256_cvttps_epi32( _mm256_floor_ps(_mm256_min_ps(screenPixelX[0], _mm256_min_ps(screenPixelX[1], screenPixelX[2]))) );
	minScreenPixelY = _mm256_cvttps_epi32( _mm256_floor_ps(_mm256_min_ps(screenPixelY[0], _mm256_min_ps(screenPixelY[1], screenPixelY[2]))) );
	maxScreenPixelX = _mm256_cvttps_epi32( _mm256_floor_ps(_mm256_max_ps(screenPixelX[0], _mm256_max_ps(screenPixelX[1], screenPixelX[2]))) );
	maxScreenPixelY = _mm256_cvttps_epi32( _mm256_floor_ps(_mm256_max_ps(screenPixelY[0], _mm256_max_ps(screenPixelY[1], screenPixelY[2]))) );

	// How "and" works?
	// 0000 0000 0110 0011 <- 96 = 32 * 3 + 3
	//		   AND
	// 1111 1111 1110 0000 <- WIDTH_MASK
	//
	// 0000 0000 0110 0000 <- 92 = 32 * 3
	//		

	outBinBoundingBoxMinX = _mm256_and_si256(minScreenPixelX, WIDTH_MASK);
	outBinBoundingBoxMinY = _mm256_and_si256(minScreenPixelY, HEIGHT_MASK);
	outBinBoundingBoxMaxX = _mm256_and_si256(_mm256_add_epi32(maxScreenPixelX, _mm256_set1_epi32(TILE_WIDTH)), WIDTH_MASK);
	outBinBoundingBoxMaxY = _mm256_and_si256(_mm256_add_epi32(maxScreenPixelY, _mm256_set1_epi32(TILE_HEIGHT)), HEIGHT_MASK);

	outBinBoundingBoxMinX = _mm256_max_epi32(outBinBoundingBoxMinX, _mm256_set1_epi32(this->mMaskedOcclusionCulling.mDepthBuffer.mResolution.mLeftBottomTileOrginX));
	outBinBoundingBoxMinY = _mm256_max_epi32(outBinBoundingBoxMinY, _mm256_set1_epi32(this->mMaskedOcclusionCulling.mDepthBuffer.mResolution.mLeftBottomTileOrginY));
	outBinBoundingBoxMaxX = _mm256_min_epi32(outBinBoundingBoxMaxX, _mm256_set1_epi32(this->mMaskedOcclusionCulling.mDepthBuffer.mResolution.mRightTopTileOrginX));
	outBinBoundingBoxMaxY = _mm256_min_epi32(outBinBoundingBoxMaxY, _mm256_set1_epi32(this->mMaskedOcclusionCulling.mDepthBuffer.mResolution.mRightTopTileOrginY));
}


void culling::BinTrianglesStage::PassTrianglesToTileBin(const M256F* screenPixelX, const M256F* screenPixelY, unsigned int& triangleCullMask, TriangleList& tileBin, const M256F& outBinBoundingBoxMinX, const M256F& outBinBoundingBoxMinY, const M256F& outBinBoundingBoxMaxX, const M256F& outBinBoundingBoxMaxY)
{
}




void culling::BinTrianglesStage::GatherVertex
(
	const float* vertices, const unsigned int* vertexIndices, const size_t indiceCount, const size_t currentIndiceIndex, const size_t vertexStrideByte, const size_t fetchTriangleCount,
	M256F* outVerticesX, M256F* outVerticesY, M256F* outVerticesZ, unsigned int& triangleCullMask
)
{
	assert(indiceCount % 3 == 0);
	assert(currentIndiceIndex % 3 == 0);

	//Gather Indices
	M256I m256i_indices[3];
	const unsigned int* currentVertexIndices = vertexIndices + currentIndiceIndex;
	const M256I indiceIndexs = _mm256_setr_epi32(0, 3, 6, 9, 12, 15, 18, 21);
	static const M256I SIMD_LANE_MASK[9] = {
		_mm256_setr_epi32(0,  0,  0,  0,  0,  0,  0,  0),
		_mm256_setr_epi32(~0,  0,  0,  0,  0,  0,  0,  0),
		_mm256_setr_epi32(~0, ~0,  0,  0,  0,  0,  0,  0),
		_mm256_setr_epi32(~0, ~0, ~0,  0,  0,  0,  0,  0),
		_mm256_setr_epi32(~0, ~0, ~0, ~0,  0,  0,  0,  0),
		_mm256_setr_epi32(~0, ~0, ~0, ~0, ~0,  0,  0,  0),
		_mm256_setr_epi32(~0, ~0, ~0, ~0, ~0, ~0,  0,  0),
		_mm256_setr_epi32(~0, ~0, ~0, ~0, ~0, ~0, ~0,  0),
		_mm256_setr_epi32(~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0)
	};

	// Compute per-lane index list offset that guards against out of bounds memory accesses
	const M256I safeIndiceIndexs = _mm256_and_si256(indiceIndexs, SIMD_LANE_MASK[fetchTriangleCount]);

	//If stride is 7
	//Current Value 
	//m256i_indices[0] : 0, 3, 6,  9, 12, 15, 18, 21
	//m256i_indices[1] : 1, 4, 7, 10, 13, 16, 19, 22
	//m256i_indices[2] : 2, 5, 8, 11, 14, 17, 20, 23
	//Point1s of Triangle
	m256i_indices[0] = _mm256_i32gather_epi32(reinterpret_cast<const int*>(currentVertexIndices + 0), safeIndiceIndexs, 4); // why 4? -> vertexIndices is unsigned int( 4byte )
	//Point2s of Triangle
	m256i_indices[1] = _mm256_i32gather_epi32(reinterpret_cast<const int*>(currentVertexIndices + 1), safeIndiceIndexs, 4);
	//Point3s of Triangle
	m256i_indices[2] = _mm256_i32gather_epi32(reinterpret_cast<const int*>(currentVertexIndices + 2), safeIndiceIndexs, 4);

	//Consider Stride
	//If StrideByte is 28
	//m256i_indices[0] : 0 * 28, 3 * 28, 6 * 28,  9 * 28, 12 * 28, 15 * 28, 18 * 28, 21 * 28
	//m256i_indices[1] : 1 * 28, 4 * 28, 7 * 28, 10 * 28, 13 * 28, 16 * 28, 19 * 28, 22 * 28
	//m256i_indices[2] : 2 * 28, 5 * 28, 8 * 28, 11 * 28, 14 * 28, 17 * 28, 20 * 28, 23 * 28
	const M256I m256i_stride = _mm256_set1_epi32(static_cast<int>(vertexStrideByte));
	m256i_indices[0] = _mm256_mullo_epi32(m256i_indices[0], m256i_stride);
	m256i_indices[1] = _mm256_mullo_epi32(m256i_indices[1], m256i_stride);
	m256i_indices[2] = _mm256_mullo_epi32(m256i_indices[2], m256i_stride);

	//Gather vertexs
	//Should consider vertexStride(vertices isn't stored contiguously because generally vertex datas is stored with uv, normal datas...) 
	//And Should consider z value
	for (size_t i = 0; i < 3; i++)
	{
		outVerticesX[i] = _mm256_i32gather_ps(vertices, m256i_indices[i], 1);
		outVerticesY[i] = _mm256_i32gather_ps(vertices + 1, m256i_indices[i], 1);
		outVerticesZ[i] = _mm256_i32gather_ps(vertices + 2, m256i_indices[i], 1);
	}
}

culling::BinTrianglesStage::BinTrianglesStage(MaskedSWOcclusionCulling& mMOcclusionCulling)
	: MaskedSWOcclusionCullingStage{ mMOcclusionCulling }
{
}

void culling::BinTrianglesStage::BinTriangles(
	const float* vertices, const unsigned int* vertexIndices, const size_t indiceCount, const size_t vertexStrideByte, 
	const float* modelToClipspaceMatrix
)
{
	size_t currentIndiceIndex = 0;
	static constexpr unsigned int triangleCountPerLoop = 8;

	while (currentIndiceIndex < indiceCount)
	{
		const size_t fetchTriangleCount = (indiceCount - currentIndiceIndex) / 3;

		// First 4 bits show if traingle is valid
		// Current Value : 00000000 00000000 00000000 11111111
		unsigned int triangleCullMask = (1 << fetchTriangleCount) - 1;


		//Why Size of array is 3?
		//A M256F can have 8 floating-point
		//A TwoDTriangle have 3 point
		//So If you have just one M256F variable, a floating-point is unused.
		//Not to make unused space, 12 floating point is required per axis
		// M256F * 3 -> 8 TwoDTriangle with no unused space

		//We don't need z value in Binning stage!!!
		M256F ndcSpaceVertexX[3], ndcSpaceVertexY[3], ndcSpaceVertexZ[3], oneDividedByW[3];


		//TODO : Why use 1/w for depth test, not z -> Check this : https://github.com/GameTechDev/MaskedOcclusionCulling/issues/23

		//Gather Vertex with indice
		//WE ARRIVE AT MODEL SPACE COORDINATE!
		this->GatherVertex(vertices, vertexIndices, indiceCount, currentIndiceIndex, vertexStrideByte, fetchTriangleCount, ndcSpaceVertexX, ndcSpaceVertexY, ndcSpaceVertexZ, triangleCullMask);

		if (triangleCullMask == 0x0)
		{
			continue;
		}

		//Convert Model space Vertex To Clip space Vertex
		//WE ARRIVE AT CLIP SPACE COORDINATE. W IS NOT 1
		this->TransformVertexsToClipSpace(ndcSpaceVertexX, ndcSpaceVertexY, ndcSpaceVertexZ, oneDividedByW, modelToClipspaceMatrix, triangleCullMask);

		for (size_t i = 0; i < 3; i++)
		{
			//oneDividedByW finally become oneDividedByW
			oneDividedByW[i] = culling::M256F_DIV(_mm256_set1_ps(1.0f), oneDividedByW[i]);
		}

		//WE ARRIVE AT NDC SPACE COORDINATE. 
		//If you use Opengl, Vertexs have value from -1 to 1
		//if you use DirectX, Vertexs have value from 0 to 1 
		//W BECOME USELESS, IGNORE IT
		this->ConverClipSpaceToNDCSpace(ndcSpaceVertexX, ndcSpaceVertexY, ndcSpaceVertexZ, oneDividedByW, triangleCullMask);

		//ScreenPixelPos : 0 ~ this->mDepthBuffer.Width, Height
		M256F screenPixelPosX[3], screenPixelPosY[3];
		this->ConvertNDCSpaceToScreenPixelSpace(ndcSpaceVertexX, ndcSpaceVertexY, screenPixelPosX, screenPixelPosY, triangleCullMask);
		//Clip Space Cull

		//BackFace Cull
		// 
		this->CullBackfaces(screenPixelPosX, screenPixelPosY, triangleCullMask);

		//Do not Sort Triangle in binning stage
		//because Culled triangles is also sorted
		//Sort triangle in drawing depth stage/
		//In that stage, all triangles is valid

		if (triangleCullMask == 0x0)
		{
			continue;
		}

		
		

		M256I outBinBoundingBoxMinX, outBinBoundingBoxMinY, outBinBoundingBoxMaxX, outBinBoundingBoxMaxY;
		//Bin Triangles to tiles

		//Compute Bin Bounding Box
		//Get Intersecting Bin List
		this->ComputeBinBoundingBox(screenPixelPosX, screenPixelPosY, outBinBoundingBoxMinX, outBinBoundingBoxMinY, outBinBoundingBoxMaxX, outBinBoundingBoxMaxY);

		std::shared_ptr<Tile[]> tiles = this->mMaskedOcclusionCulling.mDepthBuffer.mTiles;

		for (size_t triangleIndex = 0; triangleIndex < triangleCountPerLoop && ( (triangleCullMask & (1 << triangleIndex) ) != 0x0); triangleIndex++)
		{
			const size_t intersectingMinBoxX = reinterpret_cast<int*>(&outBinBoundingBoxMinX)[triangleIndex];
			const size_t intersectingMinBoxY = reinterpret_cast<int*>(&outBinBoundingBoxMinY)[triangleIndex];
			const size_t intersectingMaxBoxX = reinterpret_cast<int*>(&outBinBoundingBoxMaxX)[triangleIndex];
			const size_t intersectingMaxBoxY = reinterpret_cast<int*>(&outBinBoundingBoxMaxY)[triangleIndex];

			for (size_t y = intersectingMinBoxY; y <= intersectingMaxBoxY; y++)
			{
				for (size_t x = intersectingMinBoxX; y <= intersectingMaxBoxX; y++)
				{
					Tile& targetTile = tiles[x + y * this->mMaskedOcclusionCulling.mDepthBuffer.mResolution.mTileCountInARow];

					const size_t triListIndex = targetTile.mBinnedTriangles.mCurrentTriangleCount;
					for (size_t pointIndex = 0; pointIndex < 3; pointIndex++)
					{
						targetTile.mBinnedTriangles.mTriangleList[triListIndex].Points[pointIndex].x = reinterpret_cast<float*>(&screenPixelPosX)[triListIndex];
						targetTile.mBinnedTriangles.mTriangleList[triListIndex].Points[pointIndex].y = reinterpret_cast<float*>(&screenPixelPosY)[triListIndex];
						targetTile.mBinnedTriangles.mTriangleList[triListIndex].Points[pointIndex].z = reinterpret_cast<float*>(&ndcSpaceVertexZ)[triListIndex];
					}
				
					targetTile.mBinnedTriangles.mCurrentTriangleCount++;
					
				}
			}

		}


		//this->ComputeBinBoundingBox

		currentIndiceIndex += triangleCountPerLoop * 3; 
	}
}