#pragma once

#include <deque>
#include "body.h"
#include "collision.h"
#include "timer.h"

namespace physics
{
	struct performance_report
	{
		double collision_detection_time { 0.0 };
		double solve_constraints_time { 0.0 };
		double integrate_motion_time { 0.0 };
	};

	const physics::vec_2d default_gravity { 0, -9.8 };

	class world
	{
	private:
		physics::vec_2d gravity { default_gravity };
		std::deque<physics::body> bodies {};
		std::vector<physics::collision_manifold> contacts {};

		physics::timer timer;
		physics::performance_report performance_report;

		size_t body_id { 0 };

		void resolve_collision(physics::collision_manifold& collision, double dt);

	public:
		world() = default;
		world(physics::vec_2d gravity);
		world(world const&) = delete;
		world& operator=(world const&) = delete;

		// Create a body in the physics world
		// Only valid way to create a body
		physics::body* create_body(shape_ptr shape, physics::material material, physics::body_type type, physics::vec_2d position = {}, double rotation = 0.0);

		// Removes a body from the physics world
		void remove_body(physics::body* obj);

		// Removes all bodies from the world
		// Invaldiates all pointers to bodies in the world
		void clear();

		// Set the physics world's gravity
		void set_gravity(physics::vec_2d gravity);

		physics::vec_2d get_gravity() const;

		// Update the physics world over a discrete timestep
		void step(double time, int substeps);

		// Returns a performance report from the previous world step 
		physics::performance_report get_step_performance() const;

		std::vector<physics::collision_manifold> get_contacts() const;

		// Retrieves all bodies in the physics world
		// Used for debug and demo purposes
		std::vector<physics::body*> get_body_ptrs();

		// Get the number of bodies in the physics world
		size_t get_body_count() const;
	};
}