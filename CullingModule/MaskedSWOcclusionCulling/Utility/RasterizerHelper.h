#pragma once

#include "../../../EveryCullingCore.h"

namespace culling
{
	namespace rasterizerHelper
	{
		EVERYCULLING_FORCE_INLINE extern void GetMiddlePointOfTriangle
		(
			const culling::M256F& TriPointA_X,
			const culling::M256F& TriPointA_Y,
			const culling::M256F& TriPointA_Z,

			const culling::M256F& TriPointB_X,
			const culling::M256F& TriPointB_Y,
			const culling::M256F& TriPointB_Z,

			const culling::M256F& TriPointC_X,
			const culling::M256F& TriPointC_Y,
			const culling::M256F& TriPointC_Z,

			culling::M256F& outLeftMiddlePointC_X,
			culling::M256F& outLeftMiddlePointC_Y,
			culling::M256F& outLeftMiddlePointC_Z,

			culling::M256F& outRightMiddlePointC_X,
			culling::M256F& outRightMiddlePointC_Y,
			culling::M256F& outRightMiddlePointC_Z
		)
		{
			const culling::M256F point4X
				= culling::M256F_ADD(TriPointA_X, culling::M256F_MUL(culling::M256F_DIV(culling::M256F_SUB(TriPointB_Y, TriPointA_Y), culling::M256F_SUB(TriPointC_Y, TriPointA_Y)), culling::M256F_SUB(TriPointC_X, TriPointA_X)));

			const culling::M256F point4Y = TriPointB_Y;

			const culling::M256F point4Z
				= culling::M256F_ADD(TriPointA_Z, culling::M256F_MUL(culling::M256F_DIV(culling::M256F_SUB(TriPointB_Y, TriPointA_Y), culling::M256F_SUB(TriPointC_Y, TriPointA_Y)), culling::M256F_SUB(TriPointC_Z, TriPointA_Z)));

			const culling::M256F B4_MASK = _mm256_cmp_ps(TriPointB_X, point4X, _CMP_LE_OQ);
			const culling::M256I B4_MASK_INV_M256I = _mm256_xor_si256(_mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF), *reinterpret_cast<const culling::M256I*>(&B4_MASK));
			const culling::M256F B4_MASK_INV = *reinterpret_cast<const culling::M256F*>(&B4_MASK_INV_M256I);

			outLeftMiddlePointC_X = culling::M256F_SELECT(TriPointB_X, point4X, B4_MASK_INV);
			outLeftMiddlePointC_Y = culling::M256F_SELECT(TriPointB_Y, point4Y, B4_MASK_INV);
			outLeftMiddlePointC_Z = culling::M256F_SELECT(TriPointB_Z, point4Z, B4_MASK_INV);

			outRightMiddlePointC_X = culling::M256F_SELECT(TriPointB_X, point4X, B4_MASK);
			outRightMiddlePointC_Y = culling::M256F_SELECT(TriPointB_Y, point4Y, B4_MASK);
			outRightMiddlePointC_Z = culling::M256F_SELECT(TriPointB_Z, point4Z, B4_MASK);
		}
	};
}
