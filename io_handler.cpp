#include "io_handler.h"

#include <sys/epoll.h>

#include "event_dispatcher.h"
#include "socket_utils.h"

io_handler::io_handler(event_dispatcher *dispatcher, int fd) 
  : sock_fd_(fd), 
	had_ev_mask_(io_handler::EV_NONE_MASK), 
	dispatcher_(dispatcher)
{
}
io_handler::~io_handler()
{
}

int io_handler::add_ev_mask(int ev_mask)
{
	int ret = this->dispatcher_->add_ev_mask(this, this->had_ev_mask_, ev_mask);
	if (ret == 0)
		this->had_ev_mask_ |= ev_mask;
	return ret;
}
int io_handler::del_ev_mask(int ev_mask)
{
	int ret = this->dispatcher_->del_ev_mask(this, this->had_ev_mask_, ev_mask);
	if (ret == 0)
		this->had_ev_mask_ &= ~ev_mask;
	return ret;
}
int io_handler::ev_mask_2_epoll_ev(int ev_mask) const
{
	int epoll_ev = 0;

	if (ev_mask & io_handler::EV_READ_MASK)
		epoll_ev |= EPOLLIN;
	if (ev_mask & io_handler::EV_WRITE_MASK)
		epoll_ev |= EPOLLOUT;

	return epoll_ev;
}
void io_handler::handle_connected()
{
	this->on_connected();
}
void io_handler::handle_disconnected()
{
	if (this->had_ev_mask_ != io_handler::EV_NONE_MASK)
		this->del_ev_mask(this->had_ev_mask_);
	socket_utils::close(this->sock_fd_);
	this->sock_fd_ = -1;
	this->on_disconnected();
}
