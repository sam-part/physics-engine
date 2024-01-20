#include "object.h"

namespace physics
{
	object::object(physics::vec_2d position)
		: transform({position.x, position.y, 0})
	{}

	object::object(physics::transform transform)
		: transform(transform)
	{}

	physics::transform object::get_transform() const
	{
		return transform;
	}

	void object::move(double dx, double dy)
	{
		transform.x += dx;
		transform.y += dy;

	}

	void object::move(physics::vec_2d displacement)
	{
		transform.x += displacement.x;
		transform.y += displacement.y;
	}

	void object::rotate(double theta)
	{
		transform.rotation += theta;
	}

	void object::set_transform(physics::transform new_transform)
	{
		transform = new_transform;
	}

	void object::apply_impulse(physics::vec_2d impulse)
	{
		// J = Δp
		// J = mv2 - mv1
		// J + mv1 = mv2
		// v2 = (J + mv1) / m
		// v2 = J/m + mv1/m
		// v2 = J/m + v1

		double mass = body.get_mass();
		velocity = { velocity.x + impulse.x / mass, velocity.y + impulse.y / mass };

		// Test accuracy of:
		// double inv_mass = body.get_inverse_mass();
		// velocity = { velocity.x + impulse.x * inv_mass, velocity.y + impulse.y * inv_mass };
	}

	void object::apply_impulse(physics::vec_2d point, physics::vec_2d impulse)
	{
		physics::vec_2d center_distance = vec_sub(point, { transform.x, transform.y });
	}

	void object::apply_force(physics::vec_2d applied_force)
	{
		force.x += applied_force.x;
		force.y += applied_force.y;
	}

	void object::set_velocity(physics::vec_2d new_velocity)
	{
		velocity = new_velocity;
	}

	physics::body* object::get_body()
	{
		return &body;
	}

	void object::set_body(physics::body body)
	{
		this->body = body;
	}

	double object::get_mass() const
	{
		return body.get_mass();
	}

	physics::vec_2d object::to_world_space(physics::vec_2d local)
	{
		return physics::vec_2d {local.x + transform.x, local.y + transform.y};
	}
}