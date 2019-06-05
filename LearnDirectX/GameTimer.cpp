#include "GameTimer.h"
#include "stdafx.h"

GameTimer::GameTimer() :
	mSecondsPerCount(0.f),
	mDeltaTime(-1.f),
	mBaseTime(0),
	mPausedTime(0),
	mStopTime(0),
	mPrevTime(0),
	mCurrTime(0)
{
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)& countsPerSec);
	mSecondsPerCount = 1. / (double)countsPerSec;
}

float GameTimer::TotalTime() const
{
	if (mStopped)
	{
		return (float)(((mStopTime - mPausedTime) - mBaseTime) * mSecondsPerCount);
	}
	else
	{
		return (float)(((mCurrTime - mPausedTime) - mBaseTime) * mSecondsPerCount);
	}
	return 0.0f;
}

float GameTimer::DeltaTime() const
{
	return mDeltaTime;
}

void GameTimer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)& currTime);
	mBaseTime = currTime;
	mPrevTime = currTime;
	mStopTime = 0;
	mStopped = false;	
}

void GameTimer::Start()
{
	__int64 startTime;
	QueryPerformanceFrequency((LARGE_INTEGER*)&startTime);

	if (mStopped)
	{
		mPausedTime += (startTime - mStopTime);
		mPrevTime = startTime;
		mStopTime = 0;
		mStopped = false;
	}
}

void GameTimer::Stop()
{
	if (!mStopped)
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)& currTime);
		mStopTime = currTime;
	}
}

void GameTimer::Tick()
{
	if (mStopped)
	{
		mDeltaTime = 0.f;
		return;
	}

	__int64 currTime;
	QueryPerformanceFrequency((LARGE_INTEGER*)& currTime);
	mCurrTime = currTime;

	mDeltaTime = (mCurrTime - mPrevTime) * mSecondsPerCount;
	mPrevTime = mCurrTime;

	if (mDeltaTime < 0.f)
	{
		mDeltaTime = 0.f;
	}
}
