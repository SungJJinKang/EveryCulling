#pragma once

namespace culling
{
	struct VertexData
	{
		FLOAT32* mVertices;
		SIZE_T mVerticeCount;

		UINT32* mIndices;
		SIZE_T mIndiceCount;
	};

}