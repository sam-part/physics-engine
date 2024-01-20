#include "body.h"
#include <cassert>

void physics::body::calculate_mass()
{
	if (type == body_type::static_body)
	{
		mass = 0.0;
		moment_of_inertia = 0.0;
		inv_mass = 0.0;
		inv_moment_of_inertia = 0.0;
	}
	else
	{
		mass = shape->get_area() * material.density;
		moment_of_inertia = shape->get_area_of_inertia() * material.density;
		inv_mass = 1.0 / mass;
		inv_moment_of_inertia = 1.0 / moment_of_inertia;
	}
}

physics::body::body(shape_ptr shape, physics::material material, physics::body_type type, physics::vec_2d position, double rotation)
	: shape(std::move(shape)), material(material), type(type), position(position), rotation(rotation)
{
	calculate_mass();
	update_shape();
}

physics::vec_2d physics::body::get_position() const
{
	return position;
}

double physics::body::get_rotation() const
{
	return rotation;
}

void physics::body::move(double dx, double dy)
{
	position.x += dx;
	position.y += dy;
}

void physics::body::move(physics::vec_2d displacement)
{
	position = vec_add(position, displacement);
}

void physics::body::rotate(double theta)
{
	rotation += theta;
}

physics::vec_2d physics::body::get_velocity() const
{
	return velocity;
}

void physics::body::set_transforn(physics::vec_2d position, double rotation)
{
	this->position = position;
	this->rotation = rotation;
}

const size_t physics::body::get_id() const
{
	return id;
}

void physics::body::set_position(physics::vec_2d position)
{
	this->position = position;
}

void physics::body::set_rotation(double rotation)
{
	this->rotation = rotation;
}

void physics::body::apply_impulse(physics::vec_2d impulse)
{
	velocity = vec_add(velocity, vec_mul(impulse, inv_mass));
}

void physics::body::apply_force(physics::vec_2d applied_force)
{
	force = vec_add(force, applied_force);
}

void physics::body::set_velocity(physics::vec_2d new_velocity)
{
	velocity = new_velocity;
}

double physics::body::get_mass() const
{
	return mass;
}

double physics::body::get_inverse_mass() const
{
	return inv_mass;
}

double physics::body::get_moment_of_inertia() const
{
	return moment_of_inertia;
}

double physics::body::get_inv_moment_of_inertia() const
{
	return inv_moment_of_inertia;
}

const physics::shape* physics::body::get_shape()
{
	return shape.get();
}

physics::material physics::body::get_material()
{
	return material;
}

void physics::body::set_material(physics::material material)
{
	this->material = material;
	calculate_mass();
}

physics::body_type physics::body::get_type()
{
	return type;
}

void physics::body::set_type(body_type type)
{
	this->type = type;
	calculate_mass();
}

const std::vector<physics::vec_2d>& physics::body::get_translated_vertices() const
{
	return translated_vertices;
}

void physics::body::update_shape()
{
	physics::shape_type shape_type = shape->get_type();

	if (shape_type == physics::shape_type::polygon)
	{
		aabb.min = { DBL_MAX, DBL_MAX };
		aabb.max = { -DBL_MAX, -DBL_MAX };

		translated_vertices = static_cast<const physics::polygon*>(shape.get())->get_vertices();
		translate_vertices(translated_vertices, position, rotation, shape.get()->get_centroid());

		for (const physics::vec_2d& vertex : translated_vertices)
		{
			aabb.min.x = std::min(aabb.min.x, vertex.x);
			aabb.min.y = std::min(aabb.min.y, vertex.y);

			aabb.max.x = std::max(aabb.max.x, vertex.x);
			aabb.max.y = std::max(aabb.max.y, vertex.y);
		}
	}
	else if (shape_type == physics::shape_type::circle)
	{
		double radius = static_cast<const physics::circle*>(shape.get())->get_radius();

		aabb.min.x = position.x - radius;
		aabb.min.y = position.y - radius;

		aabb.max.x = position.x + radius;
		aabb.max.y = position.y + radius;
	}
}

physics::aabb physics::body::get_aabb()
{
	return aabb;
}

bool physics::body::operator==(const body& other)
{
	return id == other.id;
}
