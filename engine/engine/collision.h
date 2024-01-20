#pragma once

#include "body.h"

namespace physics
{
	struct collision_manifold
	{
		// Bodies involved in the collision
		physics::body* body_a { nullptr };
		physics::body* body_b { nullptr };

		// Collision normal and depth
		physics::vec_2d normal {};
		double depth { 0.0 };

		// List of contact points
		std::vector<physics::vec_2d> contact_points {};
	};

	// Checks for a collision between two bodies
	// Returns true if a collision was detected
	bool get_collision(physics::body* body_a, physics::body* body_b, physics::collision_manifold& collision);
}