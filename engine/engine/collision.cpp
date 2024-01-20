#include "collision.h"
#include <vector>

#include <iostream>
#include <algorithm>

namespace physics
{
	physics::vec_2d calculate_axis(physics::vec_2d current_point, physics::vec_2d next_point)
	{
		physics::vec_2d edge = vec_sub(next_point, current_point);
		physics::vec_2d normal {-edge.y, edge.x};
		double magnitude = vec_magnitude(edge);
		// axis does not need to be normalized right now, can be done later to save sqrt calls
		return physics::vec_2d {normal.x / magnitude, normal.y / magnitude};
	}

	struct projection
	{
		double min{ DBL_MAX };
		double max{ -DBL_MAX };
	}; 

	physics::projection project_polygon(const std::vector<physics::vec_2d>& vertices, physics::vec_2d axis)
	{
		physics::projection projection {};

		for (size_t i = 0; i < vertices.size(); i++)
		{
			double dot = vec_dot(vertices[i], axis);
			
			if (dot < projection.min)
				projection.min = dot;

			if (dot > projection.max)
				projection.max = dot;
		}

		return projection;
	}

	physics::projection project_circle(physics::vec_2d center, double radius, physics::vec_2d axis)
	{
		physics::vec_2d circle_product = vec_mul(axis, radius);

		physics::vec_2d point_a = vec_sub(center, circle_product);
		physics::vec_2d point_b = vec_add(center, circle_product);
		
		physics::projection projection;
		projection.min = vec_dot(point_a, axis);
		projection.max = vec_dot(point_b, axis);

		if (projection.min > projection.max)
			std::swap(projection.min, projection.max);

		return projection;
	}

	bool projection_overlap(physics::projection a, physics::projection b)
	{
		return !(b.max <= a.min || a.max <= b.min);
	}

	struct point_segment_info
	{
		double distance_squared { DBL_MAX };
		physics::vec_2d point {};
	};

	// Finds the closest point on a line to another point
	physics::point_segment_info get_closest_point(physics::vec_2d point, physics::vec_2d vertex_a, physics::vec_2d vertex_b)
	{
		physics::vec_2d vertex_diff = vec_sub(vertex_b, vertex_a);
		physics::vec_2d origin_diff = vec_sub(point, vertex_a);

		double projection = vec_dot(vertex_diff, origin_diff);
		double length_sq = vec_dot(vertex_diff, vertex_diff);

		physics::point_segment_info point_info {};

		if (projection <= 0.0)
		{
			point_info.point = vertex_a;
		}
		else if (projection >= length_sq)
		{
			point_info.point = vertex_b;
		}
		else
		{
			point_info.point = vec_add(vertex_a, vec_mul(vertex_diff, projection / length_sq));
		}

		point_info.distance_squared = get_distance_sq(point, point_info.point);

		return point_info;
	}

	// Collision between two polygons using seperate axis theorem
	bool get_polygon_collision(const std::vector<physics::vec_2d>& vertices_a, physics::vec_2d origin_a, const std::vector<physics::vec_2d>& vertices_b, physics::vec_2d origin_b, physics::collision_manifold& collision)
	{
		double depth = DBL_MAX;
		physics::vec_2d normal{};

		for (size_t i = 0; i < vertices_a.size(); i++)
		{
			const physics::vec_2d current_point = vertices_a[i];
			const physics::vec_2d next_point = vertices_a[(i + 1) % vertices_a.size()];
			const physics::vec_2d axis_normalized = calculate_axis(current_point, next_point);

			physics::projection projection_a = project_polygon(vertices_a, axis_normalized);
			physics::projection projection_b = project_polygon(vertices_b, axis_normalized);

			if (projection_overlap(projection_a, projection_b))
			{
				double axis_depth = std::min(projection_b.max - projection_a.min, projection_a.max - projection_b.min);

				if (axis_depth < depth)
				{
					depth = axis_depth;
					normal = axis_normalized;
				}
			}
			else
			{
				return false;
			}
		}

		for (size_t i = 0; i < vertices_b.size(); i++)
		{
			const physics::vec_2d current_point = vertices_b[i];
			const physics::vec_2d next_point = vertices_b[(i + 1) % vertices_b.size()];
			const physics::vec_2d axis_normalized = calculate_axis(current_point, next_point);

			physics::projection projection_a = project_polygon(vertices_a, axis_normalized);
			physics::projection projection_b = project_polygon(vertices_b, axis_normalized);

			if (projection_overlap(projection_a, projection_b))
			{
				double axis_depth = std::min(projection_b.max - projection_a.min, projection_a.max - projection_b.min);

				if (axis_depth < depth)
				{
					depth = axis_depth;
					normal = axis_normalized;
				}
			}
			else
			{
				return false;
			}
		}

		physics::vec_2d direction = vec_sub(origin_a, origin_b);

		size_t num_contacts = 0;
		physics::vec_2d contact_a {};
		physics::vec_2d contact_b {};

		double closest_distance = DBL_MAX;

		for (size_t i = 0; i < vertices_a.size(); i++)
		{
			physics::vec_2d point_a = vertices_a[i];

			for (size_t j = 0; j < vertices_b.size(); j++)
			{
				const physics::vec_2d current_point = vertices_b[j];
				const physics::vec_2d next_point = vertices_b[(j + 1) % vertices_b.size()];

				physics::point_segment_info point_info = get_closest_point(point_a, current_point, next_point);

				if (physics::equals(point_info.distance_squared, closest_distance))
				{
					if (physics::vec_equals(contact_a, point_info.point))
						continue;

					num_contacts = 2;
					contact_b = point_info.point;
				}
				else if (point_info.distance_squared < closest_distance)
				{
					num_contacts = 1;
					contact_a = point_info.point;
					closest_distance = point_info.distance_squared;
				}
			}
		}

		for (size_t i = 0; i < vertices_b.size(); i++)
		{
			physics::vec_2d point_a = vertices_b[i];

			for (size_t j = 0; j < vertices_a.size(); j++)
			{
				const physics::vec_2d current_point = vertices_a[j];
				const physics::vec_2d next_point = vertices_a[(j + 1) % vertices_a.size()];

				physics::point_segment_info point_info = get_closest_point(point_a, current_point, next_point);

				if (physics::equals(point_info.distance_squared, closest_distance))
				{
					if (physics::vec_equals(contact_a, point_info.point))
						continue;

					num_contacts = 2;
					contact_b = point_info.point;
				}
				else if (point_info.distance_squared < closest_distance)
				{
					num_contacts = 1;
					contact_a = point_info.point;
					closest_distance = point_info.distance_squared;
				}
			}
		}

		collision.depth = depth;
		collision.normal = normal;

		collision.contact_points.push_back(contact_a);
		if (num_contacts == 2)
			collision.contact_points.push_back(contact_b);

		return true;
	}

	// Collision between polygon and circle using seperate axis theorem
	bool get_polygon_circle_collision(const std::vector<physics::vec_2d>& vertices, physics::vec_2d polygon_origin, physics::vec_2d circle_origin, double circle_radius, physics::collision_manifold& collision)
	{
		double depth = DBL_MAX;
		physics::vec_2d normal{};

		for (size_t i = 0; i < vertices.size(); i++)
		{
			const physics::vec_2d current_point = vertices[i];
			const physics::vec_2d next_point = vertices[(i + 1) % vertices.size()];
			const physics::vec_2d axis_normalized = calculate_axis(current_point, next_point);

			physics::projection projection_a = project_polygon(vertices, axis_normalized);
			physics::projection projection_b = project_circle(circle_origin, circle_radius, axis_normalized);

			if (projection_overlap(projection_a, projection_b))
			{
				double axis_depth = std::min(projection_b.max - projection_a.min, projection_a.max - projection_b.min);

				if (axis_depth < depth)
				{
					depth = axis_depth;
					normal = axis_normalized;
				}
			}
			else
			{
				return false;
			}
		}

		double closest_distance{ DBL_MAX };
		size_t closest_index{};

		for (size_t i = 0; i < vertices.size(); i++)
		{
			double dx = vertices[i].x - circle_origin.x;
			double dy = vertices[i].y - circle_origin.y;

			double distance_sq = dx * dx + dy * dy;

			if (distance_sq < closest_distance)
			{
				closest_distance = distance_sq;
				closest_index = i;
			}
		}

		physics::vec_2d closest_vertex = vertices[closest_index];
		physics::vec_2d axis = vec_sub(closest_vertex, circle_origin);
			
		double magnitude = std::hypot(axis.x, axis.y);
		axis = { axis.x / magnitude, axis.y / magnitude };

		physics::projection polygon_projection = project_polygon(vertices, axis);
		physics::projection circle_projection = project_circle(circle_origin, circle_radius, axis);

		if (projection_overlap(polygon_projection, circle_projection))
		{
			double axis_depth = std::min(circle_projection.max - polygon_projection.min, polygon_projection.max - circle_projection.min);

			if (axis_depth < depth)
			{
				depth = axis_depth;
				normal = axis;
			}
		}
		else
		{
			return false;
		}

		physics::vec_2d direction = vec_sub(polygon_origin, circle_origin);

		physics::point_segment_info closest_point;
		closest_point.distance_squared = DBL_MAX;

		for (size_t i = 0; i < vertices.size(); i++)
		{
			const physics::vec_2d current_point = vertices[i];
			const physics::vec_2d next_point = vertices[(i + 1) % vertices.size()];

			physics::point_segment_info point_info = get_closest_point(circle_origin, current_point, next_point);

			if (point_info.distance_squared < closest_point.distance_squared)
			{
				closest_point = point_info;
			}
		}

		collision.contact_points.push_back(closest_point.point);
		collision.depth = depth;
		collision.normal = normal;

		return true;
	}

	// Collisions between two circles
	bool get_circle_collision(physics::vec_2d origin_a, double radius_a, physics::vec_2d origin_b, double radius_b, physics::collision_manifold& collision)
	{
		double center_distance = physics::get_distance(origin_a, origin_b);
		double combined_radius = radius_a + radius_b;

		bool collided = center_distance <= combined_radius;

		if (collided)
		{
			collision.depth = combined_radius - center_distance;
			collision.normal = vec_normalize(vec_sub(origin_b, origin_a));

			physics::vec_2d collision_point = vec_sub(origin_b, vec_mul(collision.normal, radius_b));
			collision.contact_points.push_back(collision_point);

			return true;
		}

		return false;
	}

	bool get_collision(physics::body* body_a, physics::body* body_b, physics::collision_manifold& collision)
	{
		collision.body_a = body_a;
		collision.body_b = body_b;

		const physics::shape* shape_a = body_a->get_shape();
		const physics::shape* shape_b = body_b->get_shape();

		physics::shape_type type_a = shape_a->get_type();
		physics::shape_type type_b = shape_b->get_type();

		physics::vec_2d origin_a = body_a->get_position();
		double rotation_a = body_a->get_rotation();

		physics::vec_2d origin_b = body_b->get_position();
		double rotation_b = body_b->get_rotation();

		// Check each shape type and call the according collision function
		if (type_a == physics::shape_type::polygon && type_b == physics::shape_type::polygon)
		{
			// Polygon-polygon collision check
			std::vector<physics::vec_2d> vertices_a = body_a->get_translated_vertices();
			std::vector<physics::vec_2d> vertices_b = body_b->get_translated_vertices();

			return get_polygon_collision(vertices_a, origin_a, vertices_b, origin_b, collision);
		}
		else if (type_a == physics::shape_type::polygon && type_b == physics::shape_type::circle)
		{
			// Polygon-circle collision check
			std::vector<physics::vec_2d> vertices = body_a->get_translated_vertices();

			double radius = static_cast<const physics::circle*>(shape_b)->get_radius();

			return get_polygon_circle_collision(vertices, origin_a, origin_b, radius, collision);
		}
		else if (type_a == physics::shape_type::circle && type_b == physics::shape_type::polygon)
		{
			// Polygon-circle collision check

			double radius = static_cast<const physics::circle*>(shape_a)->get_radius();

			std::vector<physics::vec_2d> vertices = body_b->get_translated_vertices();

			return get_polygon_circle_collision(vertices, origin_b, origin_a, radius, collision);
		}
		else if (type_a == physics::shape_type::circle && type_b == physics::shape_type::circle)
		{
			// Circle-circle collision check

			double radius_a = static_cast<const physics::circle*>(shape_a)->get_radius();
			double radius_b = static_cast<const physics::circle*>(shape_b)->get_radius();

			return get_circle_collision(origin_a, radius_a, origin_b, radius_b, collision);
		}

		return false;
	}
}