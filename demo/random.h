#pragma once

#include <random>

namespace random
{
	// Fast pseudorandom number generation using xoroshiro128+
	// Generates numbers within range using Lemire's method, modified by M.E. O'Neill
	class fast_random
	{
	private:
		uint64_t state[2];

	public:
		fast_random();
		fast_random(uint64_t seed);

		uint64_t get_number();
		uint32_t get_number(uint32_t min, uint32_t max);
		int32_t get_number(int32_t min, int32_t max);

		float get_float();
		float get_float(float min, float max);

		double get_double();
		double get_double(double min, double max);
	};

	// Splitmix64 pseudorandom number generator 
	// Used to seed the internal states of the fast_random class 
	class splitmix64
	{
	private:
		uint64_t state;

	public:
		splitmix64();
		splitmix64(uint64_t seed);

		uint64_t get_number();
	};
}