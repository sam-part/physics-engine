#include "aabb.h"

physics::aabb::aabb(physics::vec_2d min, physics::vec_2d max)
	: min(min), max(max)
{}

bool physics::aabb_intersection(physics::aabb a, physics::aabb b)
{
	return a.min.x <= b.max.x && a.max.x >= b.min.x && a.min.y <= b.max.y && a.max.y >= b.min.y;
}