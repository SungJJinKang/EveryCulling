#include "triangleSlopeEventGetter.h"


void culling::triangleSlopeHelper::GatherBottomFlatTriangleSlopeEvent
(
    const size_t triangleCount,
    const culling::Vec2& TileLeftBottomOriginPoint,
    culling::M256I* const leftFaceEvent,
    culling::M256I* const rightFaceEvent,
    const culling::M256F& TriPointA_X,
    const culling::M256F& TriPointA_Y,

    const culling::M256F& TriPointB_X,
    const culling::M256F& TriPointB_Y,

    const culling::M256F& TriPointC_X,
    const culling::M256F& TriPointC_Y

)
{
    const culling::M256F inverseSlope1 = culling::M256F_DIV(culling::M256F_SUB(TriPointB_X, TriPointA_X), culling::M256F_SUB(TriPointB_Y, TriPointA_Y));
    const culling::M256F inverseSlope2 = culling::M256F_DIV(culling::M256F_SUB(TriPointC_X, TriPointA_X), culling::M256F_SUB(TriPointC_Y, TriPointA_Y));

    const culling::M256F curx1 = culling::M256F_ADD(culling::M256F_SUB(culling::M256F_ADD(TriPointA_X, culling::M256F_MUL(culling::M256F_SUB(culling::M256F_ADD(_mm256_set1_ps(TileLeftBottomOriginPoint.y), _mm256_set1_ps(0.5f)), TriPointA_Y), inverseSlope1)), _mm256_set1_ps(TileLeftBottomOriginPoint.x)), _mm256_set1_ps(0.5f));
    const culling::M256F curx2 = culling::M256F_ADD(culling::M256F_SUB(culling::M256F_ADD(TriPointA_X, culling::M256F_MUL(culling::M256F_SUB(culling::M256F_ADD(_mm256_set1_ps(TileLeftBottomOriginPoint.y), _mm256_set1_ps(0.5f)), TriPointA_Y), inverseSlope2)), _mm256_set1_ps(TileLeftBottomOriginPoint.x)), _mm256_set1_ps(0.5f));
    
    for (size_t triIndex = 0; triIndex < triangleCount; triIndex++)
    {
        culling::M256F leftFaceEventFloat = _mm256_round_ps(_mm256_set_ps(*(reinterpret_cast<const float*>(&curx1) + triIndex), *(reinterpret_cast<const float*>(&curx1) + triIndex) + *(reinterpret_cast<const float*>(&inverseSlope1) + triIndex) * 1.0f, *(reinterpret_cast<const float*>(&curx1) + triIndex) + *(reinterpret_cast<const float*>(&inverseSlope1) + triIndex) * 2.0f, *(reinterpret_cast<const float*>(&curx1) + triIndex) + *(reinterpret_cast<const float*>(&inverseSlope1) + triIndex) * 3.0f, *(reinterpret_cast<const float*>(&curx1) + triIndex) + *(reinterpret_cast<const float*>(&inverseSlope1) + triIndex) * 4.0f, *(reinterpret_cast<const float*>(&curx1) + triIndex) + *(reinterpret_cast<const float*>(&inverseSlope1) + triIndex) * 5.0f, *(reinterpret_cast<const float*>(&curx1) + triIndex) + *(reinterpret_cast<const float*>(&inverseSlope1) + triIndex) * 6.0f, *(reinterpret_cast<const float*>(&curx1) + triIndex) + *(reinterpret_cast<const float*>(&inverseSlope1) + triIndex) * 7.0f), (_MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC));
        leftFaceEvent[triIndex] = _mm256_cvtps_epi32(leftFaceEventFloat);
        leftFaceEvent[triIndex] = _mm256_max_epi32(leftFaceEvent[triIndex], _mm256_set1_epi32(0));
    }
    
    for (size_t triIndex = 0; triIndex < triangleCount; triIndex++)
    {
        culling::M256F rightFaceEventFloat = _mm256_round_ps(_mm256_set_ps(*(reinterpret_cast<const float*>(&curx2) + triIndex), *(reinterpret_cast<const float*>(&curx2) + triIndex) + *(reinterpret_cast<const float*>(&inverseSlope2) + triIndex) * 1.0f, *(reinterpret_cast<const float*>(&curx2) + triIndex) + *(reinterpret_cast<const float*>(&inverseSlope2) + triIndex) * 2.0f, *(reinterpret_cast<const float*>(&curx2) + triIndex) + *(reinterpret_cast<const float*>(&inverseSlope2) + triIndex) * 3.0f, *(reinterpret_cast<const float*>(&curx2) + triIndex) + *(reinterpret_cast<const float*>(&inverseSlope2) + triIndex) * 4.0f, *(reinterpret_cast<const float*>(&curx2) + triIndex) + *(reinterpret_cast<const float*>(&inverseSlope2) + triIndex) * 5.0f, *(reinterpret_cast<const float*>(&curx2) + triIndex) + *(reinterpret_cast<const float*>(&inverseSlope2) + triIndex) * 6.0f, *(reinterpret_cast<const float*>(&curx2) + triIndex) + *(reinterpret_cast<const float*>(&inverseSlope2) + triIndex) * 7.0f), (_MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC));
        rightFaceEvent[triIndex] = _mm256_cvtps_epi32(rightFaceEventFloat);
        rightFaceEvent[triIndex] = _mm256_max_epi32(rightFaceEvent[triIndex], _mm256_set1_epi32(0));
    }
}

void culling::triangleSlopeHelper::GatherTopFlatTriangleSlopeEvent
(
    const size_t triangleCount,
    const culling::Vec2& TileLeftBottomOriginPoint,
    culling::M256I* const leftFaceEvent,
    culling::M256I* const rightFaceEvent,
    const culling::M256F& TriPointA_X,
    const culling::M256F& TriPointA_Y,

    const culling::M256F& TriPointB_X,
    const culling::M256F& TriPointB_Y,

    const culling::M256F& TriPointC_X,
    const culling::M256F& TriPointC_Y

)
{
    const culling::M256F inverseSlope1 = culling::M256F_DIV(culling::M256F_SUB(TriPointA_X, TriPointC_X), culling::M256F_SUB(TriPointA_Y, TriPointC_Y));
    const culling::M256F inverseSlope2 = culling::M256F_DIV(culling::M256F_SUB(TriPointB_X, TriPointC_X), culling::M256F_SUB(TriPointB_Y, TriPointC_Y));

    const culling::M256F curx1 = culling::M256F_ADD(culling::M256F_SUB(culling::M256F_ADD(TriPointC_X, culling::M256F_MUL(culling::M256F_SUB(culling::M256F_ADD(_mm256_set1_ps(TileLeftBottomOriginPoint.y), _mm256_set1_ps(0.5f)), TriPointC_Y), inverseSlope1)), _mm256_set1_ps(TileLeftBottomOriginPoint.x)), _mm256_set1_ps(0.5f));
    const culling::M256F curx2 = culling::M256F_ADD(culling::M256F_SUB(culling::M256F_ADD(TriPointC_X, culling::M256F_MUL(culling::M256F_SUB(culling::M256F_ADD(_mm256_set1_ps(TileLeftBottomOriginPoint.y), _mm256_set1_ps(0.5f)), TriPointC_Y), inverseSlope2)), _mm256_set1_ps(TileLeftBottomOriginPoint.x)), _mm256_set1_ps(0.5f));
    
    for (size_t triIndex = 0; triIndex < triangleCount; triIndex++)
    {
        culling::M256F leftFaceEventFloat = _mm256_round_ps(_mm256_set_ps(*(reinterpret_cast<const float*>(&curx1) + triIndex), *(reinterpret_cast<const float*>(&curx1) + triIndex) + *(reinterpret_cast<const float*>(&inverseSlope1) + triIndex) * 1.0f, *(reinterpret_cast<const float*>(&curx1) + triIndex) + *(reinterpret_cast<const float*>(&inverseSlope1) + triIndex) * 2.0f, *(reinterpret_cast<const float*>(&curx1) + triIndex) + *(reinterpret_cast<const float*>(&inverseSlope1) + triIndex) * 3.0f, *(reinterpret_cast<const float*>(&curx1) + triIndex) + *(reinterpret_cast<const float*>(&inverseSlope1) + triIndex) * 4.0f, *(reinterpret_cast<const float*>(&curx1) + triIndex) + *(reinterpret_cast<const float*>(&inverseSlope1) + triIndex) * 5.0f, *(reinterpret_cast<const float*>(&curx1) + triIndex) + *(reinterpret_cast<const float*>(&inverseSlope1) + triIndex) * 6.0f, *(reinterpret_cast<const float*>(&curx1) + triIndex) + *(reinterpret_cast<const float*>(&inverseSlope1) + triIndex) * 7.0f), (_MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC));
        leftFaceEvent[triIndex] = _mm256_cvtps_epi32(leftFaceEventFloat);
        leftFaceEvent[triIndex] = _mm256_max_epi32(leftFaceEvent[triIndex], _mm256_set1_epi32(0));
    }
    
    for (size_t triIndex = 0; triIndex < triangleCount; triIndex++)
    {
        culling::M256F rightFaceEventFloat = _mm256_round_ps(_mm256_set_ps(*(reinterpret_cast<const float*>(&curx2) + triIndex), *(reinterpret_cast<const float*>(&curx2) + triIndex) + *(reinterpret_cast<const float*>(&inverseSlope2) + triIndex) * 1.0f, *(reinterpret_cast<const float*>(&curx2) + triIndex) + *(reinterpret_cast<const float*>(&inverseSlope2) + triIndex) * 2.0f, *(reinterpret_cast<const float*>(&curx2) + triIndex) + *(reinterpret_cast<const float*>(&inverseSlope2) + triIndex) * 3.0f, *(reinterpret_cast<const float*>(&curx2) + triIndex) + *(reinterpret_cast<const float*>(&inverseSlope2) + triIndex) * 4.0f, *(reinterpret_cast<const float*>(&curx2) + triIndex) + *(reinterpret_cast<const float*>(&inverseSlope2) + triIndex) * 5.0f, *(reinterpret_cast<const float*>(&curx2) + triIndex) + *(reinterpret_cast<const float*>(&inverseSlope2) + triIndex) * 6.0f, *(reinterpret_cast<const float*>(&curx2) + triIndex) + *(reinterpret_cast<const float*>(&inverseSlope2) + triIndex) * 7.0f), (_MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC));
        rightFaceEvent[triIndex] = _mm256_cvtps_epi32(rightFaceEventFloat);
        rightFaceEvent[triIndex] = _mm256_max_epi32(rightFaceEvent[triIndex], _mm256_set1_epi32(0));
    }
}