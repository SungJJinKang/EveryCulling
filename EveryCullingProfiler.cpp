#include "EveryCullingProfiler.h"

thread_local bool culling::EveryCullingProfiler::IsLocalThreadRecordProfilingData = false;
std::atomic<bool> culling::EveryCullingProfiler::IsProfilingDataRecordByOtherThread = false;

void culling::EveryCullingProfiler::SetStartTime(const char* const cullingModuleName)
{
	bool expected = false;
	if (IsProfilingDataRecordByOtherThread.compare_exchange_strong(expected, true) == true)
	{
		IsLocalThreadRecordProfilingData = true;

		mProfilingDatas[std::string_view{ cullingModuleName }].mStartTime = std::chrono::high_resolution_clock::now();
	}
	
}

void culling::EveryCullingProfiler::SetEndTime(const char* const cullingModuleName)
{
	if (IsLocalThreadRecordProfilingData == true)
	{
		ProfilingData& profilingData = mProfilingDatas[std::string_view{ cullingModuleName }];

		profilingData.mEndTime = std::chrono::high_resolution_clock::now();
		profilingData.mElapsedTime = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(profilingData.mEndTime - profilingData.mStartTime).count();

		IsLocalThreadRecordProfilingData = false;
		IsProfilingDataRecordByOtherThread = false;
	}
	
}

culling::EveryCullingProfiler::EveryCullingProfiler()
	: mProfilingDatas{}
{

}

double culling::EveryCullingProfiler::GetElapsedTime(const char* const cullingModuleName)
{
	return mProfilingDatas[cullingModuleName].mElapsedTime;
}

const std::unordered_map<std::string_view, culling::EveryCullingProfiler::ProfilingData>& culling::EveryCullingProfiler::GetProfilingDatas() const
{
	return mProfilingDatas;
}