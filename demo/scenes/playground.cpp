#include "playground.h"
#include "../application.h"

namespace demo
{
	bool playground_scene::selected_object_valid()
	{
		return is_object_selected;
	}

	playground_scene::playground_scene(physics::world& world)
		: scene(world)
	{
		scene_defaults.default_pan = { 0, 1.0 };
		scene_defaults.default_scale = 65.0;	

		update_on_pause = true;
	}

	void playground_scene::start()
	{
		physics::material material;
		material.density = 1.0;
		material.restitution = 0.8;

		physics::shape_ptr square_shape = physics::make_rect(1, 1);
		physics::body* square_body = world.create_body(std::move(square_shape), material, physics::dynamic_body);
		objects.push_back(std::move(std::make_unique<demo::polygon_object>(square_body)));

		physics::shape_ptr ground_shape = physics::make_rect(150, 1);
		physics::body* ground_body = world.create_body(std::move(ground_shape), material, physics::static_body, { 0, -5.0 });
		object_ptr ground = std::move(std::make_unique<demo::polygon_object>(ground_body));
		ground->set_color({ 54, 94, 60 });
		objects.push_back(std::move(ground));
	}

	void playground_scene::update_scene(const std::vector<sf::Event>& events, application& app)
	{
		ImGuiIO io = ImGui::GetIO();

		if (is_object_selected && selected_object_index != objects[selected_object_index]->get_body()->get_id())
		{
			is_object_selected = false;
		}

		if (selected_mode == mode::view)
		{
			allow_zoom = true;
			allow_pan = true;
		}
		else if (selected_mode == mode::object_create)
		{
			allow_pan = false;

			for (const auto& event : events)
			{
				if (event.type == sf::Event::MouseButtonPressed && !io.WantCaptureMouse)
				{
					if (event.mouseButton.button == sf::Mouse::Left)
					{
						sf::Vector2i screen_position = { event.mouseButton.x, event.mouseButton.y };
						physics::vec_2d position = app.screen_to_world(static_cast<sf::Vector2f>(screen_position));

						sf::Color sf_fill_color { static_cast<uint8_t>(fill_color[0] * 255), static_cast<uint8_t>(fill_color[1] * 255), static_cast<uint8_t>(fill_color[2] * 255) };
						sf::Color sf_outline_color{ static_cast<uint8_t>(outline_color[0] * 255), static_cast<uint8_t>(outline_color[1] * 255), static_cast<uint8_t>(outline_color[2] * 255) };

						if (random_color)
						{
							sf_fill_color.r = rng.get_number(0, 255);
							sf_fill_color.g = rng.get_number(0, 255);
							sf_fill_color.b = rng.get_number(0, 255);
						}

						if (selected_shape_type == create_shape_type::circle)
						{
							double radius = create_shape_radius;
							if (random_size)
								radius = rng.get_double(0.25, 0.75);

							physics::shape_ptr circle = physics::make_circle(radius);
							physics::body* circle_body = world.create_body(std::move(circle), create_shape_material, body_type, position);
							circle_body->set_rotation(-physics::deg_to_rad(create_shape_rotation));
							std::unique_ptr<demo::circle_object> circle_object = std::make_unique<demo::circle_object>(circle_body);
							circle_object->use_texture();
							object_ptr circle_obj = std::move(circle_object);
							circle_obj->set_color(sf_fill_color);
							circle_obj->set_outline_color(sf_outline_color);
							objects.push_back(std::move(circle_obj));
						}
						else if (selected_shape_type == create_shape_type::rectangle)
						{
							double width = create_shape_width;
							double height = create_shape_height;

							if (random_size)
							{
								width = rng.get_double(0.5, 1.5);
								height = rng.get_double(0.5, 1.5);
							}

							physics::shape_ptr rect = physics::make_rect(width, height);
							physics::body* rect_body = world.create_body(std::move(rect), create_shape_material, body_type, position);
							rect_body->set_rotation(-physics::deg_to_rad(create_shape_rotation));
							object_ptr rect_obj = std::make_unique<demo::polygon_object>(rect_body);
							rect_obj->set_color(sf_fill_color);
							rect_obj->set_outline_color(sf_outline_color);
							objects.push_back(std::move(rect_obj));
						}
						else if (selected_shape_type == create_shape_type::polygon)
						{
							if (creating_polygon && !create_shape_polygon_closed)
							{
								double tolerance = 5.0;

								if (create_shape_vertices.empty())
								{
									create_shape_vertices.push_back(position);
								}
								else
								{
									sf::Vector2f vertex_screen_pos = app.world_to_screen(create_shape_vertices[0]);
									double dx = vertex_screen_pos.x - screen_position.x;
									double dy = vertex_screen_pos.y - screen_position.y;

									// Check if polygon connected back to start
									if (dx * dx + dy * dy <= tolerance * tolerance)
									{
										create_shape_polygon_closed = true;
									}
									else
									{
										create_shape_vertices.push_back(position);
									}
								}
							}
							else if (!creating_polygon && create_shape_polygon_closed)
							{
								std::vector<physics::vec_2d> adjusted_vertices{};

								for (const auto& vertex : create_shape_vertices)
									adjusted_vertices.push_back(vec_sub(vertex, create_shape_vertices[0]));

								physics::shape_ptr polygon = physics::make_polygon(adjusted_vertices);
								physics::body* polygon_body = world.create_body(std::move(polygon), create_shape_material, body_type, position);
								polygon_body->set_rotation(-physics::deg_to_rad(create_shape_rotation));
								object_ptr polygon_obj = std::make_unique<demo::polygon_object>(polygon_body);
								polygon_obj->set_color(sf_fill_color);
								polygon_obj->set_outline_color(sf_outline_color);
								objects.push_back(std::move(polygon_obj));
							}
						}
					}
				}
			}
		}
		else if (selected_mode == mode::object_edit)
		{
			allow_pan = false;

			for (const auto& event : events)
			{
				if (event.type == sf::Event::MouseButtonPressed && !io.WantCaptureMouse)
				{
					if (event.mouseButton.button == sf::Mouse::Left)
					{
						int x = event.mouseButton.x;
						int y = event.mouseButton.y;

						physics::vec_2d mouse_pos = app.screen_to_world({ static_cast<float>(x), static_cast<float>(y) });
						bool object_found = false;

						// Inefficient test to find the object clicked
						size_t index = 0;
						for (const auto& obj : objects)
						{
							physics::vec_2d position = obj->get_body()->get_position();
							physics::shape_type type = obj->get_body()->get_shape()->get_type();

							if (type == physics::shape_type::polygon)
							{
								const std::vector<physics::vec_2d> vertices = obj->get_body()->get_translated_vertices();

								// Ray casting algorithm
								// https://en.wikipedia.org/wiki/Point_in_polygon

								size_t count = 0;
								for (size_t vertex_idx = 0; vertex_idx < vertices.size(); vertex_idx++)
								{
									physics::vec_2d vertex_a = vertices[vertex_idx];
									physics::vec_2d vertex_b = vertices[(vertex_idx + 1) % vertices.size()];

									if ((mouse_pos.y < vertex_a.y) != (mouse_pos.y < vertex_b.y) && mouse_pos.x < vertex_a.x + (mouse_pos.y - vertex_a.y) / (vertex_b.y - vertex_a.y) * (vertex_b.x - vertex_a.x))
										count++;
								}

								// If ray passes through an odd number of edges, point is inside polygon
								if (count % 2 == 1)
								{
									selected_object_index = index;
									object_found = true;
									break;
								}
							}
							else if (type == physics::shape_type::circle)
							{
								double radius = static_cast<const physics::circle*>(obj->get_body()->get_shape())->get_radius();
								double distance = physics::get_distance_sq(mouse_pos, position);

								if (distance <= radius * radius)
								{
									selected_object_index = index;
									object_found = true;
									break;
								}
							}

							index++;
						}

						if (object_found)
						{
							is_object_selected = true;
						}
					}
				}
			}
		}
	}

	void playground_scene::draw(application& app, sf::RenderWindow& window)
	{
		if (creating_polygon)
		{
			std::vector<sf::Vertex> vertices{};
			for (const physics::vec_2d& vertex : create_shape_vertices)
				vertices.push_back(sf::Vertex{ app.world_to_screen(vertex) });

			if (create_shape_polygon_closed && !vertices.empty())
			{
				vertices.push_back(vertices[0]);
			}
			else
			{
				vertices.push_back(sf::Vertex{ sf::Vector2f {sf::Mouse::getPosition(window) } });
			}

			window.draw(vertices.data(), vertices.size(), sf::LinesStrip);
		}
	}

	void playground_scene::update_menu()
	{
		if (!ImGui::Begin("Physics Engine Demo"))
		{
			ImGui::End();
			return;
		}

		ImGui::NewLine();

		if (ImGui::TreeNodeEx("Playground", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::TreeNodeEx("Select Mode", ImGuiTreeNodeFlags_DefaultOpen))
			{
				if (ImGui::Selectable("View / Control", selected_mode == mode::view))
				{
					selected_mode = mode::view;
				}

				if (ImGui::Selectable("Object Create", selected_mode == mode::object_create))
				{
					selected_mode = mode::object_create;
				}

				if (ImGui::Selectable("Object Edit", selected_mode == mode::object_edit))
				{
					selected_mode = mode::object_edit;
				}

				ImGui::TreePop();
			}

			ImGui::NewLine();

			if (selected_mode == mode::object_create)
			{
				if (ImGui::TreeNodeEx("Object Create", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::Text("Left click to place in world");
					ImGui::NewLine();

					if (ImGui::TreeNodeEx("Shape", ImGuiTreeNodeFlags_DefaultOpen))
					{
						if (ImGui::Selectable("Circle", selected_shape_type == create_shape_type::circle))
						{
							selected_shape_type = create_shape_type::circle;
						}

						if (ImGui::Selectable("Rectangle", selected_shape_type == create_shape_type::rectangle))
						{
							selected_shape_type = create_shape_type::rectangle;
						}

						if (ImGui::Selectable("Polygon", selected_shape_type == create_shape_type::polygon))
						{
							selected_shape_type = create_shape_type::polygon;
						}

						ImGui::NewLine();

						ImGui::PushItemWidth(120);

						if (selected_shape_type == create_shape_type::circle)
						{
							ImGui::Checkbox("Random Size", &random_size);
							ImGui::InputDouble("Radius", &create_shape_radius, 0.1, 0.5, "%.3f");
							create_shape_radius = std::max(create_shape_radius, 0.0);
						}

						if (selected_shape_type == create_shape_type::rectangle)
						{
							ImGui::Checkbox("Random Size", &random_size);
							ImGui::InputDouble("Width", &create_shape_width, 0.1, 0.5, "%.3f");
							ImGui::InputDouble("Height", &create_shape_height, 0.1, 0.5, "%.3f");
							create_shape_width = std::max(create_shape_width, 0.0);
							create_shape_height = std::max(create_shape_height, 0.0);
						}

						ImGui::InputDouble("Rotation", &create_shape_rotation, 1.0, 10.0, "%.f deg");

						if (selected_shape_type == create_shape_type::polygon)
						{
							if (!creating_polygon)
							{
								if (ImGui::Button("Set Vertices"))
								{
									creating_polygon = true;
									create_shape_polygon_closed = false;
									create_shape_vertices.clear();
								}
							}
							else
							{
								ImGui::Text("Click to create vertices");
								ImGui::Text("Connect to first vertex to finish");
								ImGui::NewLine();

								if (ImGui::Button("Cancel"))
								{
									create_shape_vertices.clear();
									creating_polygon = false;
								}

								if (ImGui::Button("Reset Vertices"))
								{
									create_shape_vertices.clear();
								}

								if (create_shape_polygon_closed)
								{
									if (ImGui::Button("Done"))
									{
										creating_polygon = false;
									}
								}
							}
						}
					
						ImGui::PopItemWidth();

						ImGui::TreePop();
					}

					ImGui::NewLine();

					if (ImGui::TreeNodeEx("Type", ImGuiTreeNodeFlags_DefaultOpen))
					{
						if (ImGui::Selectable("Dynamic Body", body_type == physics::dynamic_body))
						{
							body_type = physics::dynamic_body;
						}
						if (ImGui::Selectable("Static Body", body_type == physics::static_body))
						{
							body_type = physics::static_body;
						}

						ImGui::TreePop();
					}

					ImGui::NewLine();

					if (ImGui::TreeNodeEx("Material", ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::PushItemWidth(120);

						ImGui::InputDouble("Density", &create_shape_material.density, 0.1, 1.0, "%.3f");
						create_shape_material.density = std::max(create_shape_material.density, 0.0);

						double min = 0.0;
						double max = 1.0;
						ImGui::SliderScalar("Restitution", ImGuiDataType_Double, &create_shape_material.restitution, &min, &max, "%.3f");
						create_shape_material.restitution = std::clamp(create_shape_material.restitution, 0.0, 1.0);

						ImGui::PopItemWidth();
						ImGui::TreePop();
					}

					ImGui::NewLine();

					if (ImGui::TreeNodeEx("Color", ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::Checkbox("Random Color", &random_color);

						ImGui::PushItemWidth(160);

						ImGui::NewLine();
						ImGui::ColorEdit3("Fill Color", (float*)fill_color.data());
						ImGui::ColorEdit3("Outline Color", (float*)outline_color.data());

						ImGui::PopItemWidth();

						ImGui::TreePop();
					}

					ImGui::TreePop();
				}
			}
			else if (selected_mode == mode::object_edit && selected_object_valid())
			{
				if (ImGui::TreeNodeEx("Object Edit", ImGuiTreeNodeFlags_DefaultOpen))
				{
					physics::shape_type shape_type = objects[selected_object_index]->get_body()->get_shape()->get_type();

					if (shape_type == physics::shape_type::circle)
					{
						ImGui::Text("Circle");
					}
					else if (shape_type == physics::shape_type::polygon)
					{
						ImGui::Text("Polygon");
					}

					std::string mass_text = "Mass: " + std::to_string(objects[selected_object_index]->get_body()->get_mass()) + " kg";
					ImGui::Text(mass_text.c_str());

					ImGui::NewLine();

					/*
					if (ImGui::Button("Delete"))
					{
						objects.erase(objects.begin() + selected_object_index);
						world.remove_body(objects[selected_object_index]->get_body());

						selected_object_index = -1;
						is_object_selected = false;
					}
					*/
					//else
					//{
						ImGui::NewLine();

						if (ImGui::TreeNodeEx("Material##edit", ImGuiTreeNodeFlags_DefaultOpen))
						{
							physics::material material = objects[selected_object_index]->get_body()->get_material();
							bool changed = false;

							ImGui::PushItemWidth(120);

							if (ImGui::InputDouble("Density##edit", &material.density, 0.1, 1.0, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
							{
								material.density = std::max(material.density, 0.0);
								changed = true;
							}

							double min = 0.0;
							double max = 1.0;
							if (ImGui::SliderScalar("Restitution##edit", ImGuiDataType_Double, &create_shape_material.restitution, &min, &max, "%.3f"))
							{
								material.restitution = std::clamp(material.restitution, 0.0, 1.0);
								changed = true;
							}

							if (changed)
								objects[selected_object_index]->get_body()->set_material(material);

							ImGui::PopItemWidth();
							ImGui::TreePop();
						}

						ImGui::NewLine();

						if (ImGui::TreeNodeEx("Color##edit", ImGuiTreeNodeFlags_DefaultOpen))
						{
							ImGui::PushItemWidth(160);

							if (ImGui::ColorEdit3("Fill Color##edit", (float*)fill_color.data()))
							{
								sf::Color sf_fill_color{ static_cast<uint8_t>(fill_color[0] * 255), static_cast<uint8_t>(fill_color[1] * 255), static_cast<uint8_t>(fill_color[2] * 255) };
								objects[selected_object_index]->set_color(sf_fill_color);
							}

							if (ImGui::ColorEdit3("Outline Color##edit", (float*)outline_color.data()))
							{
								sf::Color sf_outline_color{ static_cast<uint8_t>(outline_color[0] * 255), static_cast<uint8_t>(outline_color[1] * 255), static_cast<uint8_t>(outline_color[2] * 255) };
								objects[selected_object_index]->set_color(sf_outline_color);
							}

							ImGui::PopItemWidth();

							ImGui::TreePop();
						}
					//}

					ImGui::TreePop();
				}
			}

			ImGui::TreePop();
		}

		ImGui::End();
	}
}