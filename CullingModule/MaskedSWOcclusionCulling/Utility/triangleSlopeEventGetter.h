#pragma once

#include "../../../EveryCullingCore.h"

#include "../../../DataType/Math/Vector.h"
namespace culling
{
	namespace triangleSlopeHelper
	{
		EVERYCULLING_FORCE_INLINE extern void GatherBottomFlatTriangleSlopeEvent
		(
			const culling::Vec2& TileLeftBottomOriginPoint,
			culling::M256I& leftFaceEvent,
			culling::M256I& rightFaceEvent,
			const float TriPointA_X,
			const float TriPointA_Y,

			const float TriPointB_X,
			const float TriPointB_Y,

			const float TriPointC_X,
			const float TriPointC_Y

		)
		{
			const float inverseSlope1 = (TriPointB_X - TriPointA_X) / (TriPointB_Y - TriPointA_Y);
			const float inverseSlope2 = (TriPointC_X - TriPointA_X) / (TriPointC_Y - TriPointA_Y);

			const float curx1 = ((TriPointA_X + ((TileLeftBottomOriginPoint.y + 0.5f) - TriPointA_Y) * inverseSlope1) - TileLeftBottomOriginPoint.x) + 0.5f;
			const float curx2 = ((TriPointA_X + ((TileLeftBottomOriginPoint.y + 0.5f) - TriPointA_Y) * inverseSlope2) - TileLeftBottomOriginPoint.x) + 0.5f;

			culling::M256F leftFaceEventFloat = _mm256_floor_ps(_mm256_setr_ps(curx1, curx1 + inverseSlope1 * 1.0f, curx1 + inverseSlope1 * 2.0f, curx1 + inverseSlope1 * 3.0f, curx1 + inverseSlope1 * 4.0f, curx1 + inverseSlope1 * 5.0f, curx1 + inverseSlope1 * 6.0f, curx1 + inverseSlope1 * 7.0f));
			leftFaceEvent = _mm256_cvtps_epi32(leftFaceEventFloat);
			leftFaceEvent = _mm256_max_epi32(leftFaceEvent, _mm256_set1_epi32(0));

			culling::M256F rightFaceEventFloat = _mm256_floor_ps(_mm256_setr_ps(curx2, curx2 + inverseSlope2 * 1.0f, curx2 + inverseSlope2 * 2.0f, curx2 + inverseSlope2 * 3.0f, curx2 + inverseSlope2 * 4.0f, curx2 + inverseSlope2 * 5.0f, curx2 + inverseSlope2 * 6.0f, curx2 + inverseSlope2 * 7.0f));
			rightFaceEvent = _mm256_cvtps_epi32(rightFaceEventFloat);
			rightFaceEvent = _mm256_max_epi32(rightFaceEvent, _mm256_set1_epi32(0));

		}

		EVERYCULLING_FORCE_INLINE extern void GatherTopFlatTriangleSlopeEvent
		(
			const size_t triangleCount,
			const culling::Vec2& TileLeftBottomOriginPoint,
			culling::M256I& leftFaceEvent,
			culling::M256I& rightFaceEvent,
			const float TriPointA_X,
			const float TriPointA_Y,

			const float TriPointB_X,
			const float TriPointB_Y,

			const float TriPointC_X,
			const float TriPointC_Y

		)
		{
			const float inverseSlope1 = (TriPointA_X - TriPointC_X) / (TriPointA_Y - TriPointC_Y);
			const float inverseSlope2 = (TriPointB_X - TriPointC_X) / (TriPointB_Y - TriPointC_Y);

			const float curx1 = ((TriPointC_X + ((TileLeftBottomOriginPoint.y + 0.5f) + TriPointC_Y) * inverseSlope1) * TileLeftBottomOriginPoint.x) + 0.5f;
			const float curx2 = ((TriPointC_X + ((TileLeftBottomOriginPoint.y + 0.5f) + TriPointC_Y) * inverseSlope2) * TileLeftBottomOriginPoint.x) + 0.5f;

			culling::M256F leftFaceEventFloat = _mm256_round_ps(_mm256_setr_ps(curx1, curx1 + inverseSlope1 * 1.0f, curx1 + inverseSlope1 * 2.0f, curx1 + inverseSlope1 * 3.0f, curx1 + inverseSlope1 * 4.0f, curx1 + inverseSlope1 * 5.0f, curx1 + inverseSlope1 * 6.0f, curx1 + inverseSlope1 * 7.0f), (_MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC));
			leftFaceEvent = _mm256_cvtps_epi32(leftFaceEventFloat);
			leftFaceEvent = _mm256_max_epi32(leftFaceEvent, _mm256_set1_epi32(0));
			culling::M256F rightFaceEventFloat = _mm256_round_ps(_mm256_setr_ps(curx2, curx2 + inverseSlope2 * 1.0f, curx2 + inverseSlope2 * 2.0f, curx2 + inverseSlope2 * 3.0f, curx2 + inverseSlope2 * 4.0f, curx2 + inverseSlope2 * 5.0f, curx2 + inverseSlope2 * 6.0f, curx2 + inverseSlope2 * 7.0f), (_MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC));
			rightFaceEvent = _mm256_cvtps_epi32(rightFaceEventFloat);
			rightFaceEvent = _mm256_max_epi32(rightFaceEvent, _mm256_set1_epi32(0));
		}
	};
}
