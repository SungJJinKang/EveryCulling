#pragma once

#include "../EveryCullingCore.h"
#include "Math/Vector.h"

namespace culling
{
	class Position_BoundingSphereRadius
	{

	private:

		culling::Vector3 Position;
		float BoundingSphereRadius;

	public:

		FORCE_INLINE void SetPosition(const culling::Vector3& _position)
		{
			Position = _position;
		}

		FORCE_INLINE void SetBoundingSphereRadius(const float _boundingSphereRadius)
		{
			BoundingSphereRadius = -(_boundingSphereRadius + BOUNDING_SPHRE_RADIUS_MARGIN);
		}
	};
}
