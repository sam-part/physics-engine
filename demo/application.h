#pragma once

#include <SFML/Graphics.hpp>
#include <engine/engine.h>
#include <string>
#include <iostream>
#include <functional>

#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"

#include "object.h"
#include "polygon_object.h"
#include "circle_object.h"

#include "scenes/scenes.h"
#include "scene.h"

namespace demo
{
	using scene_ptr = std::unique_ptr<demo::scene>;

	struct scene_info
	{
		std::string name {};
		std::function<scene_ptr()> create_scene {};
	};

	class application
	{
	private:
		sf::RenderWindow window;

		const int default_width = 1200;
		const int default_height = 800;
		const std::string window_name = "Physics Simulation";

		int window_width = default_width;
		int window_height = default_height;

		bool running = false;

		const int max_fps = 60;
		const int iterations_per_frame = 10;
		const double dt = 1.0 / (max_fps * iterations_per_frame);

		sf::Vector2f pan {0, 0};
		float scale = 50.0;
		float zoom_per_scroll = 0.90;

		sf::Vector2i prev_mouse_pos {0, 0};
		
		physics::world world;
		
		std::vector<demo::scene_info> scenes {};
		scene_ptr scene { nullptr };
		size_t scene_index { 0 };
		bool menu_open = true;
		bool paused = false;
		bool draw_collision_points = false;

		void initialize_scenes();
		void set_scene(size_t index);
		void update_object_scales();
		void menu(physics::performance_report performance_report);

	public:
		application();

		sf::Vector2f world_to_screen(physics::vec_2d point);
		physics::vec_2d screen_to_world(sf::Vector2f point);

		void run();
	};
}