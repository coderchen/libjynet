#include <iostream>
#include <jynet/io_handler.h>
#include <jynet/io_listener.h>
#include <jynet/event_dispatcher.h>
#include <jynet/socket_utils.h>

class test_client : public io_handler
{
	public:
		test_client(event_dispatcher *dispatcher, int fd)
			: io_handler(dispatcher, fd)
		{ }

		virtual int handle_input()
		{
			char buf[1024] = {0};
			socket_utils::recv(this->get_sock_fd(), buf, sizeof(buf));
			this->del_ev_mask(io_handler::EV_READ_MASK);
			return this->add_ev_mask(io_handler::EV_WRITE_MASK);
		}
		virtual int handle_output()
		{
			const char buf[] = "HTTP/1.0 200 OK\r\n"
				"Content-Length:0\r\n"
				"\r\n";
			socket_utils::send(this->get_sock_fd(), buf, sizeof(buf) - 1);
			return -1; // return -1, then will call on_disconnected
		}
		virtual void on_connected()
		{
			this->add_ev_mask(io_handler::EV_READ_MASK);
		}
		virtual void on_disconnected()
		{
		}
};
class my_listener : public io_listener
{
	public:
		my_listener(event_dispatcher *dispatcher)
			: io_listener(dispatcher)
		{ }

	protected:
		virtual io_handler* new_connection(event_dispatcher *dispatcher, int conn_fd)
		{
			return new test_client(dispatcher, conn_fd);
		}
};
int main()
{
	event_dispatcher ed;
	if (ed.init() != 0) {
		std::cout << "init err." << std::endl;
		return -1;
	}

	my_listener listener(&ed);
	if (listener.listen(5757) != 0) {
		std::cout << "listen err." << std::endl;
		return -1;
	}

	ed.run();
	return 0;
}
