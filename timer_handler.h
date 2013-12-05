#ifndef TIMER_HANDLER_H_
#define TIMER_HANDLER_H_

#include <stdint.h>

class event_dispatcher;

class timer_handler
{
	public:
		timer_handler();
		virtual ~timer_handler();

	public:
		virtual void handle_timeout(event_dispatcher *dispatcher, int64_t cur_ms) = 0;

	protected:
		int schedule_timer(event_dispatcher *dispatcher, int ms);

	private:
		timer_handler(const timer_handler&);
		timer_handler& operator=(const timer_handler&);
};

#endif
