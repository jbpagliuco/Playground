#pragma once

#include <chrono>

namespace playground
{
	class Duration
	{
	public:
		double AsSeconds()const;
		uint64_t AsMilliseconds()const;
		uint64_t AsMicroseconds()const;
		uint64_t AsNanoseconds()const;

	private:
		friend class Timestamp;

		typedef std::chrono::nanoseconds _Duration;
		_Duration mDuration;
	};

	class Timestamp
	{
	public:
		static Timestamp Now();

		Duration operator-(const Timestamp &rhs)const;

	private:
		typedef std::chrono::time_point<std::chrono::steady_clock> TimePoint;
		TimePoint mTimePoint;
	};

	class Timer
	{
	public:
		void Start();
		void Start(int intevalMs);

		bool Ticking()const;
		bool Elapsed()const;

		bool Valid()const;
		void Invalidate();

	private:
		static constexpr int INVALID_INTERVAL = -1;
		int mInterval = INVALID_INTERVAL;

		bool mValid = false;
		Timestamp mStart;
	};
}