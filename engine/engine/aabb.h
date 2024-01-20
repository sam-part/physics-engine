#pragma once

#include "math.h"

namespace physics
{
	// Axis-Aligned Bounding Box
	struct aabb
	{
		physics::vec_2d min {};
		physics::vec_2d max {};

		aabb() = default;
		aabb(physics::vec_2d min, physics::vec_2d max);
	};

	// Returns true if two axis-aligned bounding boxes intersect
	bool aabb_intersection(physics::aabb a, physics::aabb b);
}