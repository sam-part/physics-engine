#include "transform.h"

namespace physics
{
	transform::transform(double x, double y)
		: x(x), y(y)
	{}

	transform::transform(double x, double y, double rotation)
		: x(x), y(y), rotation(rotation)
	{}
}