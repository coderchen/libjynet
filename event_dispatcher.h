#ifndef EVENT_DISPATCHER_H_
#define EVENT_DISPATCHER_H_

#include <stdint.h>

struct epoll_event;
class io_handler;
class io_listener;
class timer_handler;
class timer_min_heap;

class event_dispatcher
{
public:
  event_dispatcher();
	~event_dispatcher();

  int init();
  void run();

	int add_ev_mask(io_handler *handler, int had_ev_mask, int add_ev_mask);
	int del_ev_mask(io_handler *handler, int had_ev_mask, int del_ev_mask);
	int schedule_timer(timer_handler *handler, int ms);

private:
	void process_timeout(int64_t now_ms);
	void process_io_event(int nfds);

private:
  int epoll_fd_;
  int event_capacity_;
  epoll_event *events_;

  int handler_capacity_;
  io_handler **io_handlers_;

	timer_min_heap *timer_heap_;

private:
  event_dispatcher(const event_dispatcher&);
  event_dispatcher& operator= (const event_dispatcher&);
};

#endif
