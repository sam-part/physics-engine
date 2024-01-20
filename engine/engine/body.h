#pragma once

#include "shape.h"
#include "material.h"
#include "aabb.h"
#include <memory>

namespace physics
{
	enum class body_type
	{
		static_body,
		dynamic_body
	};

	const body_type static_body = body_type::static_body;
	const body_type dynamic_body = body_type::dynamic_body;

	class world;

	class body
	{
		friend class world;

	private:
		// Private constructor (bodies are created by the world class)
		body(shape_ptr shape, physics::material material, physics::body_type type, physics::vec_2d position, double rotation);

		size_t id { 0 };

		// Body position in world space
		physics::vec_2d position {};

		// Body velocity
		physics::vec_2d velocity {};

		// Body rotation
		double rotation { 0.0 };

		// Angular velocity in radians
		double angular_velocity { 0.0 };

		// Force acting on the body over the current timestep 
		physics::vec_2d force {};

		shape_ptr shape;
		physics::material material {};
		double mass { 0.0 };
		double inv_mass { 0.0 };
		double moment_of_inertia { 0.0 };
		double inv_moment_of_inertia { 0.0 };

		body_type type { physics::static_body };

		// Translated world-space vertices for polygon shapes
		std::vector<physics::vec_2d> translated_vertices {};

		// Axis-Aligned Bounding Box to improve collision detection performance
		physics::aabb aabb {};

		void calculate_mass();

	public:
		physics::vec_2d get_position() const;
		double get_rotation() const;

		physics::vec_2d get_velocity() const;

		const size_t get_id() const;
		
		void move(double dx, double dy);
		void move(physics::vec_2d displacement);
		void rotate(double theta);

		void set_transforn(physics::vec_2d position, double rotation);
		void set_position(physics::vec_2d position);
		void set_rotation(double rotation);

		// Applies an impulse to the object in units of N·s
		void apply_impulse(physics::vec_2d impulse);

		// Applies an impulse to the object at a point in units of N·s
		// void apply_impulse(physics::vec_2d point, physics::vec_2d impulse);

		// Applies a force to the object in units of N
		// This force will only be applied over the next timestep
		void apply_force(physics::vec_2d applied_force);

		// Set the object's velocity
		void set_velocity(physics::vec_2d new_velocity);

		double get_mass() const;
		double get_inverse_mass() const;
		double get_moment_of_inertia() const;
		double get_inv_moment_of_inertia() const;
		
		const physics::shape* get_shape();
		//void set_shape(shape_ptr shape);

		physics::material get_material();
		void set_material(physics::material material);

		physics::body_type get_type();
		void set_type(body_type type);

		// Get world-space translated vertices for polygons
		const std::vector<physics::vec_2d>& get_translated_vertices() const;

		// Update the internal shape of the body
		void update_shape();
		physics::aabb get_aabb();

		bool operator == (const body& other);
	};
}