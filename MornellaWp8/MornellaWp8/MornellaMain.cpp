#include "pch.h"

#include <thread>
#include <chrono>

int mornellaMain(void)
{
	
		Windows::Foundation::TimeSpan span;
		span.Duration = 10000000L;   // convert 1 sec to 100ns ticks
	 
		Windows::Phone::Devices::Notification::VibrationDevice^ vibr = Windows::Phone::Devices::Notification::VibrationDevice::GetDefault();
		vibr->Vibrate(span);

		std::this_thread::sleep_for(std::chrono::milliseconds(2000));

		return 0;
	
}