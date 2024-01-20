#include "world.h"

namespace physics
{
	world::world(physics::vec_2d gravity)
		: gravity(gravity)
	{}

	std::vector<physics::collision_manifold> world::get_contacts() const
	{
		return contacts;
	}

	size_t world::get_body_count() const
	{
		return bodies.size();
	}

	std::vector<physics::body*> world::get_body_ptrs()
	{
		std::vector<physics::body*> body_ptrs;

		for (physics::body& body : bodies)
			body_ptrs.push_back(&body);

		return body_ptrs;
	}

	physics::body* world::create_body(shape_ptr shape, physics::material material, physics::body_type type, physics::vec_2d position, double rotation)
	{
		physics::body body(std::move(shape), material, type, position, rotation);
		body.id = body_id++;

		bodies.push_back(std::move(body));

		if (!bodies.empty())
			return &bodies.back();
			
		return nullptr;
	}

	void world::remove_body(physics::body* body)
	{
		auto it = std::find(bodies.begin(), bodies.end(), *body);
		if (it != bodies.end())
			bodies.erase(it);
	}

	void world::clear()
	{
		bodies.clear();
	}

	void world::set_gravity(physics::vec_2d gravity)
	{
		this->gravity = gravity;
	}

	physics::vec_2d world::get_gravity() const
	{
		return gravity;
	}

	void world::resolve_collision(physics::collision_manifold& collision, double dt)
	{
		physics::body* body_a = collision.body_a;
		physics::body* body_b = collision.body_b;

		bool a_static = body_a->type == physics::static_body;
		bool b_static = body_b->type == physics::static_body;

		physics::vec_2d origin_a = body_a->position;
		physics::vec_2d origin_b = body_b->position;

		physics::vec_2d direction = vec_sub(origin_a, origin_b);

		// If both objects are static, ignore
		if (a_static && b_static)
		{
			return;
		}

		// If normal in the wrong directiob, negate
		if (vec_dot(direction, collision.normal) > 0)
			collision.normal = vec_mul(collision.normal, -1.0);

		if (a_static && !b_static)
		{
			body_b->move(vec_mul(collision.normal, collision.depth));
		}
		else if (!a_static && b_static)
		{
			body_a->move(vec_mul(collision.normal, -collision.depth));
		}
		else
		{
			body_a->move(vec_mul(collision.normal, -collision.depth * 0.5));
			body_b->move(vec_mul(collision.normal, collision.depth * 0.5));	
		}

		double restitution = body_a->material.restitution * body_b->material.restitution;

		double inv_mass_a = body_a->get_inverse_mass();
		double inv_mass_b = body_b->get_inverse_mass();

		double inv_inertia_a = body_a->get_inv_moment_of_inertia();
		double inv_inertia_b = body_b->get_inv_moment_of_inertia();

		double static_friction = body_a->material.static_friction * body_b->material.static_friction;
		double kinetic_friction = body_a->material.kinetic_friction * body_b->material.kinetic_friction;

		// Average contact points
		size_t num_contacts = collision.contact_points.size();
		physics::vec_2d contact_point{};
		for (size_t i = 0; i < num_contacts; i++)
		{
			contact_point = vec_add(contact_point, collision.contact_points[i]);
		}

		contact_point = vec_div(contact_point, num_contacts);

		// Find relative contact points to body origin
		vec_2d ra = vec_sub(contact_point, origin_a);
		vec_2d rb = vec_sub(contact_point, origin_b);

		vec_2d ra_perp = { -ra.y, ra.x };
		vec_2d rb_perp = { -rb.y, rb.x };

		// Calculate angular velocity vector
		vec_2d angular_va = vec_mul(ra_perp, body_a->angular_velocity);
		vec_2d angular_vb = vec_mul(rb_perp, body_b->angular_velocity);

		// Calculate total velocity 
		vec_2d va = vec_add(body_a->velocity, angular_va);
		vec_2d vb = vec_add(body_b->velocity, angular_vb);

		// Relative velocity between the two bodies
		vec_2d relative_vel = vec_sub(vb, va);

		// Calculate impulse magnitude (from Newcastle University)
		double j = -(1.0 + restitution) * vec_dot(relative_vel, collision.normal);
		j /= inv_mass_a + inv_mass_b + inv_inertia_a * square(vec_cross(ra, collision.normal)) + inv_inertia_b * square(vec_cross(rb, collision.normal));

		// Calculate and apply impulse to both bodies
		vec_2d impulse = vec_mul(collision.normal, j);

		body_a->velocity = vec_sub(body_a->velocity, vec_mul(impulse, inv_mass_a));
		body_a->angular_velocity -= inv_inertia_a * vec_cross(ra, impulse);

		body_b->velocity = vec_add(body_b->velocity, vec_mul(impulse, inv_mass_b));
		body_b->angular_velocity += inv_inertia_b * vec_cross(rb, impulse);

		// Calculate tangential vector
		vec_2d tangent = vec_sub(relative_vel, vec_mul(collision.normal, vec_dot(relative_vel, collision.normal)));

		if (vec_equals(tangent, vec_zero))
			return;
		else
			tangent = vec_normalize(tangent);

		double ra_perp_dot_t = vec_dot(ra_perp, tangent);
		double rb_perp_dot_t = vec_dot(rb_perp, tangent);

		// Calculate friction impulse magnitude (from Newcastle University)
		double j_tangent = -vec_dot(relative_vel, tangent);
		j_tangent /= inv_mass_a + inv_mass_b + inv_inertia_a * square(ra_perp_dot_t) + inv_inertia_b * square(rb_perp_dot_t);

		vec_2d friction_impulse {};

		if (std::abs(j_tangent) <= j * static_friction) // Apply static friction if the frictional impulse does not overcome it
			friction_impulse = vec_mul(tangent, j_tangent);
		else
			friction_impulse = vec_mul(tangent, -j * kinetic_friction); // Otherwise, apply kinetic friction

		// Apply impusle to both bodies
		body_a->velocity = vec_sub(body_a->velocity, vec_mul(friction_impulse, inv_mass_a));
		body_a->angular_velocity -= inv_inertia_a * vec_cross(ra, friction_impulse);

		body_b->velocity = vec_add(body_b->velocity, vec_mul(friction_impulse, inv_mass_b));
		body_b->angular_velocity += inv_inertia_b * vec_cross(rb, friction_impulse);
	}

	void world::step(double time, int substeps)
	{
		if (bodies.empty())
			return;

		// Calculate dt for each substep
		double dt = time / substeps;

		// Store performance benchmarks
		uint64_t collision_detection_time { 0 };
		uint64_t solve_constraints_time { 0 };
		uint64_t integrate_motion_time { 0 };

		// Update world for each substep
		for (int substep = 0; substep < substeps; substep++)
		{
			contacts.clear();
			timer.reset();

			for (physics::body& body : bodies)
			{
				//if (body.type == body_type::static_body)
				//	continue;
				
				// Update AABB and cache translated polygon vertices
				body.update_shape();
			}

			// O(n^2) collision detection
			// Can be improved with a broad-phase narrow-phase approach
			for (size_t i = 0; i < bodies.size() - 1; i++)
			{
				for (size_t j = i + 1; j < bodies.size(); j++)
				{
					if (i == j)
						continue;

					if (bodies[i].type == body_type::static_body && bodies[j].type == body_type::static_body)
						continue;

					// If AABBs do not intersect, there is no chance of collision 
					// Improves collision detection performance by ~10-20x 
					if (!physics::aabb_intersection(bodies[i].get_aabb(), bodies[j].get_aabb()))
					{
						continue;
					}

					// Test for collision between the two bodies
					physics::collision_manifold collision;
					if (physics::get_collision(&bodies[i], &bodies[j], collision))
					{
						// If objects are in contact, add to a list of contacts
						contacts.push_back(collision);
					}
				}
			}

			collision_detection_time += timer.elapsed<std::chrono::microseconds>();
			timer.reset();

			// Resolve each collision
			for (physics::collision_manifold& collision : contacts)
			{
				resolve_collision(collision, dt);
			}

			solve_constraints_time += timer.elapsed<std::chrono::microseconds>();
			timer.reset();

			for (size_t i = 0; i < bodies.size(); i++)
			{
				physics::body& body = bodies[i];

				double mass = body.get_mass();
				double inv_mass = body.get_inverse_mass();

				if (body.type == physics::static_body || mass <= 0 || inv_mass <= 0)
					continue;

				// obj.force * inv_mass
				physics::vec_2d acceleration{};
				//std::cout << body.force.x << ", " << body.force.y << "\n";
				acceleration.x = gravity.x + body.force.x * inv_mass;
				acceleration.y = gravity.y + body.force.y * inv_mass;

				// Velocity verlet integration
				body.position.x += body.velocity.x * dt + 0.5 * acceleration.x * dt * dt;
				body.position.y += body.velocity.y * dt + 0.5 * acceleration.y * dt * dt;

				body.velocity.x += acceleration.x * dt;
				body.velocity.y += acceleration.y * dt;

				body.rotation += body.angular_velocity * dt;

				body.force = vec_zero;
			}

			integrate_motion_time += timer.elapsed<std::chrono::microseconds>();
		}

		performance_report.collision_detection_time = collision_detection_time / substeps / 1000.0;
		performance_report.solve_constraints_time = solve_constraints_time / substeps / 1000.0;
		performance_report.integrate_motion_time = integrate_motion_time / substeps / 1000.0;
	}

	physics::performance_report world::get_step_performance() const
	{
		return performance_report;
	}
}