#pragma once

#include "../EveryCullingCore.h"

namespace culling
{
	struct VertexData
	{
		const culling::Vec3* mVertices;
		size_t mVerticeCount;

		const std::uint32_t* mIndices;
		size_t mIndiceCount;

		/// <summary>
		/// Vertex Stride ( offset between vertices )
		///	ex) Vertex1.X Vertex1.Y Vertex1.Z Vertex1.UV_X(4byte) Vertex1.UV_Y(4byte) Vertex2.X Vertex2.Y Vertex2.Z
		///		-> Stride is 8byte!
		/// </summary>
		size_t mVertexStride;
	};

}