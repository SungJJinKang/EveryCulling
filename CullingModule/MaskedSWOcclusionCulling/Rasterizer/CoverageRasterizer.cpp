#include "CoverageRasterizer.h"

#include <algorithm>


M128I culling::CoverageRasterizer::FillBottomFlatTriangle(CoverageMask& coverageMask, const Vector2& LeftBottomPoint, const Vector2& point1, const Vector2& point2, const Vector2& point3)
{
    M128I Result;

    //if invslope > 0, left facing edge
    //else, right facing edge
    float invslope1 = (point2.x - point1.x) / (point2.y - point1.y);
    float invslope2 = (point3.x - point1.x) / (point3.y - point1.y);

    float curx = point1.x - LeftBottomPoint.x;

#if TILE_WIDTH == 16

    //I will use this variable like 2 byte integer, not 4 byte
    M128I rightEvent1 = _mm_set_epi16(curx, curx + invslope1, curx + invslope1 * 2, curx + invslope1 * 3, curx + invslope1 * 4, curx + invslope1 * 5, curx + invslope1 * 6, curx + invslope1 * 7); // I will use this variable like 2byte integer, not 4byte
    rightEvent1 = _mm_max_epi16(rightEvent1, *reinterpret_cast<M128I*>(&M128F_Zero));
    M128I rightEvent2 = _mm_set_epi16(curx, curx + invslope2, curx + invslope2 * 2, curx + invslope2 * 3, curx + invslope2 * 4, curx + invslope2 * 5, curx + invslope2 * 6, curx + invslope2 * 7); // I will use this variable like 2byte integer, not 4byte
    rightEvent2 = _mm_max_epi16(rightEvent2, *reinterpret_cast<M128I*>(&M128F_Zero));

    M128I Mask1 = _mm_srl_epi16(*reinterpret_cast<M128I*>(&M128F_EVERY_BITS_ONE), rightEvent1);
    M128I Mask2 = _mm_srl_epi16(*reinterpret_cast<M128I*>(&M128F_EVERY_BITS_ONE), rightEvent2);

    Mask1 = _mm_xor_si128(Mask1, *reinterpret_cast<M128I*>(&((invslope1 > 0.0f) ? M128F_Zero : M128F_EVERY_BITS_ONE)));
    Mask2 = _mm_xor_si128(Mask2, *reinterpret_cast<M128I*>(&((invslope2 > 0.0f) ? M128F_Zero : M128F_EVERY_BITS_ONE)));

    Result = _mm_and_si128(Mask1, Mask2);

    
   
#elif TILE_WIDTH == 32

#endif

    return Result;
}

M128I culling::CoverageRasterizer::FillTopFlatTriangle(CoverageMask& coverageMask, const Vector2& LeftBottomPoint, const Vector2& point1, const Vector2& point2, const Vector2& point3)
{
    M128I Result;

    //if invslope < 0, left facing edge
    //else, right facing edge
    float invslope1 = (point3.x - point1.x) / (point3.y - point1.y);
    float invslope2 = (point3.x - point2.x) / (point3.y - point2.y);

    float curx = point1.x - LeftBottomPoint.x;

#if TILE_WIDTH == 16

    //I will use this variable like 2 byte integer, not 4 byte
    M128I rightEvent1 = _mm_set_epi16(curx, curx - invslope1, curx - invslope1 * 2, curx - invslope1 * 3, curx - invslope1 * 4, curx - invslope1 * 5, curx - invslope1 * 6, curx - invslope1 * 7); // I will use this variable like 2byte integer, not 4byte
    rightEvent1 = _mm_max_epi16(rightEvent1, *reinterpret_cast<M128I*>(&M128F_Zero));
    M128I rightEvent2 = _mm_set_epi16(curx, curx - invslope2, curx - invslope2 * 2, curx - invslope2 * 3, curx - invslope2 * 4, curx - invslope2 * 5, curx - invslope2 * 6, curx - invslope2 * 7); // I will use this variable like 2byte integer, not 4byte
    rightEvent2 = _mm_max_epi16(rightEvent2, *reinterpret_cast<M128I*>(&M128F_Zero));

    M128I Mask1 = _mm_srl_epi16(*reinterpret_cast<M128I*>(&M128F_EVERY_BITS_ONE), rightEvent1);
    M128I Mask2 = _mm_srl_epi16(*reinterpret_cast<M128I*>(&M128F_EVERY_BITS_ONE), rightEvent2);

    Mask1 = _mm_xor_si128(Mask1, *reinterpret_cast<M128I*>(&((invslope1 < 0.0f) ? M128F_Zero : M128F_EVERY_BITS_ONE)));
    Mask2 = _mm_xor_si128(Mask2, *reinterpret_cast<M128I*>(&((invslope2 < 0.0f) ? M128F_Zero : M128F_EVERY_BITS_ONE)));

    Result = _mm_and_si128(Mask1, Mask2);

#elif TILE_WIDTH == 32

#endif

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

        M128I Result1 = FillBottomFlatTriangle(coverageMask, LeftBottomPoint, triangle.Point1, triangle.Point2, point4);
        M128I Result2 = FillTopFlatTriangle(coverageMask, LeftBottomPoint, triangle.Point2, point4, triangle.Point3);

        coverageMask.mBits = _mm_or_si128(Result1, Result2);
    }

}
