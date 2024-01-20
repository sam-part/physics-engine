#pragma once

#include "transform.h"
#include "body.h"
#include "math.h"
	
namespace physics
{
	class world;

	class object
	{
		friend class world;

	private:
		physics::transform transform {};
		physics::vec_2d velocity {};
		double angular_velocity { 0.0 };

		physics::vec_2d force {};
		double torque { 0.0 };
		// moment of inertia

		physics::body body;

	public:
		object() = default;
		object(physics::vec_2d position);
		object(physics::transform transform);

		physics::transform get_transform() const;
		void move(double dx, double dy);
		void move(physics::vec_2d displacement);
		void rotate(double theta);
		void set_transform(physics::transform new_transform);

		// Applies an impulse to the object in units of N·s
		void apply_impulse(physics::vec_2d impulse);

		// Applies an impulse to the object at a point in units of N·s
		void apply_impulse(physics::vec_2d point, physics::vec_2d impulse);

		// Applies a force to the object in units of N
		// This force will only be applied over the next timestep
		void apply_force(physics::vec_2d applied_force);

		void set_velocity(physics::vec_2d new_velocity);

		physics::body* get_body();
		void set_body(physics::body body);

		double get_mass() const;

		// Converts local-space coordinates relative to the object into world-space coordinates
		physics::vec_2d to_world_space(physics::vec_2d local);
	};
}