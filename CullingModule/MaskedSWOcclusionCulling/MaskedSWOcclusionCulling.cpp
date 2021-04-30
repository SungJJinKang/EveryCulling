#include "MaskedSWOcclusionCulling.h"


void culling::MaskedSWOcclusionCulling::SetViewProjectionMatrix(math::Matrix4x4& viewProjectionMatrix)
{
	this->mViewProjectionMatrix = viewProjectionMatrix.data();
}




void culling::MaskedSWOcclusionCulling::TransformVertexsToClipSpace(M128F* vertX, M128F* vertY, M128F* vertW, const float* toClipspaceMatrix)
{
	if (toClipspaceMatrix != nullptr)
	{
		for (int i = 0; i < 3; ++i)
		{
			M128F tmpX, tmpY, tmpW;
			tmpX = M128F_MUL_AND_ADD(vertX[i], _mm_set1_ps(toClipspaceMatrix[0]), M128F_MUL_AND_ADD(vertY[i], _mm_set1_ps(toClipspaceMatrix[4]), M128F_MUL_AND_ADD(vertW[i], _mm_set1_ps(toClipspaceMatrix[8]), _mm_set1_ps(toClipspaceMatrix[12]))));
			tmpY = M128F_MUL_AND_ADD(vertX[i], _mm_set1_ps(toClipspaceMatrix[1]), M128F_MUL_AND_ADD(vertY[i], _mm_set1_ps(toClipspaceMatrix[5]), M128F_MUL_AND_ADD(vertW[i], _mm_set1_ps(toClipspaceMatrix[9]), _mm_set1_ps(toClipspaceMatrix[13]))));
			tmpW = M128F_MUL_AND_ADD(vertX[i], _mm_set1_ps(toClipspaceMatrix[3]), M128F_MUL_AND_ADD(vertY[i], _mm_set1_ps(toClipspaceMatrix[7]), M128F_MUL_AND_ADD(vertW[i], _mm_set1_ps(toClipspaceMatrix[11]), _mm_set1_ps(toClipspaceMatrix[15]))));
			vertX[i] = tmpX;	
			vertY[i] = tmpY;	
			vertW[i] = tmpW;
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

void culling::MaskedSWOcclusionCulling::GatherVertex(const float* vertices, const unsigned int* vertexIndices, size_t indiceCount, size_t currentIndiceCount, M128F* verticesX, M128F* verticesY, M128F* verticesZ)
{


	//

}

void culling::MaskedSWOcclusionCulling::BinTriangles(const float* vertices, const unsigned int* vertexIndices, size_t indiceCount, float* modelToClipspaceMatrix)
{

	size_t currentIndiceIndex = 0;
	unsigned int numSIMDLane = 4;

	while (currentIndiceIndex < indiceCount)
	{
		unsigned int triMask = 1 << numSIMDLane - 1;
		//Why Size of array is 3?
		//A M128F can have 4 floating-point
		//A TwoDTriangle have 3 point
		//So If you have just one M128F variable, a floating-point is unused.
		//Not to make unused space, 12 floating point is required per axis
		// M128F * 3 -> 12 floating-point -> 4 TwoDTriangle with no unused space
		M128F vertexX[3], vertexY[3], vertexZ[3];



	}


	
}

