#include "dynamics_problem.h"

namespace demo
{
	dynamics_problem_scene::dynamics_problem_scene(physics::world& world)
		: scene(world)
	{
		scene_defaults.default_pan = { 10, -10 };
		scene_defaults.default_scale = 30.0;
	}

	void dynamics_problem_scene::start()
	{
		double angle_rad = physics::deg_to_rad(ramp_angle);

		physics::material material;
		material.kinetic_friction = std::sqrt(kinetic_friction);
		material.static_friction = 0;
		
		std::vector<physics::vec_2d> ramp_vertices(3);

		double width = ramp_length * std::cos(angle_rad);
		double height = ramp_length * std::sin(angle_rad);
		ramp_vertices[0] = { 0, 0 };
		ramp_vertices[1] = { width, 0 };
		ramp_vertices[2] = { 0, height };

		physics::shape_ptr ramp = physics::make_polygon(ramp_vertices);
		physics::vec_2d centroid = ramp->get_centroid();
		physics::body* ramp_body = world.create_body(std::move(ramp), material, physics::static_body, centroid);
		object_ptr ramp_obj = std::make_unique<demo::polygon_object>(ramp_body);
		objects.push_back(std::move(ramp_obj));

		std::vector<physics::vec_2d> box_vertices{ {-0.5, -0.5}, {0.5, -0.5}, {0.5, 0.5}, {-0.5, 0.5} };
		physics::shape_ptr box = physics::make_polygon(box_vertices);
		physics::vec_2d box_centroid = box->get_centroid();
		box_body = world.create_body(std::move(box), material, physics::dynamic_body, {0, 0});
		box_body->set_rotation(physics::deg_to_rad(90.0 - ramp_angle));

		// Position is based on center of mass so need to calculate the right placement position for the box
		double epsilon = 0.0001;
		physics::vec_2d rotated_point = physics::rotate_point(box_vertices[1], physics::deg_to_rad(90.0 - ramp_angle));
		start_point = { rotated_point.x + epsilon, height + rotated_point.y - box_centroid.y + epsilon };

		box_body->set_position(start_point);

		object_ptr box_obj = std::make_unique<demo::polygon_object>(box_body);
		objects.push_back(std::move(box_obj));

		// Calculate the estimated final speed
		double gravity = std::abs(world.get_gravity().y);
		calculated_value = std::sqrt(2.0 * gravity * travel_length * (std::sin(angle_rad) - kinetic_friction * std::cos(angle_rad)));
	}

	void dynamics_problem_scene::update_world(double dt, application& app)
	{
		if (!reached_end)
		{
			double distance = physics::get_distance(box_body->get_position(), start_point);

			if (distance >= travel_length)
			{
				reached_end = true;
				final_speed = physics::vec_magnitude(box_body->get_velocity());

				box_body->set_type(physics::static_body);
			}
		}
	}

	void dynamics_problem_scene::update_menu()
	{
		if (!ImGui::Begin("Physics Engine Demo"))
		{
			ImGui::End();
			return;
		}

		ImGui::NewLine();

		if (ImGui::TreeNodeEx("Scenario", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("A physics scenario to demonstrate the accuracy of the physics engine in a simple dynamics problem");

			ImGui::NewLine();
			ImGui::Text("A box starts from rest and slides %.4fm down a ramp inclined at angle of %.4f degrees. ", travel_length, ramp_angle);
			ImGui::Text("The friction coefficient between the box and the ramp is %.4f.", kinetic_friction);

			ImGui::NewLine();
			ImGui::Text("Estimated final speed: %.4fm/s", calculated_value);

			if (reached_end)
			{
				ImGui::Text("Measured final speed: %.4fm/s", final_speed);
			}

			ImGui::TreePop();
		}

		ImGui::End();
	}
}