#include "CoverageRasterizer.h"

#include <cmath>
#include <limits>

FORCE_INLINE culling::M256I culling::CoverageRasterizer::FillBottomFlatTriangle
(
    const Vec2& LeftBottomPoint, 
    const Vec2& point1, 
    const Vec2& point2, 
    const Vec2& point3
)
{
    //Assume Triangle is sorted

    //if invslope > 0, left facing edge
    //else, right facing edge
    const float invslope1 = (point2.x - point1.x) / (point2.y - point1.y);
    const float invslope2 = (point3.x - point1.x) / (point3.y - point1.y);

    const float curx = point1.x - LeftBottomPoint.x; 

    //Ceil event values and Cast to integer
    const culling::M256F rightEventFloat = _mm256_set_ps(curx, curx + invslope1, curx + invslope1 * 2, curx + invslope1 * 3, curx + invslope1 * 4, curx + invslope1 * 5, curx + invslope1 * 6, curx + invslope1 * 7);
    culling::M256I rightEvent = _mm256_cvtps_epi32(_mm256_ceil_ps(rightEventFloat));
    rightEvent = _mm256_max_epi32(rightEvent, _mm256_set1_epi32(0));
    
    //Ceil event values and Cast to integer
    const culling::M256F leftEventFloat = _mm256_set_ps(curx, curx + invslope2, curx + invslope2 * 2, curx + invslope2 * 3, curx + invslope2 * 4, curx + invslope2 * 5, curx + invslope2 * 6, curx + invslope2 * 7);
    culling::M256I leftEvent = _mm256_cvtps_epi32(_mm256_ceil_ps(leftEventFloat));
    leftEvent = _mm256_max_epi32(leftEvent, _mm256_set1_epi32(0));

    culling::M256I Mask1 = _mm256_srlv_epi32(_mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF), rightEvent);
    culling::M256I Mask2 = _mm256_srlv_epi32(_mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF), leftEvent);

    //Mask1 = _mm256_xor_si256(Mask1, (invslope1 < 0.0f) ? _mm256_set1_epi64x(0x0000000000000000) : _mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF));
    //Mask2 = _mm256_xor_si256(Mask2, (invslope2 < 0.0f) ? _mm256_set1_epi64x(0x0000000000000000) : _mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF));

    culling::M256I Result = _mm256_and_si256(Mask1, Mask2);

    return Result;
}

FORCE_INLINE culling::M256I culling::CoverageRasterizer::FillTopFlatTriangle
(
    const Vec2& LeftBottomPoint, 
    const Vec2& point1, 
    const Vec2& point2, 
    const Vec2& point3
)
{
    //Assume Triangle is sorted
    
    //if invslope < 0, left facing edge
    //else, right facing edge
    const float invslope1 = (point3.x - point1.x) / (point3.y - point1.y);
    const float invslope2 = (point3.x - point2.x) / (point3.y - point2.y);

    const float curx = point3.x - LeftBottomPoint.x;

    const culling::M256F rightEventFloat = _mm256_set_ps(curx, curx - invslope1, curx - invslope1 * 2, curx - invslope1 * 3, curx - invslope1 * 4, curx - invslope1 * 5, curx - invslope1 * 6, curx - invslope1 * 7);
    culling::M256I rightEvent = _mm256_cvtps_epi32(_mm256_ceil_ps(rightEventFloat)); 
    rightEvent = _mm256_max_epi32(rightEvent, _mm256_set1_epi32(0));

    const culling::M256F leftEventFloat = _mm256_set_ps(curx, curx - invslope2, curx - invslope2 * 2, curx - invslope2 * 3, curx - invslope2 * 4, curx - invslope2 * 5, curx - invslope2 * 6, curx - invslope2 * 7);
    culling::M256I leftEvent = _mm256_cvtps_epi32(_mm256_ceil_ps(leftEventFloat)); // I will use this variable like 2byte integer, not 4byte
    leftEvent = _mm256_max_epi32(leftEvent, _mm256_set1_epi32(0));

    culling::M256I Mask1 = _mm256_srlv_epi32(_mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF), rightEvent);
    culling::M256I Mask2 = _mm256_srlv_epi32(_mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF), leftEvent);

    
    //Mask1 = _mm256_xor_si256(Mask1, (invslope1 < 0.0f) ? _mm256_set1_epi64x(0x0000000000000000) : _mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF));
    //Mask2 = _mm256_xor_si256(Mask2, (invslope2 < 0.0f) ? _mm256_set1_epi64x(0x0000000000000000) : _mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF));

    culling::M256I Result = _mm256_and_si256(Mask1, Mask2);

    return Result;
}

void culling::CoverageRasterizer::FillTriangle
(
    culling::Tile& tile, 
    const Vec2& LeftBottomPoint,
	const culling::Vec2& triangleVertex1, 
    const culling::Vec2& triangleVertex2, 
    const culling::Vec2& triangleVertex3
)
{
    if (std::abs(triangleVertex2.y - triangleVertex3.y) < std::numeric_limits<float>::epsilon())
    {
        tile.mHizDatas.l1CoverageMask = _mm256_or_si256(tile.mHizDatas.l1CoverageMask, FillBottomFlatTriangle(LeftBottomPoint, triangleVertex1, triangleVertex2, triangleVertex3));
    }
    // check for trivial case of top-flat triangle
    else if (std::abs(triangleVertex1.y - triangleVertex2.y) < std::numeric_limits<float>::epsilon())
    {
        tile.mHizDatas.l1CoverageMask = _mm256_or_si256(tile.mHizDatas.l1CoverageMask, FillTopFlatTriangle(LeftBottomPoint, triangleVertex1, triangleVertex2, triangleVertex3));
    }
    else
    {
        // general case - split the triangle in a topflat and bottom-flat one
        const Vec2 point4{ triangleVertex1.x + ((triangleVertex2.y - triangleVertex1.y) / (triangleVertex3.y - triangleVertex1.y)) * (triangleVertex3.x - triangleVertex1.x), triangleVertex2.y };

        culling::M256I Result1, Result2;

        Result1 = FillBottomFlatTriangle
    	(
            LeftBottomPoint, 
            triangleVertex1, 
            (triangleVertex2.x < point4.x) ? triangleVertex2 : point4, 
            (triangleVertex2.x < point4.x) ? point4 : triangleVertex2
        );
        Result2 = FillTopFlatTriangle
    	(
            LeftBottomPoint, 
            (triangleVertex2.x < point4.x) ? triangleVertex2 : point4,
            (triangleVertex2.x < point4.x) ? point4 : triangleVertex2, 
            triangleVertex3);

        tile.mHizDatas.l1CoverageMask = _mm256_or_si256(tile.mHizDatas.l1CoverageMask, _mm256_or_si256(Result1, Result2));


    }
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

