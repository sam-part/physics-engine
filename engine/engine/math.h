#pragma once

namespace physics
{
	// 2D vector class
	struct vec_2d
	{
		double x { 0.0 };
		double y { 0.0 };

		vec_2d() = default;
		vec_2d(double x, double y);
	};

	const vec_2d vec_zero { 0.0, 0.0 };

	vec_2d vec_add(vec_2d lhs, vec_2d rhs);
	vec_2d vec_sub(vec_2d lhs, vec_2d rhs);
	double vec_dot(vec_2d lhs, vec_2d rhs);
	double vec_cross(vec_2d lhs, vec_2d rhs);
	vec_2d vec_cross(vec_2d vector, double factor);
	vec_2d vec_mul(vec_2d lhs, vec_2d rhs);
	vec_2d vec_mul(vec_2d vector, double factor);
	vec_2d vec_div(vec_2d vector, double factor);

	double vec_magnitude_sq(vec_2d vector);
	double vec_magnitude(vec_2d vector);
	vec_2d vec_normalize(vec_2d vector);

	double get_distance(vec_2d lhs, vec_2d rhs);
	double get_distance_sq(vec_2d lhs, vec_2d rhs);

	constexpr double pi = 3.14159265358979323846;

	vec_2d rotate_point(vec_2d point, double theta);
	double deg_to_rad(double theta);
	double rad_to_deg(double theta);

	constexpr double fp_compare_epsilon = 1e-5;
	bool equals(double lhs, double rhs);
	bool vec_equals(vec_2d lhs, vec_2d rhs);

	double square(double number);
}