#include "socket_utils.h"

#include <sys/socket.h>
#include <errno.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int socket_utils::set_reuse_addr(int fd)
{
  int flag = 1;
  return ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const void*)&flag, sizeof(flag));
}
int socket_utils::set_nonblock(int fd)
{
	int flag = ::fcntl(fd, F_GETFL);
	if (flag == -1)
		return -1;

	flag |= O_NONBLOCK;
	return ::fcntl(fd, F_SETFL, flag);
}
int socket_utils::close(int fd)
{
  int ret = -1;
  do {
    ret = ::close(fd);
  } while (ret == -1 && errno == EINTR);
  
  return ret;
}
int socket_utils::bind(int fd, int port)
{
  sockaddr_in sin;
  ::memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_port = ::htons(port);
  sin.sin_addr.s_addr = ::htonl(INADDR_ANY);

  return ::bind(fd, (sockaddr*)&sin, sizeof(sin));
}
int socket_utils::accept(int fd)
{
  int conn_fd = -1;
  do {
    conn_fd = ::accept(fd, NULL, NULL);
  } while (conn_fd == -1 && errno == EINTR);

  return conn_fd;
}
int socket_utils::recv(int fd, void *buf, int size)
{
  int rcv_len = 0;
  do {
    rcv_len = ::recv(fd, buf, size, 0);
  } while (rcv_len == -1 && errno == EINTR);
  
  return rcv_len;
}
int socket_utils::send(int fd, const void *buf, int size)
{
  int snd_len = 0;
  do {
    snd_len = ::send(fd, buf, size, 0);
  } while (snd_len == -1 && errno == EINTR);

  return snd_len;
}
