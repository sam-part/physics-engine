#pragma once

namespace physics
{
	struct transform
	{
		double x { 0.0 };
		double y { 0.0 };

		// Rotation in radians
		double rotation { 0.0 }; 

		transform() = default;
		transform(double x, double y);
		transform(double x, double y, double rotation);
	};
}