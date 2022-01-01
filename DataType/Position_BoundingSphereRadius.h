#pragma once

#include "../EveryCullingCore.h"
#include "Math/Vector.h"
#include <cstring>

namespace culling
{
	struct alignas(16) Position_BoundingSphereRadius
	{

		culling::Vec4 Position;


		FORCE_INLINE void SetPosition(const culling::Vec3& _position)
		{
			std::memcpy(Position.data(), _position.data(), sizeof(culling::Vec3));
		}

		FORCE_INLINE const culling::Vec3& GetPosition() const
		{
			return *reinterpret_cast<const culling::Vec3*>(Position.data());
		}

		FORCE_INLINE void SetPosition(const float* const vec3)
		{
			std::memcpy(&Position, vec3, sizeof(culling::Vec3));
		}

		FORCE_INLINE void SetBoundingSphereRadius(const float _boundingSphereRadius)
		{
			// why minus?
			// Calculated distance between frustum plane and object is positive when object is in frustum

			Position.values[3] = _boundingSphereRadius;
		}

		FORCE_INLINE float GetBoundingSphereRadius() const
		{
			return Position.values[3];
		}
	};
}
