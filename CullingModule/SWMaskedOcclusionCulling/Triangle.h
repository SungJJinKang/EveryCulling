#pragma once

namespace culling
{
	struct Point
	{
		float x, y;
	};

	struct Triangle
	{
		/// <summary>
		/// x, y
		/// </summary>
		Point Point1;

		/// <summary>
		/// x, y
		/// </summary>
		Point Point2;

		/// <summary>
		/// x, y
		/// </summary>
		Point Point3;
	};
}