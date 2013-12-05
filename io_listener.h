#ifndef IO_LISTENER_H_
#define IO_LISTENER_H_

#include "io_handler.h"

class io_listener : public io_handler
{
	public:
		io_listener(event_dispatcher *dispatcher);
		virtual ~io_listener();

		virtual int handle_input();
		int listen(int port);

	protected:
		virtual int handle_output()		{ return 0; }
		virtual int on_connected()		{ return 0; }
		virtual int on_disconnected() { return 0; }

		virtual io_handler* new_connection(event_dispatcher *dispatcher, 
																			 int conn_fd) = 0;
};

#endif
