#include "polygon_object.h"
#include <iostream>

namespace demo
{
	void polygon_object::build_polygon(float scale)
	{
		if (body == nullptr || body->get_shape() == nullptr || body->get_shape()->get_type() != physics::shape_type::polygon)
			return;

		const std::vector<physics::vec_2d>& vertices = static_cast<const physics::polygon*>(body->get_shape())->get_vertices();

		polygon.setPointCount(vertices.size());

		for (int point_index = 0; point_index < vertices.size(); point_index++)
		{
			physics::vec_2d vertex = vertices[point_index];
			polygon.setPoint(point_index, sf::Vector2f{ static_cast<float>(vertex.x * scale), -static_cast<float>(vertex.y * scale) });
		}

		physics::vec_2d centroid = body->get_shape()->get_centroid();
		polygon.setOrigin({ static_cast<float>(centroid.x * scale), static_cast<float>(-centroid.y * scale) });

		polygon.setFillColor(fill_color);
		polygon.setOutlineColor(outline_color);
		polygon.setOutlineThickness(1);
	}

	polygon_object::polygon_object(physics::body* body)
		: object(body)
	{
		build_polygon(1.0f);
	}

	sf::Shape& polygon_object::get_shape()
	{
		return polygon;
	}

	void polygon_object::update_shape(float scale)
	{
		build_polygon(scale);
	}
}