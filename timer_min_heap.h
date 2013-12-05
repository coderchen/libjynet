#ifndef TIMER_MIN_HEAP_H_
#define TIMER_MIN_HEAP_H_

#include <stdint.h>

class timer_handler;

struct timer_node
{
	int64_t dead_line_;
	timer_handler *handler_;
};

class timer_min_heap
{
	public:
		timer_min_heap();
		~timer_min_heap();

	public:
		timer_handler* check_timeout(int64_t now_ms);
		int schedule_timer(int64_t dead_line, timer_handler *handler);
		int nearest_timeout(int64_t now_ms);

	private:
		int grow_up();
		void adjust_down(int slot);
		void adjust_up(int slot);

	private:
		int capacity_;
		int size_;
		timer_node *timers_;

	private:
		timer_min_heap(const timer_min_heap&);
		timer_min_heap& operator=(const timer_min_heap&);
};

#endif
