#pragma once

#include "Math/Matrix.h"

namespace culling
{
	struct QueryObject
	{
		culling::Mat4x4 mLocal2WorldMatrix;
		UINT32 mQueryID;
		UINT32 mBufferID;
		UINT32 mVertexArrayObjectID;
	};

}

