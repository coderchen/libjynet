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
  : init_ok_(false),
		epoll_fd_(-1),
    max_events_(256),
    events_(NULL),
    max_handlers_(1024),
    io_handlers_(NULL),
		timer_heap_(NULL)
{ 
}
event_dispatcher::~event_dispatcher()
{
	this->clear();
}

int event_dispatcher::init()
{
	if (!this->init_ok_)
	{
		time_cache::instance()->update();
		do 
		{
			if ((this->epoll_fd_ = ::epoll_create(32000)) == -1) 
				break;

			if (!(this->events_ = new epoll_event[this->max_events_]))
				break;
			::memset(this->events_, 0, sizeof(epoll_event) * this->max_events_);

			if (!(this->io_handlers_ = new io_handler*[this->max_handlers_]))
				break;
			::memset(this->io_handlers_, 0, sizeof(void*) * this->max_handlers_);

			if (!(this->timer_heap_ = new timer_min_heap))
				break;

			this->init_ok_ = true;

		} while (0);

		if (!this->init_ok_)
			this->clear();
	}

  return this->init_ok_ ? 0 : -1;
}
void event_dispatcher::clear()
{
	if (this->epoll_fd_ != -1)
	{
		socket_utils::close(this->epoll_fd_);
		this->epoll_fd_ = -1;
	}
	if (this->events_)
	{
		delete []this->events_;
		this->events_ = NULL;
	}
	if (this->io_handlers_)
	{
		delete []this->io_handlers_;
		this->io_handlers_ = NULL;
	}
	if (this->timer_heap_)
	{
		delete this->timer_heap_;
		this->timer_heap_ = NULL;
	}

	this->init_ok_ = false;
	this->max_events_ = 256;
	this->max_handlers_ = 1024;
}
int event_dispatcher::grow_up_io_handlers()
{
	int new_max_handlers = this->max_handlers_ * 2;
	io_handler **new_io_handlers = new io_handler*[new_max_handlers];
	if (!new_io_handlers)
		return -1;

	::memset(new_io_handlers, 0, sizeof(void*) * new_max_handlers);
	::memcpy(new_io_handlers, this->io_handlers_, sizeof(void*) * this->max_handlers_);
	delete []this->io_handlers_;
	this->io_handlers_ = new_io_handlers;
	this->max_handlers_ = new_max_handlers;
	return 0;
}
int event_dispatcher::add_event(int fd, io_handler *handler, int had_ev, int add_ev)
{
	while (fd >= this->max_handlers_)
		if (this->grow_up_io_handlers() != 0)
			return -1;

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
int event_dispatcher::del_event(int fd, io_handler *handler, int had_ev, int del_ev)
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
			int ms = this->timer_heap_->nearest_timeout(time_cache::instance()->cur_ms());
			if (ms > 30 * 60 * 1000) // time too long ,kernerl befor 2.6.24 bug
				ms = 30 * 60 * 1000; // 30 minutes
      nfds = ::epoll_wait(this->epoll_fd_, this->events_, this->max_events_, ms);
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
		if (!handler) // what happend
			continue;
		int ret = 0;
		if (what & (EPOLLIN | EPOLLHUP | EPOLLERR))
			ret = handler->handle_input();
		if (ret == 0 && (what & EPOLLOUT))
			ret = handler->handle_output();

		if (ret != 0) //error
		{
			handler->on_disconnected();
			socket_utils::close(fd);
		}
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
