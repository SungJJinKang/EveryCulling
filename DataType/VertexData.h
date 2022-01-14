#pragma once

#include "../EveryCullingCore.h"

namespace culling
{
	struct VertexData
	{
		std::atomic<std::uint32_t> mBinnedIndiceCount; //  4byte + ??

		const culling::Vec3* mVertices; // 8byte or 4byte
		std::uint64_t mVerticeCount; // 8byte

		const std::uint32_t* mIndices; // 8byte or 4byte
		std::uint64_t mIndiceCount; // 8byte

		/// <summary>
		/// Vertex Stride ( offset between vertices )
		///	ex) Vertex1.X Vertex1.Y Vertex1.Z Vertex1.UV_X(4byte) Vertex1.UV_Y(4byte) Vertex2.X Vertex2.Y Vertex2.Z
		///		-> Stride is 8byte!
		/// </summary>
		std::uint64_t mVertexStride; // 8byte
		char padding[32]; // to prevent false sharing
		
		EVERYCULLING_FORCE_INLINE void Reset(const unsigned long long currentTickCount)
		{
			if(currentTickCount % 2 == 0)
			{
				// Clear binned triangle 
				mBinnedIndiceCount = 0;
			}
			
		}
	};

}