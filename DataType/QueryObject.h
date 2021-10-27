#pragma once

#include "Math/Matrix.h"

namespace culling
{
	struct QueryObject
	{
		culling::Mat4x4 mLocal2WorldMatrix;
		std::uint32_t mQueryID;
		std::uint32_t mBufferID;
		std::uint32_t mVertexArrayObjectID;
	};

}

