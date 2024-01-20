#include "circle_object.h"

demo::circle_object::circle_object(physics::body* body)
	: object(body)
{
	if (body == nullptr || body->get_shape()->get_type() != physics::shape_type::circle)
		return;

	radius = static_cast<const physics::circle*>(body->get_shape())->get_radius();
	circle.setRadius(radius);
	circle.setOrigin({ static_cast<float>(radius), static_cast<float>(radius) });

	circle.setFillColor(fill_color);
	circle.setOutlineColor(outline_color);
	circle.setOutlineThickness(1);
	circle.setPointCount(128);
}

void demo::circle_object::use_texture()
{
	texture.loadFromFile("sprites/circle.png");
	circle.setTexture(&texture);
}

sf::Shape& demo::circle_object::get_shape()
{
	return circle;
}

void demo::circle_object::update_shape(float scale)
{
	circle.setRadius(radius * scale);
	circle.setOrigin({ static_cast<float>(radius * scale), static_cast<float>(radius * scale) });

	circle.setFillColor(fill_color);
	circle.setOutlineColor(outline_color);
}
