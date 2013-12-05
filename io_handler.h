#ifndef IO_HANDLER_H_
#define IO_HANDLER_H_

class event_dispatcher;

class io_handler
{
public:
  io_handler(event_dispatcher *dispatcher, int fd);
  virtual ~io_handler();

  virtual int handle_input() = 0;
  virtual int handle_output() = 0;
	int connected();
	int disconnected();

protected:
	int add_event(int ev);
	int del_event(int ev);

protected:
  int sock_fd_;
  int had_ev_;
  event_dispatcher *dispatcher_;

private:
  virtual int on_connected() = 0;
  virtual int on_disconnected() = 0; 
private:
  io_handler(const io_handler&);
  io_handler& operator= (const io_handler&);
};

#endif
