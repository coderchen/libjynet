#include "io_listener.h"

#include <errno.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#include "event_dispatcher.h"
#include "socket_utils.h"

io_listener::io_listener(event_dispatcher *dispatcher)
	: io_handler(dispatcher, -1)
{
}
io_listener::~io_listener()
{
}

int io_listener::listen(int port)
{
	if ((this->sock_fd_ = ::socket(AF_INET, SOCK_STREAM, 0)) == -1)
		return -1;

	socket_utils::set_reuse_addr(this->sock_fd_);
	socket_utils::set_nonblock(this->sock_fd_);

	if (socket_utils::bind(this->sock_fd_, port) == -1) {
		socket_utils::close(this->sock_fd_);
		this->sock_fd_ = -1;
		return -1;
	}

	if (::listen(this->sock_fd_, 1024) == -1) {
		socket_utils::close(this->sock_fd_);
		this->sock_fd_ = -1;
		return -1;
	}

	if (this->add_ev_mask(io_handler::EV_READ_MASK) == -1) {
		socket_utils::close(this->sock_fd_);
		this->sock_fd_ = -1;
		return -1;
	}

	return 0;
}
int io_listener::handle_input()
{
	while (1) {
		int conn_fd = socket_utils::accept(this->sock_fd_);
		if (conn_fd == -1) {
			if (errno != EAGAIN) {
				// log
			}
			break;
		}

		io_handler *handler = this->new_connection(this->dispatcher_,
																							 conn_fd);
		if (handler)
		 	handler->on_connected();
	}

	return 0;
}
