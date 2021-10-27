#pragma once

#include "../EveryCullingCore.h"

namespace culling
{
	struct VertexData
	{
		float* mVertices;
		size_t mVerticeCount;

		std::uint32_t* mIndices;
		size_t mIndiceCount;
	};

}