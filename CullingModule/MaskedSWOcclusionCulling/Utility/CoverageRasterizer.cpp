#include "CoverageRasterizer.h"

#include <cmath>
#include <limits>

culling::EVERYCULLING_M256I culling::CoverageRasterizer::FillBottomFlatTriangle
(
    const Vec2& TileLeftBottomOriginPoint, 
    const Vec2& point1, 
    const Vec2& point2, 
    const Vec2& point3
)
{
    assert(point2.x <= point3.x);
    assert(std::abs(point2.y - point3.y) < std::numeric_limits<float>::epsilon());
    assert(point1.y >= point2.y);
    assert(point1.y >= point3.y);

    const float inverseSlope1 = (float)(point2.x - point1.x) / (point2.y - point1.y);
    const float inverseSlope2 = (float)(point3.x - point1.x) / (point3.y - point1.y);

    const float curx1 = point1.x + (TileLeftBottomOriginPoint.y + 0.5f - point1.y) * inverseSlope1 - TileLeftBottomOriginPoint.x + 0.5f;
    const float curx2 = point1.x + (TileLeftBottomOriginPoint.y + 0.5f - point1.y) * inverseSlope2 - TileLeftBottomOriginPoint.x + 0.5f;

    const culling::EVERYCULLING_M256F leftFaceEventFloat = _mm256_round_ps(_mm256_setr_ps(curx1, curx1 + inverseSlope1 * 1.0f, curx1 + inverseSlope1 * 2.0f, curx1 + inverseSlope1 * 3.0f, curx1 + inverseSlope1 * 4.0f, curx1 + inverseSlope1 * 5.0f, curx1 + inverseSlope1 * 6.0f, curx1 + inverseSlope1 * 7.0f), (_MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC));
    culling::EVERYCULLING_M256I leftFaceEvent = _mm256_cvtps_epi32(leftFaceEventFloat);
    leftFaceEvent = _mm256_max_epi32(leftFaceEvent, _mm256_set1_epi32(0));

    const culling::EVERYCULLING_M256F rightFaceEventFloat = _mm256_round_ps(_mm256_setr_ps(curx2, curx2 + inverseSlope2 * 1.0f, curx2 + inverseSlope2 * 2.0f, curx2 + inverseSlope2 * 3.0f, curx2 + inverseSlope2 * 4.0f, curx2 + inverseSlope2 * 5.0f, curx2 + inverseSlope2 * 6.0f, curx2 + inverseSlope2 * 7.0f), (_MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC));
    culling::EVERYCULLING_M256I rightFaceEvent = _mm256_cvtps_epi32(rightFaceEventFloat);
    rightFaceEvent = _mm256_max_epi32(rightFaceEvent, _mm256_set1_epi32(0));

    culling::EVERYCULLING_M256I Mask1 = _mm256_srlv_epi32(_mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF), leftFaceEvent);
    culling::EVERYCULLING_M256I Mask2 = _mm256_srlv_epi32(_mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF), rightFaceEvent);
    
    culling::EVERYCULLING_M256I Result = _mm256_and_si256(Mask1, _mm256_xor_si256(Mask2, _mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF)));

    const culling::EVERYCULLING_M256F aboveFlatBottomTriangleFace = _mm256_cmp_ps(_mm256_setr_ps(TileLeftBottomOriginPoint.y, TileLeftBottomOriginPoint.y + 1.0f, TileLeftBottomOriginPoint.y + 2.0f, TileLeftBottomOriginPoint.y + 3.0f, TileLeftBottomOriginPoint.y + 4.0f, TileLeftBottomOriginPoint.y + 5.0f, TileLeftBottomOriginPoint.y + 6.0f, TileLeftBottomOriginPoint.y + 7.0f), _mm256_set1_ps(point2.y), _CMP_GE_OQ);

    Result = _mm256_and_si256(Result, *reinterpret_cast<const culling::EVERYCULLING_M256I*>(&aboveFlatBottomTriangleFace));

	return Result;
}

culling::EVERYCULLING_M256I culling::CoverageRasterizer::FillTopFlatTriangle
(
    const Vec2& TileLeftBottomOriginPoint, 
    const Vec2& point1, 
    const Vec2& point2, 
    const Vec2& point3
)
{
    assert(point1.x <= point2.x);
    assert(std::abs(point1.y - point2.y) < std::numeric_limits<float>::epsilon());
    assert(point1.y >= point3.y);
    assert(point2.y >= point3.y);
    
    const float inverseSlope1 = (float)(point1.x - point3.x) / (point1.y - point3.y);
    const float inverseSlope2 = (float)(point2.x - point3.x) / (point2.y - point3.y);

    const float curx1 = point3.x + (TileLeftBottomOriginPoint.y + 0.5f - point3.y) * inverseSlope1 - TileLeftBottomOriginPoint.x + 0.5f;
    const float curx2 = point3.x + (TileLeftBottomOriginPoint.y + 0.5f - point3.y) * inverseSlope2 - TileLeftBottomOriginPoint.x + 0.5f;

    const culling::EVERYCULLING_M256F leftFaceEventFloat = _mm256_round_ps(_mm256_setr_ps(curx1, curx1 + inverseSlope1 * 1.0f, curx1 + inverseSlope1 * 2.0f, curx1 + inverseSlope1 * 3.0f, curx1 + inverseSlope1 * 4.0f, curx1 + inverseSlope1 * 5.0f, curx1 + inverseSlope1 * 6.0f, curx1 + inverseSlope1 * 7.0f), (_MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC));
    culling::EVERYCULLING_M256I leftFaceEvent = _mm256_cvtps_epi32(leftFaceEventFloat);
    leftFaceEvent = _mm256_max_epi32(leftFaceEvent, _mm256_set1_epi32(0));

    const culling::EVERYCULLING_M256F rightFaceEventFloat = _mm256_round_ps(_mm256_setr_ps(curx2, curx2 + inverseSlope2 * 1.0f, curx2 + inverseSlope2 * 2.0f, curx2 + inverseSlope2 * 3.0f, curx2 + inverseSlope2 * 4.0f, curx2 + inverseSlope2 * 5.0f, curx2 + inverseSlope2 * 6.0f, curx2 + inverseSlope2 * 7.0f), (_MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC));
    culling::EVERYCULLING_M256I rightFaceEvent = _mm256_cvtps_epi32(rightFaceEventFloat);
    rightFaceEvent = _mm256_max_epi32(rightFaceEvent, _mm256_set1_epi32(0));

    culling::EVERYCULLING_M256I Mask1 = _mm256_srlv_epi32(_mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF), leftFaceEvent);
    culling::EVERYCULLING_M256I Mask2 = _mm256_srlv_epi32(_mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF), rightFaceEvent);
    
    culling::EVERYCULLING_M256I Result = _mm256_and_si256(Mask1, _mm256_xor_si256(Mask2, _mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF)));

    const culling::EVERYCULLING_M256F belowFlatTopTriangleFace = _mm256_cmp_ps(_mm256_setr_ps(TileLeftBottomOriginPoint.y, TileLeftBottomOriginPoint.y + 1.0f, TileLeftBottomOriginPoint.y + 2.0f, TileLeftBottomOriginPoint.y + 3.0f, TileLeftBottomOriginPoint.y + 4.0f, TileLeftBottomOriginPoint.y + 5.0f, TileLeftBottomOriginPoint.y + 6.0f, TileLeftBottomOriginPoint.y + 7.0f), _mm256_set1_ps(point1.y), _CMP_LE_OQ);

    Result = _mm256_and_si256(Result, *reinterpret_cast<const culling::EVERYCULLING_M256I*>(&belowFlatTopTriangleFace));

    return Result;
}

culling::EVERYCULLING_M256I culling::CoverageRasterizer::FillTriangle
(
    const Vec2& TileLeftBottomOriginPoint,
	const culling::Vec2& triangleVertex1, 
    const culling::Vec2& triangleVertex2, 
    const culling::Vec2& triangleVertex3
)
{
    culling::EVERYCULLING_M256I result;
    if (std::abs(triangleVertex2.y - triangleVertex3.y) < std::numeric_limits<float>::epsilon())
    {// Bottom Flat Triangle
        result = FillBottomFlatTriangle
        (
            TileLeftBottomOriginPoint,
            triangleVertex1,
            ((triangleVertex2.x < triangleVertex3.x) ? triangleVertex2 : triangleVertex3),
            ((triangleVertex2.x < triangleVertex3.x) ? triangleVertex3 : triangleVertex2)
        );
    }
    // check for trivial case of top-flat triangle
    else if (std::abs(triangleVertex1.y - triangleVertex2.y) < std::numeric_limits<float>::epsilon())
    {// Top Flat Triangle
        result = FillTopFlatTriangle
        (
            TileLeftBottomOriginPoint,
            ((triangleVertex1.x < triangleVertex2.x) ? triangleVertex1 : triangleVertex2),
            ((triangleVertex1.x < triangleVertex2.x) ? triangleVertex2 : triangleVertex1),
            triangleVertex3
        );
    }
    else
    {
        // http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
        // general case - split the triangle in a topflat and bottom-flat one
        culling::Vec2 point4{ triangleVertex1.x + ((float)(triangleVertex2.y - triangleVertex1.y) / (float)(triangleVertex3.y - triangleVertex1.y)) * (triangleVertex3.x - triangleVertex1.x), triangleVertex2.y };
      
    	culling::EVERYCULLING_M256I Result1, Result2;

        assert(triangleVertex1.y > triangleVertex2.y);
        assert(triangleVertex1.y > triangleVertex3.y);
        
        Result1 = FillBottomFlatTriangle
    	(
            TileLeftBottomOriginPoint, 
            triangleVertex1, 
            (triangleVertex2.x < point4.x) ? triangleVertex2 : point4, 
            (triangleVertex2.x < point4.x) ? point4 : triangleVertex2
        );


        assert(triangleVertex3.y < triangleVertex1.y);
        assert(triangleVertex3.y < triangleVertex2.y);

        Result2 = FillTopFlatTriangle
    	(
            TileLeftBottomOriginPoint, 
            (triangleVertex2.x < point4.x) ? triangleVertex2 : point4,
            (triangleVertex2.x < point4.x) ? point4 : triangleVertex2,
            triangleVertex3
        );
        
        result = _mm256_or_si256(Result1, Result2);
    }

    return result;
}


/// <summary>
/// reference : http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
/// </summary>
/// <param name="coverageMask"></param>
/// <param name="LeftBottomPoint"></param>
/// <param name="triangle"></param>
culling::EVERYCULLING_M256I culling::CoverageRasterizer::FillTriangle
(
    const Vec2& LeftBottomPoint, 
    TwoDTriangle& triangle
)
{
    SortTriangle(triangle);

    return FillTriangle(LeftBottomPoint, triangle.Points[0], triangle.Points[1], triangle.Points[2]);

}

culling::EVERYCULLING_M256I culling::CoverageRasterizer::FillTriangle
(
    const Vec2& LeftBottomPoint, 
    ThreeDTriangle& triangle
)
{
    SortTriangle(triangle);

    return FillTriangle(LeftBottomPoint, *reinterpret_cast<culling::Vec2*>(triangle.Points + 0), *reinterpret_cast<culling::Vec2*>(triangle.Points + 1), *reinterpret_cast<culling::Vec2*>(triangle.Points + 2));
}



/*
void culling::CoverageRasterizer::FillTriangleBatch
(
    const size_t triangleCount,
    culling::EVERYCULLING_M256I* const outCoverageMask, // 8 coverage mask. array size should be 8
    const culling::Vec2& TileLeftBottomOriginPoint,
    culling::EVERYCULLING_M256F& TriPointA_X,
    culling::EVERYCULLING_M256F& TriPointA_Y,

    culling::EVERYCULLING_M256F& TriPointB_X,
    culling::EVERYCULLING_M256F& TriPointB_Y,

    culling::EVERYCULLING_M256F& TriPointC_X,
    culling::EVERYCULLING_M256F& TriPointC_Y
)
{
    Sort_8_2DTriangles(TriPointA_X, TriPointA_Y, TriPointB_X, TriPointB_Y, TriPointC_X, TriPointC_Y);

    // http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
        // general case - split the triangle in a topflat and bottom-flat one
    const culling::EVERYCULLING_M256F point4X
        = culling::EVERYCULLING_M256F_ADD(TriPointA_X, culling::EVERYCULLING_M256F_MUL(culling::EVERYCULLING_M256F_DIV(culling::EVERYCULLING_M256F_SUB(TriPointB_Y, TriPointA_Y), culling::EVERYCULLING_M256F_SUB(TriPointC_Y, TriPointA_Y)), culling::EVERYCULLING_M256F_SUB(TriPointC_X, TriPointA_X)));

    const culling::EVERYCULLING_M256F point4Y = TriPointB_Y;
    
    culling::EVERYCULLING_M256I Result1[8], Result2[8];
    

    const culling::EVERYCULLING_M256F B4_MASK = _mm256_cmp_ps(TriPointB_X, point4X, _CMP_LE_OQ);
    const culling::EVERYCULLING_M256I B4_MASK_INV_M256I = _mm256_xor_si256(_mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF), *reinterpret_cast<const culling::EVERYCULLING_M256I*>(&B4_MASK));
    const culling::EVERYCULLING_M256F B4_MASK_INV = *reinterpret_cast<const culling::EVERYCULLING_M256F*>(&B4_MASK_INV_M256I);

    const culling::EVERYCULLING_M256F B4_MIN_X = culling::EVERYCULLING_M256F_SELECT(TriPointB_X, point4X, B4_MASK_INV);
    const culling::EVERYCULLING_M256F B4_MIN_Y = culling::EVERYCULLING_M256F_SELECT(TriPointB_Y, point4Y, B4_MASK_INV);

    const culling::EVERYCULLING_M256F B4_MAX_X = culling::EVERYCULLING_M256F_SELECT(TriPointB_X, point4X, B4_MASK);
    const culling::EVERYCULLING_M256F B4_MAX_Y = culling::EVERYCULLING_M256F_SELECT(TriPointB_Y, point4Y, B4_MASK);


	FillBottomFlatTriangleBatch
    (
        triangleCount,
        Result1,
        TileLeftBottomOriginPoint,

        TriPointA_X,
        TriPointA_Y,

        B4_MIN_X,
        B4_MIN_Y,

        B4_MAX_X,
        B4_MAX_Y
    );

    
    FillTopFlatTriangleBatch
    (
        triangleCount,
        Result2,
        TileLeftBottomOriginPoint,

        B4_MIN_X,
        B4_MIN_Y,

        B4_MAX_X,
        B4_MAX_Y,

        TriPointC_X,
        TriPointC_Y
    );


    
    for(size_t triangleIndex = 0 ; triangleIndex < triangleCount; triangleIndex++)
    {
        outCoverageMask[triangleIndex] = _mm256_or_si256(Result1[triangleIndex], Result2[triangleIndex]);
    }
    
 
}
*/

