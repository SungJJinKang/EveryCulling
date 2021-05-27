#pragma once

#include "../EveryCullingCore.h"
#include "Math/Matrix.h"

namespace culling
{
	struct QueryObject
	{
		culling::Matrix4X4 mLocal2WorldMatrix;
		unsigned int mQueryID;
		unsigned int mBufferID;
		unsigned int mVertexArrayObjectID;
	};

}

