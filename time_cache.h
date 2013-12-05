#ifndef TIME_CACHE_H_
#define TIME_CACHE_H_

#include <stdint.h>
#include <sys/time.h>

class time_cache
{
	public:
		static time_cache* instance();

		void update();

		int64_t cur_ms();
		int64_t cur_sec();

	private:
		timeval tv_;

	private:
		time_cache();
};

#endif
