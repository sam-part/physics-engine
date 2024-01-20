 #pragma once

#include "../scene.h"

namespace demo
{
	class kinematics_problem_scene : public scene
	{
	private:
		double launch_angle = 60.0;
		double launch_speed = 24.0;
		double max_height = 0.0;
		bool landed = false;

		double range{};
		double height{};

		double calculated_value_range {};
		double calculated_value_height {};

		physics::vec_2d start_point{};
		physics::body* body { nullptr };
	public:

		kinematics_problem_scene(physics::world& world);

		void start();
		void update_world(double dt, application& app) override;
		void update_menu() override;
	};
}