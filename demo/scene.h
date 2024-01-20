#pragma once

#include <engine/engine.h>
#include <vector>
#include <memory>
#include "imgui/imgui.h"
#include "polygon_object.h"
#include "circle_object.h"

namespace demo
{
	struct scene_default_settings
	{
		sf::Vector2f default_pan {0, 0};
		double default_scale { 100.0 };
	};

	using object_ptr = std::unique_ptr<demo::object>;

	class application;

	// Base scene class 
	class scene
	{
		friend class application;

	protected:
		physics::world& world;
		std::vector<object_ptr> objects {};

		demo::scene_default_settings scene_defaults {};

		bool allow_zoom = true;
		bool allow_pan = true;
		bool update_on_pause = false;

		scene(physics::world& world);

	public:
		// Start the scene
		virtual void start() = 0;

		// Update the scene
		virtual void update_scene(const std::vector<sf::Event>& events, application& app) {};

		// Update the scene (tied to world substeps)
		virtual void update_world(double dt, application& app) {};

		// Allow the scene to draw to the window
		virtual void draw(application& app, sf::RenderWindow& window) {};

		// Allow the scene to update the menu
		virtual void update_menu() {}; 

		bool allows_zoom() const;
		bool allows_pan() const;
		bool updates_on_pause() const;

		demo::scene_default_settings get_default_settings() const;
	};
}