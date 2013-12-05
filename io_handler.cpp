#include "io_handler.h"

#include <sys/epoll.h>

#include "event_dispatcher.h"
#include "socket_utils.h"

io_handler::io_handler(event_dispatcher *dispatcher, int fd) 
  : sock_fd_(fd), had_ev_(0), dispatcher_(dispatcher)
{
}
io_handler::~io_handler()
{
}

int io_handler::add_event(int ev)
{
	int ret = this->dispatcher_->add_event(this->sock_fd_, 
																				 this, 
																				 this->had_ev_, 
																				 ev);
	if (ret == 0)
		this->had_ev_ |= ev;
	return ret;
}
int io_handler::del_event(int ev)
{
	int ret = this->dispatcher_->del_event(this->sock_fd_, 
																				 this,
																				 this->had_ev_, 
																				 ev);
	if (ret == 0)
		this->had_ev_ &= ~ev;
	return ret;
}
int io_handler::connected()
{
	socket_utils::set_nonblock(this->sock_fd_);
	this->add_event(EPOLLIN | EPOLLOUT | EPOLLET);
	return this->on_connected();
}
int io_handler::disconnected()
{
	socket_utils::close(this->sock_fd_);
	this->sock_fd_ = -1;
	this->del_event(this->had_ev_);
	return this->on_disconnected();
}
