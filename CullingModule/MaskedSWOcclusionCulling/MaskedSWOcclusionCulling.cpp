#include "MaskedSWOcclusionCulling.h"





void culling::MaskedSWOcclusionCulling::TransformVertexs(M128F* vertX, M128F* vertY, M128F* vertZ, const float* modelToClipspaceMatrix)
{
	if (modelToClipspaceMatrix != nullptr)
	{
		for (int i = 0; i < 3; ++i)
		{
			M128F tmpX, tmpY, tmpW;
			tmpX = M128F_MUL_AND_ADD(vertX[i], _mm_set1_ps(modelToClipspaceMatrix[0]), M128F_MUL_AND_ADD(vertY[i], _mm_set1_ps(modelToClipspaceMatrix[4]), M128F_MUL_AND_ADD(vertZ[i], _mm_set1_ps(modelToClipspaceMatrix[8]), _mm_set1_ps(modelToClipspaceMatrix[12]))));
			tmpY = M128F_MUL_AND_ADD(vertX[i], _mm_set1_ps(modelToClipspaceMatrix[1]), M128F_MUL_AND_ADD(vertY[i], _mm_set1_ps(modelToClipspaceMatrix[5]), M128F_MUL_AND_ADD(vertZ[i], _mm_set1_ps(modelToClipspaceMatrix[9]), _mm_set1_ps(modelToClipspaceMatrix[13]))));
			tmpW = M128F_MUL_AND_ADD(vertX[i], _mm_set1_ps(modelToClipspaceMatrix[3]), M128F_MUL_AND_ADD(vertY[i], _mm_set1_ps(modelToClipspaceMatrix[7]), M128F_MUL_AND_ADD(vertZ[i], _mm_set1_ps(modelToClipspaceMatrix[11]), _mm_set1_ps(modelToClipspaceMatrix[15]))));
			vertX[i] = tmpX;	
			vertY[i] = tmpY;	
			vertZ[i] = tmpW;
		}
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
		//A Triangle have 3 point
		//So If you have just one M128F variable, a floating-point is unused.
		//Not to make unused space, 12 floating point is required per axis
		// M128F * 3 -> 12 floating-point -> 4 Triangle with no unused space
		M128F vertexX[3], vertexY[3], vertexZ[3];



	}


	
}

