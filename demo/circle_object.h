#pragma once

#include "object.h"

namespace demo
{
	class circle_object : public object
	{
	private:
		sf::CircleShape circle;
		double radius { 0.0 };

		sf::Texture texture;

	public:
		circle_object(physics::body* body);

		void use_texture();

		sf::Shape& get_shape();
		void update_shape(float scale);
	};
}