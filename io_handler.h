#ifndef IO_HANDLER_H_
#define IO_HANDLER_H_

class event_dispatcher;

class io_handler
{
public:
  static int ev_read;
  static int ev_write;
public:
  io_handler(event_dispatcher *dispatcher, int fd);
  virtual ~io_handler();

  virtual int handle_input() = 0;
  virtual int handle_output() = 0;
  virtual void on_connected() = 0;
  virtual void on_disconnected() = 0; 

protected:
	int add_event(int ev);
	int del_event(int ev);

protected:
  int sock_fd_;
  int had_ev_;
  event_dispatcher *dispatcher_;

private:
  io_handler(const io_handler&);
  io_handler& operator= (const io_handler&);
};

#endif
