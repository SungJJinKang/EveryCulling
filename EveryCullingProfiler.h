#pragma once

#include "EveryCullingCore.h"

#include <unordered_map>
#include <string_view>
#include <chrono>
#include <atomic>

namespace culling
{
	class EveryCulling;
	class EveryCullingProfiler
	{
		friend class EveryCulling;

	public:

		struct ProfilingData
		{
			std::chrono::steady_clock::time_point mStartTime;
			std::chrono::steady_clock::time_point mEndTime;
			double mElapsedTime;

		};

	private:

		static thread_local bool IsLocalThreadRecordProfilingData;
		static std::atomic<bool> IsProfilingDataRecordByOtherThread;

		std::unordered_map<std::string_view, ProfilingData> mProfilingDatas;
		void SetStartTime(const char* const cullingModuleName);
		void SetEndTime(const char* const cullingModuleName);

	public:

		EveryCullingProfiler();

		double GetElapsedTime(const char* const cullingModuleName);
		const std::unordered_map<std::string_view, ProfilingData>& GetProfilingDatas() const;
	};
}


