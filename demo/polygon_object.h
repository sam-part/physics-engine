#pragma once

#include "object.h"

namespace demo
{
	class polygon_object : public object
	{
	private:
		sf::ConvexShape polygon {};

		void build_polygon(float scale);

	public:
		polygon_object(physics::body* body);

		sf::Shape& get_shape();
		void update_shape(float scale);
	};
}