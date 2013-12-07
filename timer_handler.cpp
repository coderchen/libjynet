#include "timer_handler.h"

#include "event_dispatcher.h"

timer_handler::timer_handler()
{
}
timer_handler::~timer_handler()
{
}

int timer_handler::schedule_timer(event_dispatcher *timer_dispatcher,
																	int ms)
{
	if (!timer_dispatcher || ms <= 0)
		return -1;
	return timer_dispatcher->schedule_timer(this, ms);
}
