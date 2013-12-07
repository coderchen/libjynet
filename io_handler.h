#ifndef IO_HANDLER_H_
#define IO_HANDLER_H_

class event_dispatcher;

class io_handler
{
public:
	enum 
	{
		EV_NONE_MASK  = 0x0000,
		EV_READ_MASK  =	0X0001,
		EV_WRITE_MASK = 0X0002
	};
public:
  io_handler(event_dispatcher *dispatcher, int fd);
  virtual ~io_handler();

  virtual int handle_input() = 0;
  virtual int handle_output() = 0;
  virtual int on_connected() = 0;
  virtual int on_disconnected() = 0; 

	int get_sock_fd() const { return this->sock_fd_; }
	int ev_mask_2_epoll_ev(int ev_mask) const;

protected:
	int add_ev_mask(int ev_mask);
	int del_ev_mask(int ev_mask);

protected:
  int sock_fd_;
  int had_ev_mask_;
  event_dispatcher *dispatcher_;

private:
  io_handler(const io_handler&);
  io_handler& operator= (const io_handler&);
};

#endif
