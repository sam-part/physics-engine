#include "application.h"
#include "random.h"

namespace demo
{
	// Create each scene 
	void application::initialize_scenes()
	{
		scenes.push_back({"Playground", [&]() {
			return std::move(std::make_unique<demo::playground_scene>(world));
		}});

		scenes.push_back({ "Gravity", [&]() {
			return std::move(std::make_unique<demo::gravity_scene>(world));
		}});

		scenes.push_back({ "Kinematics Problem", [&] {
			return std::move(std::make_unique<demo::kinematics_problem_scene>(world));
		}});
		
		scenes.push_back({ "Dynamics Problem", [&] {
			return std::move(std::make_unique<demo::dynamics_problem_scene>(world));
		}});

		set_scene(0);
	}

	// Set the current scene
	void application::set_scene(size_t index)
	{
		world.clear();

		world.set_gravity(world_gravity);

		scene_index = index;
		scene = std::move(scenes[index].create_scene());
		scene->start();
		
		demo::scene_default_settings scene_defaults = scene->get_default_settings();
		pan = scene_defaults.default_pan;
		scale = scene_defaults.default_scale;

		update_object_scales();
	}

	// Update size of each object 
	void application::update_object_scales()
	{
		for (auto& object : scene->objects)
		{
			object->update_shape(scale);
		}
	}

	// Update ImGui menu
	void application::menu(physics::performance_report performance_report)
	{
		if (!menu_open)
			return;

		if (!ImGui::Begin("Physics Engine Demo", &menu_open))
		{
			ImGui::End();
			return;
		}

		if (ImGui::TreeNodeEx("General", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::Button(paused ? "Play" : "Pause"))
			{
				paused = !paused;
			}

			ImGui::SameLine();
			if (ImGui::Button("Reset"))
			{
				set_scene(scene_index);
			}

			ImGui::TreePop();
		}

		ImGui::NewLine();

		if (ImGui::BeginTabBar("Physics Engine Demo"))
		{
			if (ImGui::BeginTabItem("Scene"))
			{
				on_scene_tab = true;

				if (ImGui::TreeNodeEx("Scenes", ImGuiTreeNodeFlags_DefaultOpen))
				{
					for (size_t index = 0; index < scenes.size(); index++)
					{
						if (ImGui::Selectable(scenes[index].name.c_str(), index == scene_index))
						{
							set_scene(index);
						}
					}

					ImGui::TreePop();
				}

				ImGui::EndTabItem();
			}
			else
			{
				on_scene_tab = false;
			}

			if (ImGui::BeginTabItem("World"))
			{
				ImGui::PushItemWidth(160);

				ImGui::Text("Gravity");
				if (ImGui::InputDouble("##Gravity", &world_gravity.y, 0.1, 0, "%.3f m/s^2", ImGuiInputTextFlags_EnterReturnsTrue))
				{
					world.set_gravity(world_gravity);
				}
				ImGui::NewLine();

				int temp_iterations_per_frame = iterations_per_frame;
				ImGui::Text("Engine updates per frame");
				if (ImGui::InputInt("##Iterations", &temp_iterations_per_frame, 1, 1, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					iterations_per_frame = std::max(temp_iterations_per_frame, 1);
				}

				ImGui::PopItemWidth();

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		ImGui::End();
	}

	application::application()
	{
		sf::ContextSettings settings;
		settings.antialiasingLevel = 4;

		// Create window
		window.create(sf::VideoMode(default_width, default_height), window_name, sf::Style::Default, settings);
		window.setFramerateLimit(max_fps);
	}

	// Convert world space cooridnates to screen space coordinates
	sf::Vector2f application::world_to_screen(physics::vec_2d point)
	{
		return sf::Vector2f((point.x - pan.x) * scale + window_width / 2.0f, window_height - ((point.y + pan.y) * scale + window_height / 2.0f));
	}

	// Convert screen space coordinates to world space coordinates
	physics::vec_2d application::screen_to_world(sf::Vector2f point)
	{
		return physics::vec_2d((point.x - window_width / 2.0) / scale + pan.x, (-point.y + window_height / 2.0) / scale - pan.y);
	}

	void application::run()
	{
		// Initialize ImGui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::SFML::Init(window);

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

		// Initialize application scenes
		initialize_scenes();

		physics::timer timer;
		running = true;

		// Main application loop
		while (running && window.isOpen())
		{
			// Update window size
			sf::Vector2u window_size = window.getSize();
			window_width = window_size.x;
			window_height = window_size.y;

			// Update mouse position
			sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);

			bool mouse_available = !io.WantCaptureMouse;

			// Process events
			sf::Event event;
			std::vector<sf::Event> events;

			while (window.pollEvent(event))
			{
				ImGui::SFML::ProcessEvent(event);
				events.push_back(event);

				if (event.type == sf::Event::Closed)
				{
					// Close window
					running = false;
					window.close();
				}
				else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && mouse_available && scene->allows_pan())
				{
					// Pan screen with mouse
					pan.x -= (mouse_pos.x - prev_mouse_pos.x) / scale;
					pan.y -= (mouse_pos.y - prev_mouse_pos.y) / scale;
				}
				else if (event.type == sf::Event::MouseWheelScrolled && scene->allows_zoom() && mouse_available)
				{
					// Zoom to mouse position

					int scroll = event.mouseWheelScroll.delta;
					double zoom_amount = std::pow(zoom_per_scroll, std::abs(scroll));

					if (scroll > 0) // Mouse wheel up, zoom in
					{
						pan.x += (2.0 * mouse_pos.x - window_width) * (1.0 - zoom_amount) / (scale * 2.0);
						pan.y += (2.0 * mouse_pos.y - window_height) * (1.0 - zoom_amount) / (scale * 2.0);

						scale /= zoom_amount;
					}
					else if (scroll < 0) // Mouse wheel down, zoom out
					{

						scale *= zoom_amount;

						pan.x -= (2.0 * mouse_pos.x - window_width) * (1.0 - zoom_amount) / (scale * 2.0);
						pan.y -= (2.0 * mouse_pos.y - window_height) * (1.0 - zoom_amount) / (scale * 2.0);
					}
				}
				else if (event.type == sf::Event::Resized)
				{
					// Resize visible area when window is resized
					sf::FloatRect visible_area(0, 0, event.size.width, event.size.height);
					window.setView(sf::View{visible_area});
				}
				else if (event.type == sf::Event::KeyPressed)
				{
					if (event.key.code == sf::Keyboard::Escape)
					{
						// Toggle menu
						menu_open = !menu_open;
					}
					else if (event.key.code == sf::Keyboard::Space)
					{
						// Toggle pause
						paused = !paused;
					}
				}
			}

			prev_mouse_pos = mouse_pos;

			frame_counter++;
			if (timer.elapsed<std::chrono::seconds>() >= 1.0)
			{
				timer.reset();
				fps = frame_counter;
				frame_counter = 0;
				std::cout << fps << "\n";
			}

			// Update scene
			if (scene && (!paused || scene->updates_on_pause()))
				scene->update_scene(events, *this);

			// Update world
			if (!paused)
			{
				double dt = 1.0 / (max_fps * iterations_per_frame);

				for (size_t substep = 0; substep < iterations_per_frame; substep++)
				{
					world.step(dt, 1);

					// Update scene
					if (scene && (!paused || scene->updates_on_pause()))
						scene->update_world(dt, *this);
				}
			}

			// Update all visible object sizes
			update_object_scales();
			
			window.clear(sf::Color::Black);

			if (scene)
			{
				// Draw all objects in the scene
				for (auto& object : scene->objects)
				{
					sf::Shape& shape = object->get_shape();
					physics::body* body = object->get_body();

					shape.setPosition(world_to_screen(body->get_position()));
					shape.setRotation(-physics::rad_to_deg(body->get_rotation()));
					window.draw(shape);
				}

				// Allow the scene to draw
				scene->draw(*this, window);
			}

			// Draw collision points (if enabled)
			if (draw_collision_points)
			{
				for (const auto& contact : world.get_contacts())
				{
					for (const auto& collision_point : contact.contact_points)
					{
						sf::RectangleShape rect{ {5, 5} };
						rect.setFillColor(sf::Color::Green);
						rect.setOrigin(rect.getSize() / 2.0f);
						rect.setPosition(world_to_screen(collision_point));
						window.draw(rect);
					}
				}
			}

			// Draw origin
			sf::Vertex center_point(world_to_screen({ 0, 0 }), sf::Color::Yellow);
			window.draw(&center_point, 1, sf::Points);

			// Update menu
			ImGui::NewFrame();

			menu(world.get_step_performance());
			if(scene && menu_open && on_scene_tab)
				scene->update_menu();

			ImGui::Render();
			ImGui::SFML::Render(window);

			// Display window
			window.display();
		}

		ImGui::SFML::Shutdown();
	}
}