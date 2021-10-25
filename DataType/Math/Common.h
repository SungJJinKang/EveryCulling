#pragma once

#include "../../EveryCullingCore.h"
#include "Vector.h"
#include "Matrix.h"

namespace culling
{
	void NormalizePlane(Vec4& plane) noexcept;

	/// <summary>
		/// Extract 6 Planes ( 8 Planes ) From MVPMatrix
		/// SIMD Version
		///  sixPlanes[0] : x of Plane0, y of Plane0, z of Plane0, w of Plane0
		///  sixPlanes[1] : x of Plane1, y of Plane1, z of Plane1, w of Plane1
		///  sixPlanes[2] : x of Plane2, y of Plane2, z of Plane2, w of Plane2
		///  sixPlanes[3] : x of Plane3, y of Plane3, z of Plane3, w of Plane3
		///  sixPlanes[4] : x of Plane4, y of Plane4, z of Plane4, w of Plane4
		///  sixPlanes[5] : x of Plane5, y of Plane5, z of Plane5, w of Plane5
		/// references :
		/// https://www.slideshare.net/DICEStudio/culling-the-battlefield-data-oriented-design-in-practice
		/// https://www.gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf
		/// https://macton.smugmug.com/Other/2008-07-15-by-Eye-Fi/n-xmKDH/i-bJq8JqZ/A
		/// </summary>
	[[nodiscard]] void ExtractPlanesFromVIewProjectionMatrix(const Mat4x4& viewProjectionMatrix, Vec4* sixPlanes, bool normalize) noexcept;

	[[nodiscard]] void ExtractSIMDPlanesFromViewProjectionMatrix(const Mat4x4& viewProjectionMatrix, Vec4* eightPlanes, bool normalize) noexcept;

	[[nodiscard]] FORCE_INLINE Vec4 operator*(const culling::Mat4x4& mat4, const culling::Vec3& vec3) noexcept
	{
		return Vec4
		{
				mat4[0][0] * vec3.x + mat4[1][0] * vec3.y + mat4[2][0] * vec3.z + mat4[3][0],
				mat4[0][1] * vec3.x + mat4[1][1] * vec3.y + mat4[2][1] * vec3.z + mat4[3][1],
				mat4[0][2] * vec3.x + mat4[1][2] * vec3.y + mat4[2][2] * vec3.z + mat4[3][2],
				mat4[0][3] * vec3.x + mat4[1][3] * vec3.y + mat4[2][3] * vec3.z + mat4[3][3],
		};
	}


	template <typename T>
	void SWAP(T& a, T& b) noexcept
	{
		const T original = a;
		a = b;
		b = original;
	}

}