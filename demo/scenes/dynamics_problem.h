#pragma once

#include "../scene.h"

namespace demo
{
	class dynamics_problem_scene : public scene
	{
	private:
		double kinetic_friction = 0.2;
		double ramp_angle = 40.0;
		double ramp_length = 30.0;
		double travel_length = 27.0;
		double final_speed {};
		bool reached_end = false;

		double calculated_value {};

		physics::vec_2d start_point{};
		physics::body* box_body{ nullptr };

	public:
		dynamics_problem_scene(physics::world& world);

		void start();
		void update_world(double dt, application& app) override;
		void update_menu() override;
	};
}