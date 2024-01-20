#pragma once

#include "../scene.h"
#include <deque>

namespace demo
{
	class gravity_scene : public scene
	{
	private:
		struct gravity_body
		{
			double mass { 1.0 };
			double radius { 1.0 };
			physics::vec_2d position {};
			physics::vec_2d velocity {};
		};

		std::vector<std::deque<physics::vec_2d>> trails {};

		const double G = 6.67e-11;
		const size_t trail_length = 1000;

	public:
		gravity_scene(physics::world& world);

		void start();
		void update_world(double dt, application& app) override;
		void update_scene(const std::vector<sf::Event>& events, application& app) override;
		void draw(application& app, sf::RenderWindow& window) override;
		//void update_menu() override;
	};
}