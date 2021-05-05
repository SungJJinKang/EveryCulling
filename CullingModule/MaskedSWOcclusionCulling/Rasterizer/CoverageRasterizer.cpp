#include "CoverageRasterizer.h"

#include <algorithm>

M256I culling::CoverageRasterizer::FillBottomFlatTriangle(CoverageMask& coverageMask, const Vector2& LeftBottomPoint, const Vector2& point1, const Vector2& point2, const Vector2& point3)
{
    //Assume Triangle is sorted

    //if invslope > 0, left facing edge
    //else, right facing edge
    const float invslope1 = (point2.x - point1.x) / (point2.y - point1.y);
    const float invslope2 = (point3.x - point1.x) / (point3.y - point1.y);

    const float curx = point1.x - LeftBottomPoint.x;

    //TODO : Ceil event values
    M256I rightEvent1 = _mm256_set_epi32(curx, curx + invslope1, curx + invslope1 * 2, curx + invslope1 * 3, curx + invslope1 * 4, curx + invslope1 * 5, curx + invslope1 * 6, curx + invslope1 * 7); // I will use this variable like 2byte integer, not 4byte
    rightEvent1 = _mm256_max_epi32(rightEvent1, *reinterpret_cast<M256I*>(&M128F_Zero));
    M256I rightEvent2 = _mm256_set_epi32(curx, curx + invslope2, curx + invslope2 * 2, curx + invslope2 * 3, curx + invslope2 * 4, curx + invslope2 * 5, curx + invslope2 * 6, curx + invslope2 * 7); // I will use this variable like 2byte integer, not 4byte
    rightEvent2 = _mm256_max_epi32(rightEvent2, *reinterpret_cast<M256I*>(&M128F_Zero));

    M256I Mask1 = _mm256_srlv_epi32(*reinterpret_cast<M256I*>(&M128F_EVERY_BITS_ONE), rightEvent1);
    M256I Mask2 = _mm256_srlv_epi32(*reinterpret_cast<M256I*>(&M128F_EVERY_BITS_ONE), rightEvent2);

    Mask1 = _mm256_xor_si256(Mask1, *reinterpret_cast<M256I*>(&((invslope1 > 0.0f) ? M128F_Zero : M128F_EVERY_BITS_ONE)));
    Mask2 = _mm256_xor_si256(Mask2, *reinterpret_cast<M256I*>(&((invslope2 > 0.0f) ? M128F_Zero : M128F_EVERY_BITS_ONE)));

    M256I Result = _mm256_and_si256(Mask1, Mask2);

    return Result;
}

M256I culling::CoverageRasterizer::FillTopFlatTriangle(CoverageMask& coverageMask, const Vector2& LeftBottomPoint, const Vector2& point1, const Vector2& point2, const Vector2& point3)
{
    //Assume Triangle is sorted
    
    //if invslope < 0, left facing edge
    //else, right facing edge
    const float invslope1 = (point3.x - point1.x) / (point3.y - point1.y);
    const float invslope2 = (point3.x - point2.x) / (point3.y - point2.y);

    const float curx = point1.x - LeftBottomPoint.x;

    //I will use this variable like 2 byte integer, not 4 byte
    // TODO : Ceil event values
    M256I rightEvent1 = _mm256_set_epi32(curx, curx - invslope1, curx - invslope1 * 2, curx - invslope1 * 3, curx - invslope1 * 4, curx - invslope1 * 5, curx - invslope1 * 6, curx - invslope1 * 7); // I will use this variable like 2byte integer, not 4byte
    rightEvent1 = _mm256_max_epi32(rightEvent1, *reinterpret_cast<M256I*>(&M128F_Zero));
    M256I rightEvent2 = _mm256_set_epi32(curx, curx - invslope2, curx - invslope2 * 2, curx - invslope2 * 3, curx - invslope2 * 4, curx - invslope2 * 5, curx - invslope2 * 6, curx - invslope2 * 7); // I will use this variable like 2byte integer, not 4byte
    rightEvent2 = _mm256_max_epi32(rightEvent2, *reinterpret_cast<M256I*>(&M128F_Zero));

    M256I Mask1 = _mm256_srlv_epi32(*reinterpret_cast<M256I*>(&M128F_EVERY_BITS_ONE), rightEvent1);
    M256I Mask2 = _mm256_srlv_epi32(*reinterpret_cast<M256I*>(&M128F_EVERY_BITS_ONE), rightEvent2);

    Mask1 = _mm256_xor_si256(Mask1, *reinterpret_cast<M256I*>(&((invslope1 < 0.0f) ? M128F_Zero : M128F_EVERY_BITS_ONE)));
    Mask2 = _mm256_xor_si256(Mask2, *reinterpret_cast<M256I*>(&((invslope2 < 0.0f) ? M128F_Zero : M128F_EVERY_BITS_ONE)));

    M256I Result = _mm256_and_si256(Mask1, Mask2);

    return Result;
}


/// <summary>
/// reference : http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
/// </summary>
/// <param name="coverageMask"></param>
/// <param name="LeftBottomPoint"></param>
/// <param name="triangle"></param>
void culling::CoverageRasterizer::FillTriangle(CoverageMask& coverageMask, Vector2 LeftBottomPoint, TwoDTriangle& triangle)
{
    SortTriangle(triangle);

    if (triangle.Point2.y == triangle.Point3.y)
    {
        coverageMask.mBits = FillBottomFlatTriangle(coverageMask, LeftBottomPoint, triangle.Point1, triangle.Point2, triangle.Point3);
    }
    /* check for trivial case of top-flat triangle */
    else if (triangle.Point1.y == triangle.Point2.y)
    {
        coverageMask.mBits = FillTopFlatTriangle(coverageMask, LeftBottomPoint, triangle.Point1, triangle.Point2, triangle.Point3);
    }
    else
    {
        /* general case - split the triangle in a topflat and bottom-flat one */
        Vector2 point4{ triangle.Point1.x + ((float)(triangle.Point2.y - triangle.Point1.y) / (float)(triangle.Point3.y - triangle.Point1.y)) * (triangle.Point3.x - triangle.Point1.x), triangle.Point2.y };

#if TILE_WIDTH == 16
        M128I Result1, Result2;
#elif TILE_WIDTH == 32
        M256I Result1, Result2;
#endif	

        Result1 = FillBottomFlatTriangle(coverageMask, LeftBottomPoint, triangle.Point1, triangle.Point2, point4);
        Result2 = FillTopFlatTriangle(coverageMask, LeftBottomPoint, triangle.Point2, point4, triangle.Point3);

#if TILE_WIDTH == 16
        coverageMask.mBits = _mm_or_si128(Result1, Result2);
#elif TILE_WIDTH == 32
        coverageMask.mBits = _mm256_or_si256(Result1, Result2);
#endif	

        
    }

}
