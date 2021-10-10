#pragma once

#include "Math/Matrix.h"

namespace culling
{
	struct QueryObject
	{
		culling::Mat4x4 mLocal2WorldMatrix;
		unsigned int mQueryID;
		unsigned int mBufferID;
		unsigned int mVertexArrayObjectID;
	};

}

