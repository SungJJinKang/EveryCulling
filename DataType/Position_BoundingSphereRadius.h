#pragma once

#include "../EveryCullingCore.h"
#include "Math/Vector.h"
#include <cstring>

namespace culling
{
	struct alignas(16) Position_BoundingSphereRadius
	{

		culling::Vec3 Position;
		float BoundingSphereRadius;


		FORCE_INLINE void SetPosition(const culling::Vec3& _position)
		{
			Position = _position;
		}

		FORCE_INLINE void SetPosition(const void* vec4)
		{
			std::memcpy(&Position, vec4, 16);
		}

		FORCE_INLINE void SetBoundingSphereRadius(const float _boundingSphereRadius)
		{
			// why minus?
			// Calculated distance between frustum plane and object is positive when object is in frustum

			BoundingSphereRadius = -(_boundingSphereRadius + BOUNDING_SPHRE_RADIUS_MARGIN);
		}
	};
}
