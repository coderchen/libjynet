#ifndef SOCKET_UTILS_H_
#define SOCKET_UTILS_H_

class socket_utils
{
public:
  static int set_reuse_addr(int fd);
	static int set_nonblock(int fd);
  static int close(int fd);
  static int bind(int fd, int port);
  static int accept(int fd);
  static int recv(int fd, void *buf, int size);
  static int send(int fd, const void *buf, int size);
private:
  socket_utils();
};

#endif
