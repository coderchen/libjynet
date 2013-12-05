#include "time_cache.h"

#include <stddef.h>


time_cache* time_cache::instance()
{
	static time_cache tc;
	return &tc;
}

time_cache::time_cache()
{
}

void time_cache::update()
{
	::gettimeofday(&this->tv_, NULL);
}

int64_t time_cache::cur_ms()
{
	return (int64_t)this->tv_.tv_sec * 1000 + this->tv_.tv_usec / 1000;
}

int64_t time_cache::cur_sec()
{
	return (int64_t)this->tv_.tv_sec;
}
