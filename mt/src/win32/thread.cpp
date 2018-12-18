#include <mt/thread.h>

#include <windows.h>

namespace mt
{
	namespace this_thread
	{
		thread::id get_id()
		{	return ::GetCurrentThreadId();	}

		void sleep_for(milliseconds period)
		{	::Sleep(period);	}
	}
}
