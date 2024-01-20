#include "kinematics_problem.h"

namespace demo
{
	kinematics_problem_scene::kinematics_problem_scene(physics::world& world)
		: scene(world)
	{
		scene_defaults.default_pan = { 25, -10 };
		scene_defaults.default_scale = 20.0;
	}

	void kinematics_problem_scene::start()
	{
		physics::material material {};

		physics::shape_ptr circle_shape = physics::make_circle(1.0);
		body = world.create_body(std::move(circle_shape), material, physics::dynamic_body);
		start_point = body->get_position();
		object_ptr circle = std::make_unique<circle_object>(body);
		objects.push_back(std::move(circle));
		
		physics::shape_ptr ground_shape = physics::make_rect(150, 2);
		physics::body* ground_body = world.create_body(std::move(ground_shape), material, physics::static_body, { 0, -2.001 });
		object_ptr ground = std::move(std::make_unique<demo::polygon_object>(ground_body));
		ground->set_color({ 54, 94, 60 });
		objects.push_back(std::move(ground));

		double angle = physics::deg_to_rad(launch_angle);
		body->set_velocity({ launch_speed * std::cos(angle), launch_speed * std::sin(angle) });

		// Calculate the estimated range and height
		double gravity = std::abs(world.get_gravity().y);
		calculated_value_range = ((launch_speed * launch_speed) * std::sin(2.0 * angle)) / gravity;
		calculated_value_height = ((launch_speed * launch_speed) * physics::square(std::sin(angle))) / (2.0 * gravity);
	}

	void kinematics_problem_scene::update_world(double dt, application& app)
	{
		if (!landed)
		{
			double height = body->get_position().y;
			max_height = std::max(max_height, height);

			// Reached ground
			if (height <= start_point.y)
			{
				landed = true;

				range = body->get_position().x;
			}
		}
	}
	void kinematics_problem_scene::update_menu()
	{
		if (!ImGui::Begin("Physics Engine Demo"))
		{
			ImGui::End();
			return;
		}

		ImGui::NewLine();

		if (ImGui::TreeNodeEx("Scenario", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("A physics scenario to demonstrate the accuracy of the physics engine in a simple kinematics	 problem");

			ImGui::NewLine();
			ImGui::Text("A ball is launched at %.4fm/s at an angle %.4f degrees from the horizontal. ", launch_speed, launch_angle);

			ImGui::NewLine();
			ImGui::Text("Estimated range: %.4fm", calculated_value_range);
			ImGui::Text("Estimated max height: %.4fm", calculated_value_height);

			ImGui::NewLine();

			if (landed)
			{
				ImGui::Text("Measured range: %.4fm", range);
				ImGui::Text("Measured max height: %.4fm", max_height);
			}

			ImGui::TreePop();
		}

		ImGui::End();
	}
}