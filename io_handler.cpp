#include "io_handler.h"

#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include "event_dispatcher.h"
#include "socket_utils.h"

int io_handler::ev_read = EPOLLIN;
int io_handler::ev_write = EPOLLOUT;

io_handler::io_handler(event_dispatcher *dispatcher, int fd) 
  : sock_fd_(fd), had_ev_(0), dispatcher_(dispatcher)
{
}
io_handler::~io_handler()
{
}

int io_handler::add_event(int ev)
{
	int ret = this->dispatcher_->add_event(this->sock_fd_, this, this->had_ev_, ev);
	if (ret == 0)
		this->had_ev_ |= ev;
	return ret;
}
int io_handler::del_event(int ev)
{
	int ret = this->dispatcher_->del_event(this->sock_fd_, this, this->had_ev_, ev);
	if (ret == 0)
		this->had_ev_ &= ~ev;
	return ret;
}
