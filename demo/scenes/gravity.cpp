#include "gravity.h"
#include "../random.h"
#include <iostream>
#include "../application.h"

namespace demo
{
	gravity_scene::gravity_scene(physics::world& world)
		: scene(world)
	{
		scene_defaults.default_pan = { 0, 0 };
		scene_defaults.default_scale = 5.75e-2;
	}

	void gravity_scene::start()
	{
		world.set_gravity(physics::vec_zero);

		gravity_body sun;
		sun.mass = 2.5e19;
		sun.position = { 0, 0 };
		sun.radius = 500;

		gravity_body planet_1;
		planet_1.mass = 1e2;
		planet_1.position = { 1e3, 0 };
		planet_1.radius = 100;
		planet_1.velocity = { 0, 1300 };

		gravity_body planet_2;
		planet_2.mass = 2e5;
		planet_2.position = { 2e3, 0 };
		planet_2.radius = 150;
		planet_2.velocity = { 0, 825 };

		gravity_body planet_3;
		planet_3.mass = 1e6;
		planet_3.position = { 6e3, 0 };
		planet_3.radius = 250;
		planet_3.velocity = { 0, 440 };

		gravity_body planet_4;
		planet_4.mass = 5e5;
		planet_4.position = { 4e3, 0 };
		planet_4.radius = 80;
		planet_4.velocity = { 0, 565 };

		gravity_body planet_5;
		planet_5.mass = 1e8;
		planet_5.position = { 1e4, 0 };
		planet_5.radius = 300;
		planet_5.velocity = { -25, 345 };

		gravity_body planet_6;
		planet_6.mass = 1e5;
		planet_6.position = { 7e3, 0 };
		planet_6.radius = 75;
		planet_6.velocity = { -50, 425 };

		random::fast_random rng{ std::random_device{}() };

		std::vector<gravity_body> gravity_bodies = { sun, planet_1, planet_2, planet_3, planet_4, planet_5, planet_6 };
		for (size_t i = 0; i < gravity_bodies.size(); i++)
		{
			gravity_body& body = gravity_bodies[i];

			physics::material material;
			material.density = body.mass / (physics::pi * body.radius * body.radius);

			physics::shape_ptr circle_shape = physics::make_circle(body.radius);
			physics::body* circle_body = world.create_body(std::move(circle_shape), material, physics::dynamic_body, body.position);
			circle_body->set_velocity(body.velocity);

			object_ptr circle_obj = std::make_unique<demo::circle_object>(circle_body);

			sf::Color color{ (uint8_t)rng.get_number(0, 255), (uint8_t)rng.get_number(0, 255) , (uint8_t)rng.get_number(0, 255) };
			
			// Determine color brightness (from ITU-R BT.709)
			double brightness = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b; 

			// If too dark, ensure baseline level of brightness
			if (brightness < 64)
			{
				color = { (uint8_t)rng.get_number(128, 255), (uint8_t)rng.get_number(128, 255) , (uint8_t)rng.get_number(128, 255) };
			}

			circle_obj->set_color(color);

			objects.push_back(std::move(circle_obj));
		}

		// Set sun color
		if (!objects.empty())
			objects[0]->set_color({ 253, 184, 19 });

		trails.resize(gravity_bodies.size());
	}

	void gravity_scene::update_world(double dt, application& app)
	{
		for (size_t i = 0; i < objects.size(); i++)
		{
			double mass_a = objects[i]->get_body()->get_mass();
			physics::vec_2d pos_a = objects[i]->get_body()->get_position();

			for (size_t j = 0; j < objects.size(); j++)
			{
				if (i == j)
					continue;

				double mass_b = objects[j]->get_body()->get_mass();
				physics::vec_2d pos_b = objects[j]->get_body()->get_position();

				physics::vec_2d normal = vec_normalize(physics::vec_sub(pos_b, pos_a));
				double gravity_force = (G * mass_a * mass_b) / physics::get_distance_sq(pos_a, pos_b);

				physics::vec_2d force_vector = physics::vec_mul(normal, gravity_force);

				objects[i]->get_body()->apply_force(force_vector);
			}
		}
	}

	void gravity_scene::update_scene(const std::vector<sf::Event>& events, application& app)
	{
		for (size_t i = 0; i < objects.size(); i++)
		{
			trails[i].push_back(objects[i]->get_body()->get_position());
		}
	}

	void gravity_scene::draw(application& app, sf::RenderWindow& window)
	{
		for (size_t trail_index = 0; trail_index < trails.size(); trail_index++)
		{
			auto& trail = trails[trail_index];
			sf::VertexArray points(sf::Points, std::min(trail.size(), trail_length));

			sf::Color color = objects[trail_index]->get_color();

			if (trail.size() >= trail_length)
				trail.erase(trail.begin());

			for (size_t index = 0; index < trail.size(); index++)
			{
				float opacity = 255.0f / trail.size() * index;
				points[index].position = app.world_to_screen(trail[index]);
				points[index].color = sf::Color(color.r, color.g, color.b, opacity);
			}

			window.draw(points);
		}
	}
}