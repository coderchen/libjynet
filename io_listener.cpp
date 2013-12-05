#include "io_listener.h"

#include <errno.h>
#include <signal.h>
#include <sys/socket.h>

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
	bool listen_ok = false;

	do
	{
		if ((this->sock_fd_ = ::socket(AF_INET, SOCK_STREAM, 0)) == -1)
			break;

		socket_utils::set_reuse_addr(this->sock_fd_);
		if (socket_utils::bind(this->sock_fd_, port) == -1)
			break;
		
		if (::listen(this->sock_fd_, 1024) == -1)
			break;

		if (this->add_event(io_handler::ev_read) == -1)
			break;

		listen_ok = true;
	} while (0);

	if (!listen_ok && this->sock_fd_ != -1)
	{
		socket_utils::close(this->sock_fd_);
		this->sock_fd_ = -1;
	}	
	return listen_ok ? 0 : -1;
}
int io_listener::handle_input()
{
	int conn_fd = socket_utils::accept(this->sock_fd_);
	if (conn_fd == -1)
		return (errno == EAGAIN) ? 0 : -1;

	io_handler *handler = this->on_new_connection(this->dispatcher_, conn_fd);
	if (handler)
		handler->on_connected();

	return 0;
}
void io_listener::on_disconnected()
{
	this->del_event(this->had_ev_);
}
