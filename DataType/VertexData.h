#pragma once

#include "Math/Vector.h"

namespace culling
{
	struct VertexData
	{
		float* mVertices;
		size_t mVerticeCount;

		unsigned int* mIndices;
		size_t mIndiceCount;
	};

}