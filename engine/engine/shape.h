#pragma once

#include <vector>
#include <memory>
#include "math.h"

namespace physics
{
	enum class shape_type
	{
		none,
		polygon,
		circle
	};

	class shape 
	{
	protected:
		shape(physics::shape_type type);

		double area { 0.0 };
		double area_of_inertia { 0.0 };  
		physics::vec_2d centroid { 0, 0 };
		physics::shape_type type { physics::shape_type::none };

		// Virtual function to calculate properties of the shape (area, inertia, centroid)
		virtual void calculate_shape() = 0;

	public:
		~shape() = default;

		double get_area() const; 
		double get_area_of_inertia() const;
		physics::vec_2d get_centroid() const;
		physics::shape_type get_type() const;
	};

	using shape_ptr = std::unique_ptr<physics::shape>;

	// Arbitrary convex polygon class
	class polygon : public shape
	{
	private:
		std::vector<physics::vec_2d> vertices {};

		void calculate_shape();

	public:
		polygon();
		polygon(std::vector<physics::vec_2d> vertices);

		const std::vector<physics::vec_2d>& get_vertices() const;
	};

	// Circle class
	class circle : public shape
	{
	private:
		double radius { 0.0 };

		void calculate_shape();

	public:
		circle(double radius);

		double get_radius() const;
	};

	// Creates a rectangle of given dimensions centered locally around the point (0, 0) 
	shape_ptr make_rect(double width, double height);

	// Creates a polygon with given vertices (in counter-clockwise order) around the point (0, 0)
	shape_ptr make_polygon(std::vector<physics::vec_2d> vertices);

	// Create a circle of given radius centered locally around the point (0, )
	shape_ptr make_circle(double radius);

	// Translates a set of vertices 
	void translate_vertices(std::vector<physics::vec_2d>& vertices, physics::vec_2d origin, double rotation, physics::vec_2d centroid);
}