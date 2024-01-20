#pragma once

#include "object.h"

namespace demo
{
	object::object(physics::body* body)
		: body(body)
	{}

	physics::body* object::get_body()
	{
		return body;
	}

	void object::set_color(sf::Color color)
	{
		fill_color = color;
	}

	void object::set_outline_color(sf::Color color)
	{
		outline_color = color;
	}

	sf::Color object::get_color() const
	{
		return fill_color;
	}

	sf::Color object::get_outline_color() const
	{
		return outline_color;
	}
}