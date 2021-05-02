#include "MaskedSWOcclusionCulling.h"


culling::MaskedSWOcclusionCulling::MaskedSWOcclusionCulling
(
	unsigned int width, unsigned int height, 
	float nearClipPlaneDis, float farClipPlaneDis, 
	float* viewProjectionMatrix
)
	: mDepthBuffer{ width, height }, 
	mNearClipPlaneDis{ nearClipPlaneDis }, mFarClipPlaneDis{ farClipPlaneDis }, mViewProjectionMatrix{ viewProjectionMatrix }
{
}

void culling::MaskedSWOcclusionCulling::SetNearFarClipPlaneDistance(float nearClipPlaneDis, float farClipPlaneDis)
{
	this->mNearClipPlaneDis = nearClipPlaneDis;
	this->mFarClipPlaneDis = farClipPlaneDis;
}

void culling::MaskedSWOcclusionCulling::SetViewProjectionMatrix(float* viewProjectionMatrix)
{
	this->mViewProjectionMatrix = viewProjectionMatrix;
}


void culling::MaskedSWOcclusionCulling::ConverClipSpaceToNDCSpace
(
	M256F* outClipVertexX, M256F* outClipVertexY, const M256F* oneDividedByW, 
	int & triangleCullMask
)
{
	for (size_t i = 0; i < 3; i++)
	{
		//Why Do This??
		//compute 1/w in advance 
		
		outClipVertexX[i] = culling::M256F_MUL(outClipVertexX[i], oneDividedByW[i]);
		outClipVertexY[i] = culling::M256F_MUL(outClipVertexY[i], oneDividedByW[i]);

		//This code is useless
		//outClipVertexW[i] = culling::M256F_MUL(outClipVertexW[i], outClipVertexW[i]);
	}
	
}

void culling::MaskedSWOcclusionCulling::ConvertNDCSpaceToScreenPixelSpace
(
	const M256F* ndcSpaceVertexX, const M256F* ndcSpaceVertexY, 
	M256F* outScreenPixelSpaceX, M256F* outScreenPixelSpaceY, 
	int & triangleCullMask
)
{
	for (size_t i = 0; i < 3; i++)
	{
		M256F tmpScreenSpaceX, tmpScreenSpaceY;

		//Convert NDC Space Coordinates To Screen Space Coordinates 
#if NDC_RANGE == MINUS_ONE_TO_POSITIVE_ONE
		tmpScreenSpaceX = culling::M256F_MUL_AND_ADD(ndcSpaceVertexX[i], this->mDepthBuffer.mResolution.mReplicatedScreenHalfWidth, this->mDepthBuffer.mResolution.mReplicatedScreenHalfWidth);
		tmpScreenSpaceY = culling::M256F_MUL_AND_ADD(ndcSpaceVertexY[i], this->mDepthBuffer.mResolution.mReplicatedScreenHalfHeight, this->mDepthBuffer.mResolution.mReplicatedScreenHalfHeight);
#elif NDC_RANGE == ZERO_TO_POSITIVE_ONE
		tmpScreenSpaceX = culling::M256F_MUL(ndcSpaceVertexX[i], this->mDepthBuffer.mResolution.mReplicatedScreenWidth);
		tmpScreenSpaceY = culling::M256F_MUL(ndcSpaceVertexY[i], this->mDepthBuffer.mResolution.mReplicatedScreenHeight);
#else 
		assert(0); //NEVER HAPPEN
#endif

		//Snap Screen Space Coordinates To Integer Coordinate In ScreenBuffer(or DepthBuffer)
		// 
		// The rounding modes are set to match HW rasterization with OpenGL. In practice our samples are placed
		// in the (1,0) corner of each pixel, while HW rasterizer uses (0.5, 0.5). We get (1,0) because of the 
		// floor used when interpolating along triangle edges. The rounding modes match an offset of (0.5, -0.5)
		outScreenPixelSpaceX[i] = _mm256_ceil_ps(tmpScreenSpaceX);
		outScreenPixelSpaceY[i] = _mm256_floor_ps(tmpScreenSpaceY);
		

	}

}

void culling::MaskedSWOcclusionCulling::TransformVertexsToClipSpace
(
	M256F* outClipVertexX, M256F* outClipVertexY, M256F* outClipVertexW, 
	const float* toClipspaceMatrix, int & triangleCullMask
)
{
	if (toClipspaceMatrix != nullptr)
	{
		//Actually triangleCullMask should be considered.
		//But Just Compute it all.
		//TODO : Consider Trimask
		for (size_t i = 0; i < 3; ++i)
		{
			M256F tmpX, tmpY, tmpW;

			tmpX = culling::M256F_MUL_AND_ADD(outClipVertexX[i], _mm256_set1_ps(toClipspaceMatrix[0]), culling::M256F_MUL_AND_ADD(outClipVertexY[i], _mm256_set1_ps(toClipspaceMatrix[4]), culling::M256F_MUL_AND_ADD(outClipVertexW[i], _mm256_set1_ps(toClipspaceMatrix[8]), _mm256_set1_ps(toClipspaceMatrix[12]))));
			tmpY = culling::M256F_MUL_AND_ADD(outClipVertexX[i], _mm256_set1_ps(toClipspaceMatrix[1]), culling::M256F_MUL_AND_ADD(outClipVertexY[i], _mm256_set1_ps(toClipspaceMatrix[5]), culling::M256F_MUL_AND_ADD(outClipVertexW[i], _mm256_set1_ps(toClipspaceMatrix[9]), _mm256_set1_ps(toClipspaceMatrix[13]))));
			tmpW = culling::M256F_MUL_AND_ADD(outClipVertexX[i], _mm256_set1_ps(toClipspaceMatrix[3]), culling::M256F_MUL_AND_ADD(outClipVertexY[i], _mm256_set1_ps(toClipspaceMatrix[7]), culling::M256F_MUL_AND_ADD(outClipVertexW[i], _mm256_set1_ps(toClipspaceMatrix[11]), _mm256_set1_ps(toClipspaceMatrix[15]))));
		
			outClipVertexX[i] = tmpX;
			outClipVertexY[i] = tmpY;
			outClipVertexW[i] = tmpW;
			
		}
	}
}

void culling::MaskedSWOcclusionCulling::CullBackfaces(const M256F* screenPixelX, const M256F* screenPixelY, int& triangleCullMask)
{
	//I don't know How this Works.........
		//https://stackoverflow.com/questions/67357115/i-found-back-face-culling-code-but-i-cant-know-how-this-works
	M256F triArea1 = culling::M256F_MUL(culling::M256F_SUB(screenPixelX[1], screenPixelX[0]), culling::M256F_SUB(screenPixelY[2], screenPixelY[0]));
	M256F triArea2 = culling::M256F_MUL(culling::M256F_SUB(screenPixelX[0], screenPixelX[2]), culling::M256F_SUB(screenPixelY[0], screenPixelY[1]));
	M256F triArea = culling::M256F_SUB(triArea1, triArea2);

	//_CMP_GT_OQ vs _CMP_GT_OQ : https://stackoverflow.com/questions/16988199/how-to-choose-avx-compare-predicate-variants
	M256F ccwMask = _mm256_cmp_ps(triArea, _mm256_set1_ps(0.0f), _CMP_GT_OQ);

	//Set each bit of mask dst based on the most significant bit of the corresponding packed single-precision (32-bit) floating-point element in a.
	//https://software.intel.com/sites/landingpage/IntrinsicsGuide/#techs=SSE,SSE2,SSE3,SSSE3,SSE4_1,SSE4_2,AVX&expand=2156,4979,4979,1731,4929,951,4979,3869&text=movemask
	//if second triangle is front facing, low second bit of triangleCullMask is 1
	triangleCullMask = _mm256_movemask_ps(ccwMask);
}



void culling::MaskedSWOcclusionCulling::GatherVertex
(
	const float* vertices, const unsigned int* vertexIndices, const size_t indiceCount, const size_t currentIndiceIndex, const size_t vertexStrideByte, const size_t fetchTriangleCount,
	M256F* outVerticesX, M256F* outVerticesY, int & triangleCullMask
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
	}
}

void culling::MaskedSWOcclusionCulling::BinTriangles(
	const float* vertices, const unsigned int* vertexIndices, size_t indiceCount, size_t vertexStrideByte, 
	float* modelToClipspaceMatrix
)
{
	size_t currentIndiceIndex = 0;
	const unsigned int triangleCountPerLoop = 8;

	while (currentIndiceIndex < indiceCount)
	{
		// First 4 bits show if traingle is valid
		// Current Value : 00000000 00000000 00000000 11111111
		int triangleCullMask = (1 << triangleCountPerLoop) - 1;
		const size_t fetchTriangleCount = (indiceCount - currentIndiceIndex) / 3;

		//Why Size of array is 3?
		//A M256F can have 8 floating-point
		//A TwoDTriangle have 3 point
		//So If you have just one M256F variable, a floating-point is unused.
		//Not to make unused space, 12 floating point is required per axis
		// M256F * 3 -> 8 TwoDTriangle with no unused space

		//We don't need z value at Binning stage!!!
		M256F ndcSpaceVertexX[3], ndcSpaceVertexY[3], oneDividedByW[3];
		
		
		//Gather Vertex with indice
		//WE ARRIVE AT MODEL SPACE COORDINATE!
		this->GatherVertex(vertices, vertexIndices, indiceCount, currentIndiceIndex, vertexStrideByte, fetchTriangleCount, ndcSpaceVertexX, ndcSpaceVertexY, triangleCullMask);

		if (triangleCullMask == 0x0)
		{
			continue;
		}

		//Convert Model space Vertex To Clip space Vertex
		//WE ARRIVE AT CLIP SPACE COORDINATE. W IS NOT 1
		this->TransformVertexsToClipSpace(ndcSpaceVertexX, ndcSpaceVertexY, oneDividedByW, modelToClipspaceMatrix, triangleCullMask);
		
		for (size_t i = 0; i < 3; i++)
		{
			//oneDividedByW finally become oneDividedByW
			oneDividedByW[i] = culling::M256F_DIV(_mm256_set1_ps(1.0f), oneDividedByW[i]);
		}
		
		//WE ARRIVE AT NDC SPACE COORDINATE. 
		//If you use Opengl, Vertexs have value from -1 to 1
		//if you use DirectX, Vertexs have value from 0 to 1 
		//W BECOME USELESS, IGNORE IT
		this->ConverClipSpaceToNDCSpace(ndcSpaceVertexX, ndcSpaceVertexY, oneDividedByW, triangleCullMask);

		M256F screenPixelPosX[3], screenPixelPosY[3];
		this->ConvertNDCSpaceToScreenPixelSpace(ndcSpaceVertexX, ndcSpaceVertexY, screenPixelPosX, screenPixelPosY, triangleCullMask);
		//Clip Space Cull
		
		//BackFace Cull
		// 
		this->CullBackfaces(screenPixelPosX, screenPixelPosY, triangleCullMask);
		
		if (triangleCullMask == 0x0)
		{
			continue;
		}

		//Bin Triangles to tiles

		currentIndiceIndex += triangleCountPerLoop * 3;
	}


	
}

