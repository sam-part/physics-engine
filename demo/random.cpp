#include "random.h"

#include <iostream>

namespace random
{
	static inline uint64_t rotl(const uint64_t input, int n)
	{
		return (input << n) | (input >> (64 - n));
	}

	fast_random::fast_random()
	{
		state[0] = std::random_device{}();
		state[1] = std::random_device{}();
	}

	fast_random::fast_random(uint64_t seed)
	{
		splitmix64 rng(seed);

		state[0] = rng.get_number();
		state[1] = rng.get_number();
	}

	uint64_t fast_random::get_number()
	{
		uint64_t s0 = state[0];
		uint64_t s1 = state[1];

		uint64_t result = s0 + s1;
		s1 ^= s0;

		state[0] = rotl(s0, 55) ^ s1 ^ (s1 << 14);
		state[1] = rotl(s1, 36);

		return result;
	}

	// Implemented using Lemire's method, modified by M.E. O'Neill
	uint32_t fast_random::get_number(uint32_t min, uint32_t max)
	{
		uint32_t range = max - min + 1;

		uint32_t n = static_cast<uint32_t>(get_number());
		uint64_t m = static_cast<uint64_t>(n) * static_cast<uint64_t>(range);
		uint32_t l = static_cast<uint32_t>(m);

		if (l < range)
		{
#pragma warning(suppress:4146)
			uint32_t t = -range;

			if (t >= range)
			{
				t -= range;
				if (t >= range)
					t %= range;
			}

			while (l < t)
			{
				n = static_cast<uint32_t>(get_number());
				m = static_cast<uint64_t>(n) * static_cast<uint64_t>(range);
				l = static_cast<uint32_t>(m);
			}
		}

		return (m >> 32) + min;
	}

	int32_t fast_random::get_number(int32_t min, int32_t max)
	{
		uint32_t u_min = static_cast<int64_t>(min) + INT32_MAX + 1;
		uint32_t u_max = static_cast<int64_t>(max) + INT32_MAX + 1;

		return get_number(u_min, u_max) - (static_cast<uint32_t>(INT32_MAX) + 1);
	}

	float fast_random::get_float()
	{
		uint32_t number = get_number() >> 41 | 0x3f800000;
		return *reinterpret_cast<float*>(&number) - 1.0f;
	}

	float fast_random::get_float(float min, float max)
	{
		float range = max - min;
		return get_float() * range + min;
	}

	double fast_random::get_double()
	{
		uint64_t number = get_number() >> 12 | 0x3FF0000000000000;
		return *reinterpret_cast<double*>(&number) - 1.0;
	}

	double fast_random::get_double(double min, double max)
	{
		double range = max - min;
		return get_double() * range + min;
	}

	splitmix64::splitmix64()
	{
		state = std::random_device{}();
	}

	splitmix64::splitmix64(uint64_t seed)
		: state(seed)
	{}

	uint64_t splitmix64::get_number()
	{
		state += 0x9e3779b97f4a7c15;
		uint64_t z = state;
		z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
		z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
		return z ^ (z >> 31);
	}

}