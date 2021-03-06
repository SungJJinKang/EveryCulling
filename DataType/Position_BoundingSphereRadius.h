#pragma once

#include "../EveryCullingCore.h"
#include "Math/Vector.h"
#include <cstring>

namespace culling
{
	struct alignas(16) Position_BoundingSphereRadius
	{

		culling::Vec4 Position;


		EVERYCULLING_FORCE_INLINE void SetPosition(const culling::Vec3& _position)
		{
			std::memcpy(Position.data(), _position.data(), sizeof(culling::Vec3));
		}

		EVERYCULLING_FORCE_INLINE const culling::Vec3& GetPosition() const
		{
			return *reinterpret_cast<const culling::Vec3*>(Position.data());
		}

		EVERYCULLING_FORCE_INLINE void SetPosition(const float* const vec3)
		{
			std::memcpy(Position.data(), vec3, sizeof(culling::Vec3));
		}

		EVERYCULLING_FORCE_INLINE void SetBoundingSphereRadius(const float boundingSphereRadius)
		{
			// why minus?
			// Calculated distance between frustum plane and object is positive when object is in frustum
			assert(boundingSphereRadius > 0.0f);
			Position.values[3] = boundingSphereRadius;
		}

		EVERYCULLING_FORCE_INLINE float GetBoundingSphereRadius() const
		{
			return Position.values[3];
		}
	};
}
