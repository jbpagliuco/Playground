#include "Timer.h"

#include "Debug/Assert.h"
#include "Debug/Log.h"

namespace playground
{
	double Duration::AsSeconds()const
	{
		return std::chrono::duration_cast<std::chrono::duration<double>>(mDuration).count();
	}

	uint64_t Duration::AsMilliseconds()const
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(mDuration).count();
	}

	uint64_t Duration::AsMicroseconds()const
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(mDuration).count();
	}

	uint64_t Duration::AsNanoseconds()const
	{
		return std::chrono::duration_cast<std::chrono::nanoseconds>(mDuration).count();
	}


	Timestamp Timestamp::Now()
	{
		Timestamp t;
		t.mTimePoint = std::chrono::high_resolution_clock::now();

		return t;
	}

	Duration Timestamp::operator-(const Timestamp &rhs)const
	{
		Duration d;
		d.mDuration = mTimePoint - rhs.mTimePoint;

		return d;
	}


	void Timer::Start()
	{
		CORE_ASSERT_RETURN(mInterval != INVALID_INTERVAL, "You must initialize the timer before using it.");

		mValid = true;
		mStart = Timestamp::Now();
	}

	void Timer::Start(int intervalMs)
	{
		mInterval = intervalMs;
		Start();
	}

	bool Timer::Ticking()const
	{
		const Duration dT = Timestamp::Now() - mStart;
		return dT.AsMilliseconds() < mInterval;
	}

	bool Timer::Elapsed()const
	{
		const Duration dT = Timestamp::Now() - mStart;
		return dT.AsMilliseconds() >= mInterval;
	}

	bool Timer::Valid()const
	{
		return mValid;
	}

	void Timer::Invalidate()
	{
		mValid = false;
	}


	TimeProfiler::TimeProfiler(const char* name)
	{
		mName = name;
		mStartTime = Timestamp::Now();
	}

	TimeProfiler::~TimeProfiler()
	{
		const Duration duration = Timestamp::Now() - mStartTime;
		LogInfo("Timer", "%s took %lu microseconds", mName, duration.AsMicroseconds());
	}
}