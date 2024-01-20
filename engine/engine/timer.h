#pragma once

#include <chrono>
#include <cstdint>

namespace physics
{
	class timer
	{
		using clock = std::chrono::high_resolution_clock;

	private:
		clock::time_point start{};

	public:
		timer()
			: start(clock::now())
		{}

		void reset()
		{
			start = clock::now();
		}

		template<typename units = std::chrono::milliseconds>
		uint64_t elapsed() const
		{
			return std::chrono::duration_cast<units>(clock::now() - start).count();
		}
	};
}