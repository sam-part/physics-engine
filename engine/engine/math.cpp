#include "math.h"
#include <cmath>

namespace physics
{
	vec_2d::vec_2d(double x, double y)
		: x(x), y(y)
	{}

	vec_2d vec_add(vec_2d lhs, vec_2d rhs)
	{
		return vec_2d(lhs.x + rhs.x, lhs.y + rhs.y);
	}

	vec_2d vec_sub(vec_2d lhs, vec_2d rhs)
	{
		return vec_2d(lhs.x - rhs.x, lhs.y - rhs.y);
	}

	double vec_dot(vec_2d lhs, vec_2d rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y;
	}

	double vec_cross(vec_2d lhs, vec_2d rhs)
	{
		return lhs.x * rhs.y - lhs.y * rhs.x;
	}

	vec_2d vec_cross(vec_2d vector, double factor)
	{
		return vec_2d(vector.y * factor, -vector.x * factor);
	}

	vec_2d vec_mul(vec_2d lhs, vec_2d rhs)
	{
		return vec_2d(lhs.x * rhs.x, lhs.y * rhs.y);
	}

	vec_2d vec_mul(vec_2d vector, double factor)
	{
		return vec_2d(vector.x * factor, vector.y * factor);
	}

	vec_2d vec_div(vec_2d vector, double factor)
	{
		return vec_2d(vector.x / factor, vector.y / factor);
	}

	double vec_magnitude_sq(vec_2d vector)
	{
		return vector.x * vector.x + vector.y * vector.y;
	}

	double vec_magnitude(vec_2d vector)
	{
		return std::sqrt(vector.x * vector.x + vector.y * vector.y);
	}

	vec_2d vec_normalize(vec_2d vector)
	{
		double inv_magnitude = 1.0 / vec_magnitude(vector);
		return vec_2d(vector.x * inv_magnitude, vector.y * inv_magnitude);
	}

	double get_distance(vec_2d lhs, vec_2d rhs)
	{
		double dx = lhs.x - rhs.x;
		double dy = lhs.y - rhs.y;
		return std::sqrt(dx * dx + dy * dy);
	}

	double get_distance_sq(vec_2d lhs, vec_2d rhs)
	{
		double dx = lhs.x - rhs.x;
		double dy = lhs.y - rhs.y;
		return dx * dx + dy * dy;
	}

	vec_2d rotate_point(vec_2d point, double theta)
	{
		return vec_2d(point.x * std::cos(theta) - point.y * std::sin(theta), point.y * cos(theta) + point.x * sin(theta));
	}

	double deg_to_rad(double theta)
	{
		return theta * physics::pi / 180.0;
	}

	double rad_to_deg(double theta)
	{
		return theta * 180.0 / physics::pi;
	}

	bool equals(double lhs, double rhs)
	{
		return std::fabs(lhs - rhs) < fp_compare_epsilon;
	}

	bool vec_equals(vec_2d lhs, vec_2d rhs)
	{
		return equals(lhs.x, rhs.x) && equals(lhs.y, rhs.y);
	}
	double square(double number)
	{
		return number * number;
	}
}