#pragma once

#include "../scene.h"
#include "../random.h"
#include <vector>

namespace demo
{
	class playground_scene : public scene
	{
	private:
		enum class mode
		{
			view,
			object_create,
			object_edit
		};

		enum class create_shape_type
		{
			circle,
			rectangle,
			polygon
		};

		mode selected_mode { mode::view };
		create_shape_type selected_shape_type { create_shape_type::circle };

		// Inforation about shapes being currently created
		double create_shape_radius{ 0.5 };
		double create_shape_width{ 1.0 };
		double create_shape_height{ 1.0 };
		double create_shape_rotation { 0.0 };
		physics::material create_shape_material {};
		physics::body_type body_type = physics::dynamic_body;
		std::vector<physics::vec_2d> create_shape_vertices{};
		bool create_shape_polygon_closed = false;
		bool creating_polygon = false;
		std::vector<float> fill_color { default_fill_color.r / 255.0f, default_fill_color.g / 255.0f, default_fill_color.b / 255.0f };
		std::vector<float> outline_color { default_outline_color.r / 255.0f, default_outline_color.g / 255.0f, default_outline_color.b / 255.0f };
		bool random_size = false;
		bool random_color = false;

		bool is_object_selected = false;
		size_t selected_object_index { 0 };

		bool selected_object_valid();

		random::fast_random rng { std::random_device{}() };

	public:
		playground_scene(physics::world& world);

		void start();
		void update_scene(const std::vector<sf::Event>& events, application& app) override;
		void draw(application& app, sf::RenderWindow& window) override;
		void update_menu() override;
	};
}