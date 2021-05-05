#pragma once

#include <cassert>
#include <cmath>

#include "Vector.h"
#include "Matrix.h"

namespace culling
{
	inline void NormalizePlane(Vector4& plane)
	{
		float mag = std::sqrt(plane[0] * plane[0] + plane[1] * plane[1] + plane[2] * plane[2]);
		assert(std::isnan(mag) == false);

		plane[0] = plane[0] / mag;
		plane[1] = plane[1] / mag;
		plane[2] = plane[2] / mag;
		plane[3] = plane[3] / mag;
	}

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
	FORCE_INLINE void ExtractPlanesFromVIewProjectionMatrix(const Matrix4X4& viewProjectionMatrix, Vector4* sixPlanes, bool normalize) noexcept
	{
		sixPlanes[0][0] = viewProjectionMatrix[0][3] + viewProjectionMatrix[0][0];
		sixPlanes[0][1] = viewProjectionMatrix[1][3] + viewProjectionMatrix[1][0];
		sixPlanes[0][2] = viewProjectionMatrix[2][3] + viewProjectionMatrix[2][0];
		sixPlanes[0][3] = viewProjectionMatrix[3][3] + viewProjectionMatrix[3][0];
		// Right clipping plane
		sixPlanes[1][0] = viewProjectionMatrix[0][3] - viewProjectionMatrix[0][0];
		sixPlanes[1][1] = viewProjectionMatrix[1][3] - viewProjectionMatrix[1][0];
		sixPlanes[1][2] = viewProjectionMatrix[2][3] - viewProjectionMatrix[2][0];
		sixPlanes[1][3] = viewProjectionMatrix[3][3] - viewProjectionMatrix[3][0];
		// Top clipping plane
		sixPlanes[2][0] = viewProjectionMatrix[0][3] - viewProjectionMatrix[0][1];
		sixPlanes[2][1] = viewProjectionMatrix[1][3] - viewProjectionMatrix[1][1];
		sixPlanes[2][2] = viewProjectionMatrix[2][3] - viewProjectionMatrix[2][1];
		sixPlanes[2][3] = viewProjectionMatrix[3][3] - viewProjectionMatrix[3][1];
		// Bottom clipping plane
		sixPlanes[3][0] = viewProjectionMatrix[0][3] + viewProjectionMatrix[0][1];
		sixPlanes[3][1] = viewProjectionMatrix[1][3] + viewProjectionMatrix[1][1];
		sixPlanes[3][2] = viewProjectionMatrix[2][3] + viewProjectionMatrix[2][1];
		sixPlanes[3][3] = viewProjectionMatrix[3][3] + viewProjectionMatrix[3][1];
		// Near clipping plane
		sixPlanes[4][0] = viewProjectionMatrix[0][3] + viewProjectionMatrix[0][2];
		sixPlanes[4][1] = viewProjectionMatrix[1][3] + viewProjectionMatrix[1][2];
		sixPlanes[4][2] = viewProjectionMatrix[2][3] + viewProjectionMatrix[2][2];
		sixPlanes[4][3] = viewProjectionMatrix[3][3] + viewProjectionMatrix[3][2];
		// Far clipping plane
		sixPlanes[5][0] = viewProjectionMatrix[0][3] - viewProjectionMatrix[0][2];
		sixPlanes[5][1] = viewProjectionMatrix[1][3] - viewProjectionMatrix[1][2];
		sixPlanes[5][2] = viewProjectionMatrix[2][3] - viewProjectionMatrix[2][2];
		sixPlanes[5][3] = viewProjectionMatrix[3][3] - viewProjectionMatrix[3][2];

		// Normalize the plane equations, if requested
		if (normalize == true)
		{
			NormalizePlane(sixPlanes[0]);
			NormalizePlane(sixPlanes[1]);
			NormalizePlane(sixPlanes[2]);
			NormalizePlane(sixPlanes[3]);
			NormalizePlane(sixPlanes[4]);
			NormalizePlane(sixPlanes[5]);
		}
	}

	FORCE_INLINE void ExtractSIMDPlanesFromViewProjectionMatrix(const Matrix4X4& viewProjectionMatrix, Vector4* eightPlanes, bool normalize) noexcept
	{

		Vector4 sixPlane[6];

		ExtractPlanesFromVIewProjectionMatrix(viewProjectionMatrix, sixPlane, normalize);

		eightPlanes[0][0] = sixPlane[0][0];
		eightPlanes[0][1] = sixPlane[1][0];
		eightPlanes[0][2] = sixPlane[2][0];
		eightPlanes[0][3] = sixPlane[3][0];

		eightPlanes[1][0] = sixPlane[0][1];
		eightPlanes[1][1] = sixPlane[1][1];
		eightPlanes[1][2] = sixPlane[2][1];
		eightPlanes[1][3] = sixPlane[3][1];

		eightPlanes[2][0] = sixPlane[0][2];
		eightPlanes[2][1] = sixPlane[1][2];
		eightPlanes[2][2] = sixPlane[2][2];
		eightPlanes[2][3] = sixPlane[3][2];

		eightPlanes[3][0] = sixPlane[0][3];
		eightPlanes[3][1] = sixPlane[1][3];
		eightPlanes[3][2] = sixPlane[2][3];
		eightPlanes[3][3] = sixPlane[3][3];

		eightPlanes[4][0] = sixPlane[4][0];
		eightPlanes[4][1] = sixPlane[5][0];
		eightPlanes[4][2] = sixPlane[4][0];
		eightPlanes[4][3] = sixPlane[5][0];

		eightPlanes[5][0] = sixPlane[4][1];
		eightPlanes[5][1] = sixPlane[5][1];
		eightPlanes[5][2] = sixPlane[4][1];
		eightPlanes[5][3] = sixPlane[5][1];

		eightPlanes[6][0] = sixPlane[4][2];
		eightPlanes[6][1] = sixPlane[5][2];
		eightPlanes[6][2] = sixPlane[4][2];
		eightPlanes[6][3] = sixPlane[5][2];

		eightPlanes[7][0] = sixPlane[4][3];
		eightPlanes[7][1] = sixPlane[5][3];
		eightPlanes[7][2] = sixPlane[4][3];
		eightPlanes[7][3] = sixPlane[5][3];

	}

}