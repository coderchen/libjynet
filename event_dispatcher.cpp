#include "event_dispatcher.h"

#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#include "io_handler.h"
#include "io_listener.h"
#include "socket_utils.h"
#include "time_cache.h"
#include "timer_min_heap.h"
#include "timer_handler.h"

event_dispatcher::event_dispatcher()
  : epoll_fd_(-1),
    event_capacity_(256),
    events_(NULL),
    handler_capacity_(1024),
    io_handlers_(NULL),
		timer_heap_(NULL)
{ 
}
event_dispatcher::~event_dispatcher()
{
	if (this->epoll_fd_ != -1)
		socket_utils::close(this->epoll_fd_);
	if (this->events_)
		delete []this->events_;
	if (this->io_handlers_)
		delete []this->io_handlers_;
	if (this->timer_heap_)
		delete this->timer_heap_;
}

int event_dispatcher::init()
{
	if ((this->epoll_fd_ = ::epoll_create(32000)) == -1) 
		return -1;

	if (!(this->events_ = new epoll_event[this->event_capacity_])) {
		socket_utils::close(this->epoll_fd_);
		this->epoll_fd_ = -1;
		return -1;
	}
	::memset(this->events_, 0, sizeof(epoll_event) * this->event_capacity_);

	if (!(this->io_handlers_ = new io_handler*[this->handler_capacity_])) {
		socket_utils::close(this->epoll_fd_);
		this->epoll_fd_ = -1;
		delete []this->events_;
		return -1;
	}
	::memset(this->io_handlers_, 0, sizeof(void*) * this->handler_capacity_);

	if (!(this->timer_heap_ = new timer_min_heap)) {
		socket_utils::close(this->epoll_fd_);
		this->epoll_fd_ = -1;
		delete []this->events_;
		delete []this->io_handlers_;
		return -1;
	}

	time_cache::instance()->update();
	return 0;
}
int event_dispatcher::add_event(int fd, 
																io_handler *handler, 
																int had_ev, 
																int add_ev)
{
	while (fd >= this->handler_capacity_) {
		int new_capacity = this->handler_capacity_ * 2;
		io_handler **new_io_handlers = new io_handler*[new_capacity];
		if (!new_io_handlers)
			return -1;

		::memset(new_io_handlers, 0, sizeof(void*) * new_capacity);
		::memcpy(new_io_handlers, 
						this->io_handlers_, 
						sizeof(void*) * this->handler_capacity_);
		delete []this->io_handlers_;
		this->io_handlers_ = new_io_handlers;
		this->handler_capacity_ = new_capacity;
	}

	int op = (had_ev == 0) ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;
	epoll_event ee;
	::memset(&ee, 0, sizeof(ee));
	ee.events = had_ev | add_ev;
	ee.data.fd = fd;
	int ret = ::epoll_ctl(this->epoll_fd_, op, fd, &ee);
	if (ret == 0)
		this->io_handlers_[fd] = handler;
	return ret;
}
int event_dispatcher::del_event(int fd,
																io_handler *handler,
															 	int had_ev, 
																int del_ev)
{
	if (!(had_ev & del_ev))
		return 0;

	had_ev &= ~del_ev;
	int op = (had_ev == 0) ? EPOLL_CTL_DEL : EPOLL_CTL_MOD;
	epoll_event ee;
	::memset(&ee, 0, sizeof(ee));
	ee.events = had_ev;
	ee.data.fd = fd;
	int ret = ::epoll_ctl(this->epoll_fd_, op, fd, &ee); 
	if (ret == 0 && had_ev == 0)
		this->io_handlers_[fd] = NULL;
	return ret;
}
void event_dispatcher::run()
{
  while (1)
  {
    int nfds = 0;
    do
    {
			time_cache::instance()->update();
			int64_t now_ms = time_cache::instance()->cur_ms();
			int ms = this->timer_heap_->nearest_timeout(now_ms);
			if (ms > 30 * 60 * 1000) // time too long ,kernerl befor 2.6.24 bug
				ms = 30 * 60 * 1000; // 30 minutes
      nfds = ::epoll_wait(this->epoll_fd_, 
													this->events_,
												 	this->event_capacity_,
												 	ms);
    } while (nfds == -1 && errno == EINTR);

		time_cache::instance()->update();
		this->process_timeout(time_cache::instance()->cur_ms());
		this->process_io_event(nfds);
	}
}
void event_dispatcher::process_io_event(int nfds)
{
	for (int i = 0; i < nfds; ++i)
	{
		int fd = this->events_[i].data.fd;
		int what = this->events_[i].events;
		io_handler *handler = this->io_handlers_[fd];
		if (!handler) // what happend ???
			continue;
		int ret = 0;
		if (what & (EPOLLIN | EPOLLHUP | EPOLLERR))
			ret = handler->handle_input();
		if (ret == 0 && (what & EPOLLOUT))
			ret = handler->handle_output();

		if (ret != 0)
			handler->disconnected();
	}
}
void event_dispatcher::process_timeout(int64_t cur_ms)
{
	timer_handler *handler = NULL;
	while ((handler = this->timer_heap_->check_timeout(cur_ms)) != NULL)
		handler->handle_timeout(this, cur_ms);
}
int event_dispatcher::schedule_timer(timer_handler *handler, int ms)
{
	int64_t dead_line = time_cache::instance()->cur_ms() + ms;
	return this->timer_heap_->schedule_timer(dead_line, handler);
}
