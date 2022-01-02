#pragma once

#include "../../EveryCullingCore.h"
#include "SIMD_Core.h"
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
	NO_DISCARD void ExtractPlanesFromVIewProjectionMatrix(const Mat4x4& viewProjectionMatrix, Vec4* sixPlanes, bool normalize) noexcept;

	NO_DISCARD void ExtractSIMDPlanesFromViewProjectionMatrix(const Mat4x4& viewProjectionMatrix, Vec4* eightPlanes, bool normalize) noexcept;

	NO_DISCARD FORCE_INLINE Vec4 operator*(const culling::Mat4x4& mat4, const culling::Vec3& vec3) noexcept
	{
		return Vec4
		{
				mat4[0][0] * vec3.x + mat4[1][0] * vec3.y + mat4[2][0] * vec3.z + mat4[3][0],
				mat4[0][1] * vec3.x + mat4[1][1] * vec3.y + mat4[2][1] * vec3.z + mat4[3][1],
				mat4[0][2] * vec3.x + mat4[1][2] * vec3.y + mat4[2][2] * vec3.z + mat4[3][2],
				mat4[0][3] * vec3.x + mat4[1][3] * vec3.y + mat4[2][3] * vec3.z + mat4[3][3],
		};
	}

	NO_DISCARD FORCE_INLINE Vec4 operator*(const culling::Mat4x4& mat4, const culling::Vec4& vec4) noexcept
	{
		culling::M128F tempVec4;

		const culling::M128F* A = reinterpret_cast<const culling::M128F*>(&mat4);
		const culling::M128F* B = reinterpret_cast<const culling::M128F*>(&vec4);
		culling::M128F* R = reinterpret_cast<culling::M128F*>(&tempVec4);

		// First row of result (Matrix1[0] * Matrix2).
		*R = culling::M128F_MUL(M128F_REPLICATE(*B, 0), A[0]);
		*R = culling::M128F_MUL_AND_ADD(M128F_REPLICATE(*B, 1), A[1], *R);
		*R = culling::M128F_MUL_AND_ADD(M128F_REPLICATE(*B, 2), A[2], *R);
		*R = culling::M128F_MUL_AND_ADD(M128F_REPLICATE(*B, 3), A[3], *R);

		return Vec4{ *(Vec4*)(&tempVec4) };
	}

	NO_DISCARD FORCE_INLINE culling::Mat4x4 operator*(const culling::Mat4x4& mat4_A, const culling::Mat4x4& mat4_B) noexcept
	{
		culling::M256F _REULST_MAT4[2];
		culling::M128F TEMP_M128F;

		const culling::M128F* const A = reinterpret_cast<const culling::M128F*>(mat4_A.data());
		//const M128F* A = (const M128F*)this->data(); // this is slower
		const culling::M128F* const B = reinterpret_cast<const culling::M128F*>(mat4_B.data());
		culling::M128F* R = reinterpret_cast<culling::M128F*>(&_REULST_MAT4);

		// First row of result (Matrix1[0] * Matrix2).
		TEMP_M128F = culling::M128F_MUL(M128F_REPLICATE(B[0], 0), A[0]);
		TEMP_M128F = culling::M128F_MUL_AND_ADD(M128F_REPLICATE(B[0], 1), A[1], TEMP_M128F);
		TEMP_M128F = culling::M128F_MUL_AND_ADD(M128F_REPLICATE(B[0], 2), A[2], TEMP_M128F);
		R[0] = culling::M128F_MUL_AND_ADD(M128F_REPLICATE(B[0], 3), A[3], TEMP_M128F);

		// Second row of result (Matrix1[1] * Matrix2).
		TEMP_M128F = culling::M128F_MUL(M128F_REPLICATE(B[1], 0), A[0]);
		TEMP_M128F = culling::M128F_MUL_AND_ADD(M128F_REPLICATE(B[1], 1), A[1], TEMP_M128F);
		TEMP_M128F = culling::M128F_MUL_AND_ADD(M128F_REPLICATE(B[1], 2), A[2], TEMP_M128F);
		R[1] = culling::M128F_MUL_AND_ADD(M128F_REPLICATE(B[1], 3), A[3], TEMP_M128F);

		// Third row of result (Matrix1[2] * Matrix2).
		TEMP_M128F = culling::M128F_MUL(M128F_REPLICATE(B[2], 0), A[0]);
		TEMP_M128F = culling::M128F_MUL_AND_ADD(M128F_REPLICATE(B[2], 1), A[1], TEMP_M128F);
		TEMP_M128F = culling::M128F_MUL_AND_ADD(M128F_REPLICATE(B[2], 2), A[2], TEMP_M128F);
		R[2] = culling::M128F_MUL_AND_ADD(M128F_REPLICATE(B[2], 3), A[3], TEMP_M128F);

		// Fourth row of result (Matrix1[3] * Matrix2).
		TEMP_M128F = culling::M128F_MUL(M128F_REPLICATE(B[3], 0), A[0]);
		TEMP_M128F = culling::M128F_MUL_AND_ADD(M128F_REPLICATE(B[3], 1), A[1], TEMP_M128F);
		TEMP_M128F = culling::M128F_MUL_AND_ADD(M128F_REPLICATE(B[3], 2), A[2], TEMP_M128F);
		R[3] = culling::M128F_MUL_AND_ADD(M128F_REPLICATE(B[3], 3), A[3], TEMP_M128F);

		return culling::Mat4x4{ *reinterpret_cast<culling::Mat4x4*>(&_REULST_MAT4) };
	}

	template <typename T>
	FORCE_INLINE void SWAP(T& a, T& b) noexcept
	{
		const T original = a;
		a = b;
		b = original;
	}

	template <typename T>
	FORCE_INLINE T CLAMP(const T& value, const T& min, const T& max) noexcept
	{
		return MIN(MAX(value, min), max);
	}

	inline extern float PI = 3.14159265358979323846f;

	inline extern float DEGREE_TO_RADIAN = PI / 180.0f;

	inline extern float RADIAN_TO_DEGREE = 180.0f / PI;
}