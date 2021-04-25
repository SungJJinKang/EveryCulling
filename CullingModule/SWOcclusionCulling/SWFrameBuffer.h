#pragma once

#include "../../FrotbiteCullingSystemCore.h"

namespace culling
{
	class SWFrameBuffer
	{
	public:
		const size_t mWidth;
		const size_t mHeight;
		float* mPixels;
		SWFrameBuffer(size_t width, size_t height)
			: mWidth{ width }, mHeight{ height }
		{
			this->mPixels = new float[width * height];
		}
	};
}