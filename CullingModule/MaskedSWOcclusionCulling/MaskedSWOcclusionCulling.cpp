#include "MaskedSWOcclusionCulling.h"



culling::MaskedSWOcclusionCulling::MaskedSWOcclusionCulling(unsigned int width, unsigned int height, float nearClipPlaneDis, float farClipPlaneDis, float* viewProjectionMatrix)
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


void culling::MaskedSWOcclusionCulling::ConverClipSpaceToNDCSpace(M128F* outClipVertexX, M128F* outClipVertexY, const M128F* oneDividedByW, unsigned int& triangleMask)
{
	for (size_t i = 0; i < 3; i++)
	{
		//Why Do This??
		//compute 1/w in advance 
		
		outClipVertexX[i] = M128F_MUL(outClipVertexX[i], oneDividedByW[i]);
		outClipVertexY[i] = M128F_MUL(outClipVertexY[i], oneDividedByW[i]);

		//This code is useless
		//outClipVertexW[i] = M128F_MUL(outClipVertexW[i], outClipVertexW[i]);
	}
	
}

void culling::MaskedSWOcclusionCulling::ConvertNDCSpaceToScreenPixelSpace(const M128F* ndcSpaceVertexX, const M128F* ndcSpaceVertexY, M128F* outScreenPixelSpaceX, M128F* outScreenPixelSpaceY, unsigned int& triangleMask)
{
	for (size_t i = 0; i < 3; i++)
	{
		M128F tmpScreenSpaceX, tmpScreenSpaceY;

		//Convert NDC Space Coordinates To Screen Space Coordinates 
#if NDC_RANGE == MINUS_ONE_TO_POSITIVE_ONE
		tmpScreenSpaceX = M128F_MUL_AND_ADD(ndcSpaceVertexX[i], this->mDepthBuffer.mResolution.mReplicatedScreenHalfWidth, this->mDepthBuffer.mResolution.mReplicatedScreenHalfWidth);
		tmpScreenSpaceY = M128F_MUL_AND_ADD(ndcSpaceVertexY[i], this->mDepthBuffer.mResolution.mReplicatedScreenHalfHeight, this->mDepthBuffer.mResolution.mReplicatedScreenHalfHeight);
#elif NDC_RANGE == ZERO_TO_POSITIVE_ONE
		tmpScreenSpaceX = M128F_MUL(ndcSpaceVertexX[i], this->mDepthBuffer.mResolution.mReplicatedScreenWidth);
		tmpScreenSpaceY = M128F_MUL(ndcSpaceVertexY[i], this->mDepthBuffer.mResolution.mReplicatedScreenHeight);
#else 
		assert(0); //NEVER HAPPEN
#endif

		//Snap Screen Space Coordinates To Integer Coordinate In ScreenBuffer(or DepthBuffer)
		// 
		// The rounding modes are set to match HW rasterization with OpenGL. In practice our samples are placed
		// in the (1,0) corner of each pixel, while HW rasterizer uses (0.5, 0.5). We get (1,0) because of the 
		// floor used when interpolating along triangle edges. The rounding modes match an offset of (0.5, -0.5)
		outScreenPixelSpaceX[i] = _mm_ceil_ps(tmpScreenSpaceX);
		outScreenPixelSpaceY[i] = _mm_floor_ps(tmpScreenSpaceY);
		

	}

}

void culling::MaskedSWOcclusionCulling::TransformVertexsToClipSpace(M128F* outClipVertexX, M128F* outClipVertexY, M128F* outClipVertexW, const float* toClipspaceMatrix, unsigned int& triangleMask)
{
	if (toClipspaceMatrix != nullptr)
	{
		//Actually triMask should be considered.
		//But Just Compute it all.
		//TODO : Consider Trimask
		for (size_t i = 0; i < 3; ++i)
		{
			M128F tmpX, tmpY, tmpW;

			tmpX = M128F_MUL_AND_ADD(outClipVertexX[i], _mm_set1_ps(toClipspaceMatrix[0]), M128F_MUL_AND_ADD(outClipVertexY[i], _mm_set1_ps(toClipspaceMatrix[4]), M128F_MUL_AND_ADD(outClipVertexW[i], _mm_set1_ps(toClipspaceMatrix[8]), _mm_set1_ps(toClipspaceMatrix[12]))));
			tmpY = M128F_MUL_AND_ADD(outClipVertexX[i], _mm_set1_ps(toClipspaceMatrix[1]), M128F_MUL_AND_ADD(outClipVertexY[i], _mm_set1_ps(toClipspaceMatrix[5]), M128F_MUL_AND_ADD(outClipVertexW[i], _mm_set1_ps(toClipspaceMatrix[9]), _mm_set1_ps(toClipspaceMatrix[13]))));
			tmpW = M128F_MUL_AND_ADD(outClipVertexX[i], _mm_set1_ps(toClipspaceMatrix[3]), M128F_MUL_AND_ADD(outClipVertexY[i], _mm_set1_ps(toClipspaceMatrix[7]), M128F_MUL_AND_ADD(outClipVertexW[i], _mm_set1_ps(toClipspaceMatrix[11]), _mm_set1_ps(toClipspaceMatrix[15]))));
		
			outClipVertexX[i] = tmpX;
			outClipVertexY[i] = tmpY;
			outClipVertexW[i] = tmpW;
			
		}
	}
}

void culling::MaskedSWOcclusionCulling::SortTriangle(TwoDTriangle& triangle)
{
	if (triangle.Point1.y < triangle.Point2.y)
	{
		std::swap(triangle.Point1, triangle.Point2);
	}
	if (triangle.Point1.y < triangle.Point3.y)
	{
		std::swap(triangle.Point1, triangle.Point3);
	}
	if (triangle.Point2.y < triangle.Point3.y)
	{
		std::swap(triangle.Point2, triangle.Point3);
	}
}

void culling::MaskedSWOcclusionCulling::GatherVertex(
	const Vector3* vertices, const unsigned int* vertexIndices, const size_t indiceCount, const size_t currentIndiceIndex, 
	M128F* outVerticesX, M128F* outVerticesY, unsigned int& triangleMask)
{
	assert(indiceCount % 3 == 0);
	assert(currentIndiceIndex % 3 == 0);

	//Gather 12 Point -> 4 Triangles
	
	size_t triangleIndex = 0;
	for (
		size_t indiceIndex = currentIndiceIndex ; 
		indiceIndex < indiceCount && indiceIndex < currentIndiceIndex + 4 * 3 ; 
		indiceIndex += 3
		)
	{

		//store 3 point of a triangle
		for (size_t pointIndex = 0; pointIndex < 3; pointIndex++)
		{
			size_t vertexIndice = vertexIndices[currentIndiceIndex + pointIndex];
			reinterpret_cast<float*>(outVerticesX + pointIndex)[triangleIndex] = vertices[vertexIndice].x;
			reinterpret_cast<float*>(outVerticesY + pointIndex)[triangleIndex] = vertices[vertexIndice].y;
		}

		triangleIndex++;
		assert(triangleIndex < 4);
	}
	
	//triangleMask ^= 1 << triangleIndex;
}

void culling::MaskedSWOcclusionCulling::BinTriangles(const Vector3* vertices, const unsigned int* vertexIndices, size_t indiceCount, float* modelToClipspaceMatrix)
{

	size_t currentIndiceIndex = 0;
	unsigned int triangleCountPerLoop = 4;

	while (currentIndiceIndex < indiceCount)
	{
		// First 4 bits show if traingle is valid
		// Current Value : 00000000 00000000 00000000 00001111
		unsigned int triMask = (1 << triangleCountPerLoop) - 1;

		//Why Size of array is 3?
		//A M128F can have 4 floating-point
		//A TwoDTriangle have 3 point
		//So If you have just one M128F variable, a floating-point is unused.
		//Not to make unused space, 12 floating point is required per axis
		// M128F * 3 -> 12 floating-point -> 4 TwoDTriangle with no unused space

		//We don't need z value at Binning stage
		M128F ndcSpaceVertexX[3], ndcSpaceVertexY[3], oneDividedByW[3];
		

		//Gather Vertex with indice
		//WE ARRIVE AT MODEL SPACE COORDINATE!
		this->GatherVertex(vertices, vertexIndices, indiceCount, currentIndiceIndex, ndcSpaceVertexX, ndcSpaceVertexY, triMask);

		if (triMask == 0x0)
		{
			break;
		}

		//Convert Model space Vertex To Clip space Vertex
		//WE ARRIVE AT CLIP SPACE COORDINATE. W IS NOT 1
		this->TransformVertexsToClipSpace(ndcSpaceVertexX, ndcSpaceVertexY, oneDividedByW, modelToClipspaceMatrix, triMask);
		
		for (size_t i = 0; i < 3; i++)
		{
			//oneDividedByW finally become oneDividedByW
			oneDividedByW[i] = M128F_DIV(_mm_set1_ps(1.0f), oneDividedByW[i]);
		}
		
		//WE ARRIVE AT NDC SPACE COORDINATE. 
		//If you use Opengl, Vertexs have value from -1 to 1
		//if you use DirectX, Vertexs have value from 0 to 1 
		//W BECOME USELESS, IGNORE IT
		this->ConverClipSpaceToNDCSpace(ndcSpaceVertexX, ndcSpaceVertexY, oneDividedByW, triMask);

		M128F screenPixelPosX[3], screenPixelPosY[3];
		this->ConvertNDCSpaceToScreenPixelSpace(ndcSpaceVertexX, ndcSpaceVertexY, screenPixelPosX, screenPixelPosY, triMask);
		//Clip Space Cull
		
		//BackFace Cull
		// 
		//TODO : I don't now How this Works.........
		M128F triArea1 = M128F_MUL(M128F_SUB(screenPixelPosX[1], screenPixelPosX[0]), M128F_SUB(screenPixelPosY[2], screenPixelPosY[0]));
		M128F triArea2 = M128F_MUL(M128F_SUB(screenPixelPosX[0], screenPixelPosX[2]), M128F_SUB(screenPixelPosY[0], screenPixelPosY[1]));
		M128F triArea = M128F_SUB(triArea1, triArea2);
		M128F ccwMask = _mm_cmpgt_ps(triArea, _mm_set1_ps(0.0f));
		//
		
		//this->CullBackfaces(screenPixelPosX, screenPixelPosY, oneDividedByW, ccwMask, bfWinding);
		
		//Set each bit of mask dst based on the most significant bit of the corresponding packed single-precision (32-bit) floating-point element in a.
		//https://software.intel.com/sites/landingpage/IntrinsicsGuide/#techs=SSE,SSE2,SSE3,SSSE3,SSE4_1,SSE4_2,AVX&expand=2156,4979,4979,1731,4929,951,4979,3869&text=movemask
		//if second triangle is front facing, low second bit of triMask is 1
		triMask = _mm_movemask_ps(ccwMask);

		if (triMask == 0x0)
		{
			break;
		}

		//Bin Triangles to tiles

		currentIndiceIndex += 12;
	}


	
}

