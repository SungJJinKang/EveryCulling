#include "CoverageRasterizer.h"

#include <cmath>
#include <limits>

#define DEFAULT_COVERAGEMASK_FLAT_FACE_X_OFFSET (TILE_WIDTH) // ((float)(TILE_WIDTH * 2.0f) - 1.0f)
#define DEFAULT_COVERAGEMASK_FLAT_FACE_Y_OFFSET (0.0f) // ((float)(TILE_WIDTH * 2.0f) - 1.0f)

FORCE_INLINE culling::M256I culling::CoverageRasterizer::FillBottomFlatTriangle
(
    const Vec2& TileLeftBottomOriginPoint, 
    const Vec2& point1, 
    const Vec2& point2, 
    const Vec2& point3,
    const float floatFaceXOffset,
    const float floatFaceYOffset
)
{
    assert(point2.x <= point3.x);
    assert(std::abs(point2.y - point3.y) < std::numeric_limits<float>::epsilon());
    assert(point1.y >= point2.y);
    assert(point1.y >= point3.y);

    const float inverseSlope1 = (float)(point2.x - point1.x) / (point2.y - point1.y);
    const float inverseSlope2 = (float)(point3.x - point1.x) / (point3.y - point1.y);

    const float curx1 = point1.x + (TileLeftBottomOriginPoint.y - point1.y) * inverseSlope1 - TileLeftBottomOriginPoint.x - floatFaceXOffset;
    const float curx2 = point1.x + (TileLeftBottomOriginPoint.y - point1.y) * inverseSlope2 - TileLeftBottomOriginPoint.x + floatFaceXOffset;

    const culling::M256F leftFaceEventFloat = _mm256_set_ps(curx1 + inverseSlope1 * 7.0f, curx1 + inverseSlope1 * 6.0f, curx1 + inverseSlope1 * 5.0f, curx1 + inverseSlope1 * 4.0f, curx1 + inverseSlope1 * 3.0f, curx1 + inverseSlope1 * 2.0f, curx1 + inverseSlope1 * 1.0f, curx1 );
    culling::M256I leftFaceEvent = _mm256_cvtps_epi32(leftFaceEventFloat);
    leftFaceEvent = _mm256_max_epi32(leftFaceEvent, _mm256_set1_epi32(0));
    
    const culling::M256F rightFaceEventFloat = _mm256_set_ps(curx2 + inverseSlope2 * 7.0f, curx2 + inverseSlope2 * 6.0f, curx2 + inverseSlope2 * 5.0f, curx2 + inverseSlope2 * 4.0f, curx2 + inverseSlope2 * 3.0f, curx2 + inverseSlope2 * 2.0f, curx2 + inverseSlope2 * 1.0f, curx2 );
    culling::M256I rightFaceEvent = _mm256_cvtps_epi32(rightFaceEventFloat);
    rightFaceEvent = _mm256_max_epi32(rightFaceEvent, _mm256_set1_epi32(0));

    culling::M256I Mask1 = _mm256_srlv_epi32(_mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF), leftFaceEvent);
    culling::M256I Mask2 = _mm256_srlv_epi32(_mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF), rightFaceEvent);
    
    culling::M256I Result = _mm256_and_si256(Mask1, _mm256_xor_si256(Mask2, _mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF)));

    const culling::M256F aboveFlatBottomTriangleFace = _mm256_cmp_ps(_mm256_set_ps(TileLeftBottomOriginPoint.y, TileLeftBottomOriginPoint.y + 1.0f, TileLeftBottomOriginPoint.y + 2.0f, TileLeftBottomOriginPoint.y + 3.0f, TileLeftBottomOriginPoint.y + 4.0f, TileLeftBottomOriginPoint.y + 5.0f, TileLeftBottomOriginPoint.y + 6.0f, TileLeftBottomOriginPoint.y + 7.0f), _mm256_set1_ps(point2.y - floatFaceYOffset), _CMP_GE_OQ);

    Result = _mm256_and_si256(Result, *reinterpret_cast<const culling::M256I*>(&aboveFlatBottomTriangleFace));

	return Result;
}

FORCE_INLINE culling::M256I culling::CoverageRasterizer::FillTopFlatTriangle
(
    const Vec2& TileLeftBottomOriginPoint, 
    const Vec2& point1, 
    const Vec2& point2, 
    const Vec2& point3,
    const float floatFaceXOffset,
    const float floatFaceYOffset
)
{
    assert(point1.x <= point2.x);
    assert(std::abs(point1.y - point2.y) < std::numeric_limits<float>::epsilon());
    assert(point1.y >= point3.y);
    assert(point2.y >= point3.y);
    
    const float inverseSlope1 = (float)(point1.x - point3.x) / (point1.y - point3.y);
    const float inverseSlope2 = (float)(point2.x - point3.x) / (point2.y - point3.y);

    const float curx1 = point3.x + (TileLeftBottomOriginPoint.y - point3.y) * inverseSlope1 - TileLeftBottomOriginPoint.x - floatFaceXOffset;
    const float curx2 = point3.x + (TileLeftBottomOriginPoint.y - point3.y) * inverseSlope2 - TileLeftBottomOriginPoint.x + floatFaceXOffset;

    const culling::M256F leftFaceEventFloat = _mm256_set_ps(curx1, curx1 + inverseSlope1 * 1.0f, curx1 + inverseSlope1 * 2.0f, curx1 + inverseSlope1 * 3.0f, curx1 + inverseSlope1 * 4.0f, curx1 + inverseSlope1 * 5.0f, curx1 + inverseSlope1 * 6.0f, curx1 + inverseSlope1 * 7.0f);
    culling::M256I leftFaceEvent = _mm256_cvtps_epi32(leftFaceEventFloat);
    leftFaceEvent = _mm256_max_epi32(leftFaceEvent, _mm256_set1_epi32(0));

    const culling::M256F rightFaceEventFloat = _mm256_set_ps(curx2, curx2 + inverseSlope2 * 1.0f, curx2 + inverseSlope2 * 2.0f, curx2 + inverseSlope2 * 3.0f, curx2 + inverseSlope2 * 4.0f, curx2 + inverseSlope2 * 5.0f, curx2 + inverseSlope2 * 6.0f, curx2 + inverseSlope2 * 7.0f);
    culling::M256I rightFaceEvent = _mm256_cvtps_epi32(rightFaceEventFloat);
    rightFaceEvent = _mm256_max_epi32(rightFaceEvent, _mm256_set1_epi32(0));

    culling::M256I Mask1 = _mm256_srlv_epi32(_mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF), leftFaceEvent);
    culling::M256I Mask2 = _mm256_srlv_epi32(_mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF), rightFaceEvent);
    
    culling::M256I Result = _mm256_and_si256(Mask1, _mm256_xor_si256(Mask2, _mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF)));

    const culling::M256F belowFlatTopTriangleFace = _mm256_cmp_ps(_mm256_set_ps(TileLeftBottomOriginPoint.y, TileLeftBottomOriginPoint.y + 1.0f, TileLeftBottomOriginPoint.y + 2.0f, TileLeftBottomOriginPoint.y + 3.0f, TileLeftBottomOriginPoint.y + 4.0f, TileLeftBottomOriginPoint.y + 5.0f, TileLeftBottomOriginPoint.y + 6.0f, TileLeftBottomOriginPoint.y + 7.0f), _mm256_set1_ps(point1.y + floatFaceYOffset), _CMP_LE_OQ);

    Result = _mm256_and_si256(Result, *reinterpret_cast<const culling::M256I*>(&belowFlatTopTriangleFace));

    return Result;
}

void culling::CoverageRasterizer::FillTriangle
(
    culling::Tile& tile, 
    const Vec2& TileLeftBottomOriginPoint,
	const culling::Vec2& triangleVertex1, 
    const culling::Vec2& triangleVertex2, 
    const culling::Vec2& triangleVertex3
)
{
    culling::M256I result;
    if (std::abs(triangleVertex2.y - triangleVertex3.y) < std::numeric_limits<float>::epsilon())
    {// Bottom Flat Triangle
        result = FillBottomFlatTriangle
        (
            TileLeftBottomOriginPoint,
            triangleVertex1,
            ((triangleVertex2.x < triangleVertex3.x) ? triangleVertex2 : triangleVertex3),
            ((triangleVertex2.x < triangleVertex3.x) ? triangleVertex3 : triangleVertex2),
            DEFAULT_COVERAGEMASK_FLAT_FACE_X_OFFSET,
            DEFAULT_COVERAGEMASK_FLAT_FACE_Y_OFFSET
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
            triangleVertex3,
            DEFAULT_COVERAGEMASK_FLAT_FACE_X_OFFSET,
            DEFAULT_COVERAGEMASK_FLAT_FACE_Y_OFFSET
        );
    }
    else
    {
        // http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
        // general case - split the triangle in a topflat and bottom-flat one
        culling::Vec2 point4{ triangleVertex1.x + ((float)(triangleVertex2.y - triangleVertex1.y) / (float)(triangleVertex3.y - triangleVertex1.y)) * (triangleVertex3.x - triangleVertex1.x), triangleVertex2.y };
      
    	culling::M256I Result1, Result2;

        assert(triangleVertex1.y > triangleVertex2.y);
        assert(triangleVertex1.y > triangleVertex3.y);
        
        Result1 = FillBottomFlatTriangle
    	(
            TileLeftBottomOriginPoint, 
            triangleVertex1, 
            (triangleVertex2.x < point4.x) ? triangleVertex2 : point4, 
            (triangleVertex2.x < point4.x) ? point4 : triangleVertex2,
            DEFAULT_COVERAGEMASK_FLAT_FACE_X_OFFSET,
            DEFAULT_COVERAGEMASK_FLAT_FACE_Y_OFFSET
        );


        assert(triangleVertex3.y < triangleVertex1.y);
        assert(triangleVertex3.y < triangleVertex2.y);

        Result2 = FillTopFlatTriangle
    	(
            TileLeftBottomOriginPoint, 
            (triangleVertex2.x < point4.x) ? triangleVertex2 : point4,
            (triangleVertex2.x < point4.x) ? point4 : triangleVertex2,
            triangleVertex3,
            DEFAULT_COVERAGEMASK_FLAT_FACE_X_OFFSET,
            DEFAULT_COVERAGEMASK_FLAT_FACE_Y_OFFSET
        );
        
        result = _mm256_or_si256(Result1, Result2);
    }

    culling::M256I TEMP = tile.mHizDatas.l1CoverageMask;
    tile.mHizDatas.l1CoverageMask = _mm256_or_si256(TEMP, result);
}


/// <summary>
/// reference : http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
/// </summary>
/// <param name="coverageMask"></param>
/// <param name="LeftBottomPoint"></param>
/// <param name="triangle"></param>
void culling::CoverageRasterizer::FillTriangle(culling::Tile& tile, const Vec2& LeftBottomPoint, TwoDTriangle& triangle)
{
    SortTriangle(triangle);

    FillTriangle(tile, LeftBottomPoint, triangle.Points[0], triangle.Points[1], triangle.Points[2]);

}

void culling::CoverageRasterizer::FillTriangle(culling::Tile& tile, const Vec2& LeftBottomPoint, ThreeDTriangle& triangle)
{
    SortTriangle(triangle);

    FillTriangle(tile, LeftBottomPoint, *reinterpret_cast<culling::Vec2*>(triangle.Points + 0), *reinterpret_cast<culling::Vec2*>(triangle.Points + 1), *reinterpret_cast<culling::Vec2*>(triangle.Points + 2));

}


void culling::CoverageRasterizer::FillBottomFlatTriangleBatch
(
    culling::M256I* const outCoverageMask, // 8 coverage mask. array size should be 8
    const culling::Vec2& TileLeftBottomOriginPoint,
    const culling::M256F* const triangleVertex1, // [0] : First Vertex of Triangle, [1] : Second Vertex of Triangle, [2] : Third Vertex of Triangle
    const culling::M256F* const triangleVertex2,
    const culling::M256F* const triangleVertex3
)
{
    /*

     
    const float inverseSlope1 = (point1.x - point2.x) / (point1.y - point2.y);
    const float inverseSlope2 = (point1.x - point3.x) / (point1.y - point3.y);

    const float curx1 = point2.x + (TileLeftBottomOriginPoint.y - point2.y) * inverseSlope1 - TileLeftBottomOriginPoint.x;
    const float curx2 = point3.x + (TileLeftBottomOriginPoint.y - point3.y) * inverseSlope2 - TileLeftBottomOriginPoint.x;

    const culling::M256F leftFaceEventFloat = _mm256_set_ps(curx1 + inverseSlope1 * 7.0f, curx1 + inverseSlope1 * 6.0f, curx1 + inverseSlope1 * 5.0f, curx1 + inverseSlope1 * 4.0f, curx1 + inverseSlope1 * 3.0f, curx1 + inverseSlope1 * 2.0f, curx1 + inverseSlope1 * 1.0f, curx1);
    culling::M256I leftFaceEvent = _mm256_cvtps_epi32(_mm256_floor_ps(leftFaceEventFloat));
    leftFaceEvent = _mm256_max_epi32(leftFaceEvent, _mm256_set1_epi32(0));

    const culling::M256F rightFaceEventFloat = _mm256_set_ps(curx2 + inverseSlope2 * 7.0f, curx2 + inverseSlope2 * 6.0f, curx2 + inverseSlope2 * 5.0f, curx2 + inverseSlope2 * 4.0f, curx2 + inverseSlope2 * 3.0f, curx2 + inverseSlope2 * 2.0f, curx2 + inverseSlope2 * 1.0f, curx2);
    culling::M256I rightFaceEvent = _mm256_cvtps_epi32(_mm256_ceil_ps(rightFaceEventFloat));
    rightFaceEvent = _mm256_max_epi32(rightFaceEvent, _mm256_set1_epi32(0));

    culling::M256I Mask1 = _mm256_srlv_epi32(_mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF), leftFaceEvent);
    culling::M256I Mask2 = _mm256_srlv_epi32(_mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF), rightFaceEvent);

    culling::M256I Result = _mm256_and_si256(Mask1, _mm256_xor_si256(Mask2, _mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF)));

    */
}

void culling::CoverageRasterizer::FillTopFlatTriangleBatch
(
    culling::M256I* const outCoverageMask, // 8 coverage mask. array size should be 8
    const culling::Vec2& TileLeftBottomOriginPoint,
    const culling::M256F* const triangleVertex1,
    // [0] : First Vertex of Triangle, [1] : Second Vertex of Triangle, [2] : Third Vertex of Triangle
    const culling::M256F* const triangleVertex2,
    const culling::M256F* const triangleVertex3
)
{
    /*

     
    const float inverseSlope1 = (point3.x - point1.x) / (point3.y - point1.y);
    const float inverseSlope2 = (point3.x - point2.x) / (point3.y - point2.y);

    const float curx1 = point1.x + (TileLeftBottomOriginPoint.y - point1.y) * inverseSlope1 - TileLeftBottomOriginPoint.x;
    const float curx2 = point2.x + (TileLeftBottomOriginPoint.y - point2.y) * inverseSlope2 - TileLeftBottomOriginPoint.x;

    const culling::M256F leftFaceEventFloat = _mm256_set_ps(curx1 + inverseSlope1 * 7.0f, curx1 + inverseSlope1 * 6.0f, curx1 + inverseSlope1 * 5.0f, curx1 + inverseSlope1 * 4.0f, curx1 + inverseSlope1 * 3.0f, curx1 + inverseSlope1 * 2.0f, curx1 + inverseSlope1 * 1.0f, curx1);
    culling::M256I leftFaceEvent = _mm256_cvtps_epi32(_mm256_floor_ps(leftFaceEventFloat));
    leftFaceEvent = _mm256_max_epi32(leftFaceEvent, _mm256_set1_epi32(0));

    const culling::M256F rightFaceEventFloat = _mm256_set_ps(curx2 + inverseSlope2 * 7.0f, curx2 + inverseSlope2 * 6.0f, curx2 + inverseSlope2 * 5.0f, curx2 + inverseSlope2 * 4.0f, curx2 + inverseSlope2 * 3.0f, curx2 + inverseSlope2 * 2.0f, curx2 + inverseSlope2 * 1.0f, curx2);
    culling::M256I rightFaceEvent = _mm256_cvtps_epi32(_mm256_ceil_ps(rightFaceEventFloat));
    rightFaceEvent = _mm256_max_epi32(rightFaceEvent, _mm256_set1_epi32(0));

    culling::M256I Mask1 = _mm256_srlv_epi32(_mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF), leftFaceEvent);
    culling::M256I Mask2 = _mm256_srlv_epi32(_mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF), rightFaceEvent);

    culling::M256I Result = _mm256_and_si256(Mask1, _mm256_xor_si256(Mask2, _mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF)));

    */
}


void culling::CoverageRasterizer::FillTriangleBatch
(
    culling::M256I* const outCoverageMask, // 8 coverage mask. array size should be 8
    const culling::Vec2& TileLeftBottomOriginPoint,
    const culling::M256F* const triangleVertex1, // [0] : First Vertex of Triangle, [1] : Second Vertex of Triangle, [2] : Third Vertex of Triangle
    const culling::M256F* const triangleVertex2,
    const culling::M256F* const triangleVertex3
)
{
    /*
     *
     *
    // http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
       // general case - split the triangle in a topflat and bottom-flat one
    const Vec2 point4{ triangleVertex1.x + ((triangleVertex2.y - triangleVertex1.y) / (triangleVertex3.y - triangleVertex1.y)) * (triangleVertex3.x - triangleVertex1.x), triangleVertex2.y };

    culling::M256I Result1, Result2;
    
    Result1 = FillBottomFlatTriangleBatch
    (
        TileLeftBottomOriginPoint,
        triangleVertex1,
        (triangleVertex2.x < point4.x) ? triangleVertex2 : point4,
        (triangleVertex2.x < point4.x) ? point4 : triangleVertex2
    );

    Result2 = FillTopFlatTriangleBatch
    (
        TileLeftBottomOriginPoint,
        (triangleVertex2.x < point4.x) ? triangleVertex2 : point4,
        (triangleVertex2.x < point4.x) ? point4 : triangleVertex2,
        triangleVertex3
    );

    tile.mHizDatas.l1CoverageMask = _mm256_or_si256(tile.mHizDatas.l1CoverageMask, _mm256_or_si256(Result1, Result2));

    */
}
