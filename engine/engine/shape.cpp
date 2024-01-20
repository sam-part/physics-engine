#include "shape.h"

#include <iostream>

namespace physics
{
	shape::shape(physics::shape_type type)
		: type(type)
	{}

	double shape::get_area() const
	{
		return area;
	}

	double shape::get_area_of_inertia() const
	{
		return area_of_inertia;
	}

	physics::vec_2d shape::get_centroid() const
	{
		return centroid;
	}

	shape_type shape::get_type() const
	{
		return type;
	}

	void polygon::calculate_shape()
	{
		// Calculates the area and centroid of a simple polygon 
		// https://en.wikipedia.org/wiki/Polygon#Area_and_centroid
		size_t n_vertices = vertices.size();
		area = 0.0;
		centroid = { 0.0, 0.0 };
		area_of_inertia = 0.0;

		for (size_t i = 0; i < n_vertices - 1; i++)
		{
			area += vertices[i].x * vertices[i + 1].y - vertices[i + 1].x * vertices[i].y;
			centroid.x += (vertices[i].x + vertices[i + 1].x) * (vertices[i].x * vertices[i + 1].y - vertices[i + 1].x * vertices[i].y);
			centroid.y += (vertices[i].y + vertices[i + 1].y) * (vertices[i].x * vertices[i + 1].y - vertices[i + 1].x * vertices[i].y);
		}

		area += vertices[n_vertices - 1].x * vertices[0].y - vertices[0].x * vertices[n_vertices - 1].y;
		area *= 0.5;

		centroid.x += (vertices[n_vertices - 1].x + vertices[0].x) * (vertices[n_vertices - 1].x * vertices[0].y - vertices[0].x * vertices[n_vertices - 1].y);
		centroid.y += (vertices[n_vertices - 1].y + vertices[0].y) * (vertices[n_vertices - 1].x * vertices[0].y - vertices[0].x * vertices[n_vertices - 1].y);

		centroid.x /= (6.0 * area);
		centroid.y /= (6.0 * area);

		area = std::abs(area);

		// Calculates the "area of inertia" using green's theorem
		// "Area of inertia" is just the mass moment of inertia divided by mass
		// https://physics.stackexchange.com/questions/493736/moment-of-inertia-for-an-arbitrary-polygon
		// https://en.wikipedia.org/wiki/Parallel_axis_theorem
		for (size_t i = 0; i < n_vertices; i++)
		{
			physics::vec_2d vertex_a = vertices[i];
			physics::vec_2d vertex_b = vertices[i + 1 == n_vertices ? 0 : i + 1];

			// (A * A + A * B + B * B) * cross(A, B)
			area_of_inertia += std::abs(vec_cross(vertex_a, vertex_b) * (vec_dot(vertex_a, vertex_a) + vec_dot(vertex_b, vertex_b) + vec_dot(vertex_a, vertex_b)));
		}

		area_of_inertia /= 12.0;
		area_of_inertia -= area * vec_dot(centroid, centroid); 
	}

	polygon::polygon()
		: shape(physics::shape_type::polygon)
	{}

	polygon::polygon(std::vector<physics::vec_2d> vertices)
		: vertices(vertices), shape(physics::shape_type::polygon)
	{
		calculate_shape();
	}

	const std::vector<physics::vec_2d>& polygon::get_vertices() const
	{
		return vertices;
	}

	void circle::calculate_shape()
	{
		area = physics::pi * radius * radius;
		centroid = { 0, 0 };
		area_of_inertia = 0.5 * radius * radius;
	}

	circle::circle(double radius)
		: radius(radius), shape(physics::shape_type::circle)
	{
		radius = std::max(radius, 0.0);
		calculate_shape();
	}

	double circle::get_radius() const
	{
		return radius;
	}

	shape_ptr make_rect(double width, double height)
	{
		return std::move(std::make_unique<polygon>(polygon{ {{-width / 2.0, height / 2.0}, {-width / 2.0, -height / 2.0}, {width / 2.0, -height / 2.0}, {width / 2.0, height / 2.0}} }));
	}

	shape_ptr make_polygon(std::vector<physics::vec_2d> vertices)
	{
		return std::move(std::make_unique<polygon>(polygon{vertices}));
	}

	shape_ptr make_circle(double radius)
	{
		return std::move(std::make_unique<circle>(circle{ radius }));
	}

	void translate_vertices(std::vector<physics::vec_2d>& vertices, physics::vec_2d origin, double rotation, physics::vec_2d centroid)
	{
		for (physics::vec_2d& vertex : vertices)
		{
			vertex = physics::rotate_point({ vertex.x - centroid.x, vertex.y - centroid.y }, rotation);
			vertex = { vertex.x + origin.x, vertex.y + origin.y };
		}
	}
}