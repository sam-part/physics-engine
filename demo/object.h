#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <engine/body.h>
#include <iostream>

namespace demo
{
	const sf::Color default_fill_color { 64, 64, 64 };
	const sf::Color default_outline_color { 255, 255, 255 };

	// Represents a physics body graphically using SFML
	class object
	{

	protected:
		physics::body* body {nullptr};
		sf::Color fill_color { default_fill_color };
		sf::Color outline_color { default_outline_color };

	public:
		object(physics::body* body);
		physics::body* get_body();

		virtual sf::Shape& get_shape() = 0;
		virtual void update_shape(float scale) = 0;

		void set_color(sf::Color color);
		void set_outline_color(sf::Color color);

		sf::Color get_color() const;
		sf::Color get_outline_color() const;
	};
}