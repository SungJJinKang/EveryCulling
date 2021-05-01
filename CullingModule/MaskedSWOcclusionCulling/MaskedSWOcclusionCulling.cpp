#include "MaskedSWOcclusionCulling.h"


void culling::MaskedSWOcclusionCulling::SetViewProjectionMatrix(math::Matrix4x4& viewProjectionMatrix)
{
	this->mViewProjectionMatrix = viewProjectionMatrix.data();
}




void culling::MaskedSWOcclusionCulling::TransformVertexsToClipSpace(M128F* outClipVertexX, M128F* outClipVertexY, M128F* outClipVertexZ, M128F* outClipVertexW, const float* toClipspaceMatrix)
{
	if (toClipspaceMatrix != nullptr)
	{
		for (int i = 0; i < 3; ++i)
		{
			outClipVertexX[i] = M128F_MUL_AND_ADD(outClipVertexX[i], _mm_set1_ps(toClipspaceMatrix[0]), M128F_MUL_AND_ADD(outClipVertexY[i], _mm_set1_ps(toClipspaceMatrix[4]), M128F_MUL_AND_ADD(outClipVertexW[i], _mm_set1_ps(toClipspaceMatrix[8]), _mm_set1_ps(toClipspaceMatrix[12]))));
			outClipVertexY[i] = M128F_MUL_AND_ADD(outClipVertexX[i], _mm_set1_ps(toClipspaceMatrix[1]), M128F_MUL_AND_ADD(outClipVertexY[i], _mm_set1_ps(toClipspaceMatrix[5]), M128F_MUL_AND_ADD(outClipVertexW[i], _mm_set1_ps(toClipspaceMatrix[9]), _mm_set1_ps(toClipspaceMatrix[13]))));
			outClipVertexZ[i] = M128F_MUL_AND_ADD(outClipVertexX[i], _mm_set1_ps(toClipspaceMatrix[2]), M128F_MUL_AND_ADD(outClipVertexY[i], _mm_set1_ps(toClipspaceMatrix[6]), M128F_MUL_AND_ADD(outClipVertexW[i], _mm_set1_ps(toClipspaceMatrix[10]), _mm_set1_ps(toClipspaceMatrix[14]))));
			outClipVertexW[i] = M128F_MUL_AND_ADD(outClipVertexX[i], _mm_set1_ps(toClipspaceMatrix[3]), M128F_MUL_AND_ADD(outClipVertexY[i], _mm_set1_ps(toClipspaceMatrix[7]), M128F_MUL_AND_ADD(outClipVertexW[i], _mm_set1_ps(toClipspaceMatrix[11]), _mm_set1_ps(toClipspaceMatrix[15]))));
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
	M128F * outVerticesX, M128F * outVerticesY, M128F * outVerticesZ, unsigned int* triangleMask)
{
	assert(indiceCount % 3 == 0);
	assert(currentIndiceIndex % 3 == 0);

	//Gather 12 Point -> 4 Triangles
	size_t currentTriangleIndex = currentIndiceIndex / 3;
	const size_t targetTriangleIndex = MIN(currentIndiceIndex / 3 + 4, indiceCount / 3);

	/// maximum index of fOutVertexX : 11
	float* fOutVertexX = reinterpret_cast<float*>(outVerticesX);
	float* fOutVertexY = reinterpret_cast<float*>(outVerticesY);
	float* fOutVertexZ = reinterpret_cast<float*>(outVerticesZ);

	size_t fOutVertexIndex = 0;
	for (; currentTriangleIndex < targetTriangleIndex; currentTriangleIndex++)
	{
		const size_t targetVertexIndex = currentTriangleIndex * 3 + 3;
		for (size_t vertexIndex = currentTriangleIndex * 3; vertexIndex < targetVertexIndex; vertexIndex++)
		{
			size_t currentVertexIndex = vertexIndices[vertexIndex];
			fOutVertexX[fOutVertexIndex] = vertices[currentVertexIndex].x;
			fOutVertexY[fOutVertexIndex] = vertices[currentVertexIndex].y;
			fOutVertexZ[fOutVertexIndex] = vertices[currentVertexIndex].z;

			fOutVertexIndex++;
		}
	}


	//TODO : if A M128F isn't set, We can use SIMD set 0
	for (; fOutVertexIndex < 12; fOutVertexIndex++)
	{
		fOutVertexX[fOutVertexIndex] = 0.0f;
		fOutVertexY[fOutVertexIndex] = 0.0f;
		fOutVertexZ[fOutVertexIndex] = 0.0f;
	}

	//

}

void culling::MaskedSWOcclusionCulling::BinTriangles(const Vector3* vertices, const unsigned int* vertexIndices, size_t indiceCount, float* modelToClipspaceMatrix)
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
		M128F clipVertexX[3], clipVertexY[3], clipVertexZ[3], clipVertexW[3];

		//Gather Vertex with indice
		this->GatherVertex(vertices, vertexIndices, indiceCount, currentIndiceIndex, clipVertexX, clipVertexY, clipVertexZ );

		//Convert Model space Vertex To Clip space Vertex
		this->TransformVertexsToClipSpace(clipVertexX, clipVertexY, clipVertexZ, clipVertexW, modelToClipspaceMatrix);


		currentIndiceIndex += 12;
	}


	
}

