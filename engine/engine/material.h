#pragma once

namespace physics
{
	struct material
	{
		// Material density in kg/m^2
		double density { 1.0 };

		// Coefficient of static friction 
		double static_friction { 0.4 };

		// Coefficient of kinetic friction 
		double kinetic_friction { 0.3 };
		
		// Coefficeint of restitution (elasticity)
		double restitution { 0.8 };

		material() = default;
	};
}